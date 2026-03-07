use anyhow::Result;
use std::collections::HashMap;
use std::path::Path;
use std::sync::Arc;

use arrow::array::{Int64Builder, StringBuilder, StructBuilder, ListBuilder};
use arrow::datatypes::{DataType, Field, Schema};
use arrow::record_batch::RecordBatch;
use parquet::arrow::arrow_writer::ArrowWriter;
use parquet::file::properties::WriterProperties;

use crate::compute::{AccelData, SeriesData, SeriesEvent};
use crate::ffi::{Arr, ComplexOf, IntervalOf, RealValue, Value};

// ---------------------------------------------------------------------------
// Public input types (named structs, no tuples)
// ---------------------------------------------------------------------------

/// A single series result row ready for export.
pub struct SeriesExportRow {
    pub series_id: i64,
    pub series_name: String,
    pub precision: String,
    /// Flat string map of series arguments (x + named args + noise args prefixed with "noise_").
    pub arguments: HashMap<String, String>,
    pub data: SeriesData,
}

/// A single accel result row ready for export.
pub struct AccelExportRow {
    pub series_id: i64,
    pub m_value: i64,
    pub accel_name: String,
    /// Flat string map of accel-specific arguments.
    pub arguments: HashMap<String, String>,
    pub data: AccelData,
}

/// All data collected during a headless run.
pub struct ExportData {
    pub series_results: Vec<SeriesExportRow>,
}

// ---------------------------------------------------------------------------
// Exporter
// ---------------------------------------------------------------------------

pub struct ParquetExporter;

impl ParquetExporter {
    /// Export all results with hive partitioning, matching the Python layout:
    ///
    /// ```text
    /// {path}/
    ///   series/
    ///     precision=F64/
    ///       series_name=Euler/
    ///         0.parquet   ← contains: series_id, arguments, series_limit, computed
    ///   accelerations/
    ///     series_id=0/
    ///       0.parquet     ← contains: accel_name, m_value, additional_args, computed,
    ///                                 errors, events, noise_str, filtered
    /// ```
    pub fn export(data: ExportData, path: &Path) -> Result<()> {
        std::fs::create_dir_all(path)?;
        if !data.series_results.is_empty() {
            Self::export_series_partitioned(&data.series_results, &path.join("series"))?;
        }
        Ok(())
    }

    /// Incrementally export a batch of acceleration result rows to prevent RAM blowup.
    /// This writes a unique file (using UUID) to the correct hive partitioned directory.
    pub fn export_incremental_accel_batch(series_id: i64, rows: &[AccelExportRow], path: &Path) -> Result<()> {
        if rows.is_empty() { return Ok(()); }
        let base = path.join("accelerations");
        let dir = base.join(format!("series_id={}", series_id));
        std::fs::create_dir_all(&dir)?;

        let id = uuid::Uuid::new_v4().to_string();
        let file_path = dir.join(format!("{}.parquet", id));

        // Note: For incremental, we may only know the keys present in *these* rows.
        let arg_keys = collect_arg_keys(rows.iter().map(|r| &r.arguments));
        
        let refs: Vec<&AccelExportRow> = rows.iter().collect();
        Self::write_accel_partition(&refs, &arg_keys, &file_path)?;

        Ok(())
    }

    // -----------------------------------------------------------------------
    // Series — hive partitioned by (precision, series_name)
    // -----------------------------------------------------------------------

    fn export_series_partitioned(rows: &[SeriesExportRow], base: &Path) -> Result<()> {
        // Compute global arg keys once so all partition files share the same schema.
        let arg_keys = collect_arg_keys(rows.iter().map(|r| &r.arguments));

        // Group rows by (precision, series_name).
        let mut partitions: std::collections::BTreeMap<
            (String, String),   // (precision, series_name)
            Vec<usize>,         // indices into `rows`
        > = std::collections::BTreeMap::new();
        for (i, row) in rows.iter().enumerate() {
            partitions
                .entry((row.precision.clone(), row.series_name.clone()))
                .or_default()
                .push(i);
        }

        for ((precision, series_name), indices) in &partitions {
            // Hive path: precision={p}/series_name={n}/0.parquet
            let dir = base
                .join(format!("precision={}", hive_escape(precision)))
                .join(format!("series_name={}", hive_escape(series_name)));
            std::fs::create_dir_all(&dir)?;

            let partition_rows: Vec<&SeriesExportRow> =
                indices.iter().map(|&i| &rows[i]).collect();

            Self::write_series_partition(&partition_rows, &arg_keys, &dir.join("0.parquet"))?;
        }
        Ok(())
    }

    /// Write one `series` partition file.
    /// Schema (partition cols `precision` and `series_name` are **not** in the file):
    ///   series_id, arguments, series_limit, computed
    fn write_series_partition(
        rows: &[&SeriesExportRow],
        arg_keys: &[String],
        path: &Path,
    ) -> Result<()> {
        let arg_fields = arg_key_fields(arg_keys);
        let real_imag_fields = real_imag_field_vec();
        let real_imag_struct = DataType::Struct(real_imag_fields.clone().into());
        let profiling_fields = profiling_field_vec();

        let computed_item_fields = vec![
            Field::new("n",         DataType::Int64,         false),
            Field::new("value",     real_imag_struct.clone(), false),
            Field::new("deviation", DataType::Utf8,           false),
            Field::new("profiling", DataType::Struct(profiling_fields.clone().into()), true),
        ];

        // Physical schema matching Python exactly
        let schema = Arc::new(Schema::new(vec![
            Field::new("series_name",  DataType::Utf8, false),
            Field::new("series_id",    DataType::Int64, false),
            Field::new("precision",    DataType::Utf8, false),
            Field::new("arguments",    DataType::Struct(arg_fields.clone().into()), false),
            Field::new("series_limit", real_imag_struct.clone(), true),
            Field::new("computed",     list_field("item", &computed_item_fields), false),
        ]));

        let mut name_builder = StringBuilder::new();
        let mut id_builder = Int64Builder::new();
        let mut prec_builder = StringBuilder::new();
        let mut args_builder = make_args_builder(&arg_fields);
        let mut limit_builder = StructBuilder::new(
            real_imag_fields.clone(),
            vec![Box::new(StringBuilder::new()), Box::new(StringBuilder::new())],
        );
        let mut computed_builder = ListBuilder::new(StructBuilder::new(
            computed_item_fields.clone(),
            vec![
                Box::new(Int64Builder::new()),
                Box::new(make_real_imag_builder(&real_imag_fields)),
                Box::new(StringBuilder::new()),
                Box::new(make_profiling_builder(&profiling_fields)),
            ],
        ));

        for row in rows {
            name_builder.append_value(&row.series_name);
            id_builder.append_value(row.series_id);
            prec_builder.append_value(&row.precision);
            append_args_struct(&mut args_builder, arg_keys, &row.arguments);

            // series_limit (nullable)
            match &row.data.sum {
                Some(val) => {
                    let (r, i) = value_to_strings(val);
                    limit_builder.field_builder::<StringBuilder>(0).unwrap().append_value(r);
                    limit_builder.field_builder::<StringBuilder>(1).unwrap().append_value(i);
                    limit_builder.append(true);
                }
                None => {
                    limit_builder.field_builder::<StringBuilder>(0).unwrap().append_null();
                    limit_builder.field_builder::<StringBuilder>(1).unwrap().append_null();
                    limit_builder.append(false);
                }
            }

            // computed list
            let n_pts = arr_len(&row.data.result.sn);
            let lb = computed_builder.values();
            for idx in 0..n_pts {
                lb.field_builder::<Int64Builder>(0).unwrap().append_value(idx as i64);

                let (r, i) = arr_index_to_strings(&row.data.result.sn, idx);
                let val_sb = lb.field_builder::<StructBuilder>(1).unwrap();
                val_sb.field_builder::<StringBuilder>(0).unwrap().append_value(r);
                val_sb.field_builder::<StringBuilder>(1).unwrap().append_value(i);
                val_sb.append(true);

                let dev = arr_index_f64(&row.data.result.deviations, idx)
                    .map(|v| v.to_string())
                    .unwrap_or_default();
                lb.field_builder::<StringBuilder>(2).unwrap().append_value(dev);

                // No profiling on the series side — write null struct
                append_null_profiling(lb.field_builder::<StructBuilder>(3).unwrap());

                lb.append(true);
            }
            computed_builder.append(true);
        }

        finish_and_write(schema, vec![
            Arc::new(name_builder.finish()),
            Arc::new(id_builder.finish()),
            Arc::new(prec_builder.finish()),
            Arc::new(args_builder.finish()),
            Arc::new(limit_builder.finish()),
            Arc::new(computed_builder.finish()),
        ], path)
    }

    // -----------------------------------------------------------------------
    // Accelerations — hive partitioned by series_id
    // -----------------------------------------------------------------------

    /// Write one `accelerations` partition file.
    /// Schema (partition col `series_id` is **not** in the file):
    ///   accel_name, m_value, additional_args, computed, errors, events, noise_str, filtered
    fn write_accel_partition(
        rows: &[&AccelExportRow],
        arg_keys: &[String],
        path: &Path,
    ) -> Result<()> {
        let arg_fields = arg_key_fields(arg_keys);
        let real_imag_fields = real_imag_field_vec();
        let real_imag_struct = DataType::Struct(real_imag_fields.clone().into());
        let profiling_fields = profiling_field_vec();

        let computed_item_fields = vec![
            Field::new("value",     real_imag_struct.clone(), false),
            Field::new("deviation", DataType::Utf8,           false),
            Field::new("profiling", DataType::Struct(profiling_fields.clone().into()), true),
        ];
        let error_fields = vec![
            Field::new("n",       DataType::Int64, false),
            Field::new("message", DataType::Utf8,  false),
        ];
        let event_fields = vec![
            Field::new("n",           DataType::Int64, false),
            Field::new("name",        DataType::Utf8,  false),
            Field::new("description", DataType::Utf8,  false),
        ];
        let method_res_type_fields = vec![
            Field::new("values",  list_field("item", &real_imag_fields), true),
            Field::new("average", DataType::Struct(real_imag_fields.clone().into()), true),
        ];
        let methods_dummy_fields = vec![
            Field::new("__dummy__", DataType::Struct(method_res_type_fields.clone().into()), true)
        ];
        let filtered_fields = vec![
            Field::new("start_n",        DataType::Int64, false),
            Field::new("segment_length", DataType::Int64, false),
            Field::new("methods",        DataType::Struct(methods_dummy_fields.clone().into()), true),
        ];

        // Physical schema matching Python exactly
        let schema = Arc::new(Schema::new(vec![
            Field::new("series_id",       DataType::Int64, false),
            Field::new("accel_name",      DataType::Utf8,  false),
            Field::new("m_value",         DataType::Int64, false),
            Field::new("additional_args", DataType::Struct(arg_fields.clone().into()), false),
            Field::new("computed",  list_field("item", &computed_item_fields), false),
            Field::new("errors",    list_field("item", &error_fields),          false),
            Field::new("events",    list_field("item", &event_fields),           false),
            Field::new("filtered",  DataType::Struct(filtered_fields.clone().into()), true),
        ]));

        let mut sid_builder  = Int64Builder::new();
        let mut name_builder = StringBuilder::new();
        let mut m_builder    = Int64Builder::new();
        let mut args_builder = make_args_builder(&arg_fields);

        let mut computed_builder = ListBuilder::new(StructBuilder::new(
            computed_item_fields.clone(),
            vec![
                Box::new(make_real_imag_builder(&real_imag_fields)),
                Box::new(StringBuilder::new()),
                Box::new(make_profiling_builder(&profiling_fields)),
            ],
        ));
        let mut errors_builder = ListBuilder::new(StructBuilder::new(
            error_fields.clone(),
            vec![Box::new(Int64Builder::new()), Box::new(StringBuilder::new())],
        ));
        let mut events_builder = ListBuilder::new(StructBuilder::new(
            event_fields.clone(),
            vec![
                Box::new(Int64Builder::new()),
                Box::new(StringBuilder::new()),
                Box::new(StringBuilder::new()),
            ],
        ));
        // filtered is always null in the current pipeline (no filter data yet)
        let mut filtered_builder = StructBuilder::new(
            filtered_fields.clone(),
            vec![
                Box::new(Int64Builder::new()),
                Box::new(Int64Builder::new()),
                Box::new(StructBuilder::new(
                    methods_dummy_fields.clone(),
                    vec![Box::new(StructBuilder::new(
                        method_res_type_fields.clone(),
                        vec![
                            Box::new(ListBuilder::new(make_real_imag_builder(&real_imag_fields))),
                            Box::new(make_real_imag_builder(&real_imag_fields)),
                        ]
                    ))],
                )),
            ],
        );

        for row in rows {
            sid_builder.append_value(row.series_id);
            name_builder.append_value(&row.accel_name);
            m_builder.append_value(row.m_value);
            append_args_struct(&mut args_builder, arg_keys, &row.arguments);

            // computed
            let n_pts = arr_len(&row.data.result.sn);
            let lb = computed_builder.values();
            for idx in 0..n_pts {
                let (r, i) = arr_index_to_strings(&row.data.result.sn, idx);
                let val_sb = lb.field_builder::<StructBuilder>(0).unwrap();
                val_sb.field_builder::<StringBuilder>(0).unwrap().append_value(r);
                val_sb.field_builder::<StringBuilder>(1).unwrap().append_value(i);
                val_sb.append(true);

                let dev = arr_index_f64(&row.data.result.deviations, idx)
                    .map(|v| v.to_string())
                    .unwrap_or_default();
                lb.field_builder::<StringBuilder>(1).unwrap().append_value(dev);

                append_null_profiling(lb.field_builder::<StructBuilder>(2).unwrap());
                lb.append(true);
            }
            computed_builder.append(true);

            // errors (events named "error") vs regular events
            let (algo_errors, events): (Vec<&SeriesEvent>, Vec<&SeriesEvent>) =
                row.data.events.iter().partition(|e| e.name == "error");

            let err_lb = errors_builder.values();
            for ev in algo_errors {
                err_lb.field_builder::<Int64Builder>(0).unwrap().append_value(ev.n as i64);
                err_lb.field_builder::<StringBuilder>(1).unwrap().append_value(&ev.description);
                err_lb.append(true);
            }
            errors_builder.append(true);

            let evt_lb = events_builder.values();
            for ev in events {
                evt_lb.field_builder::<Int64Builder>(0).unwrap().append_value(ev.n as i64);
                evt_lb.field_builder::<StringBuilder>(1).unwrap().append_value(&ev.name);
                evt_lb.field_builder::<StringBuilder>(2).unwrap().append_value(&ev.description);
                evt_lb.append(true);
            }
            events_builder.append(true);

            // filtered — always null
            filtered_builder.field_builder::<Int64Builder>(0).unwrap().append_null();
            filtered_builder.field_builder::<Int64Builder>(1).unwrap().append_null();
            let methods_sb = filtered_builder.field_builder::<StructBuilder>(2).unwrap();
            let dummy_sb = methods_sb.field_builder::<StructBuilder>(0).unwrap();
            
            let val_list = dummy_sb.field_builder::<ListBuilder<StructBuilder>>(0).unwrap();
            val_list.append(false); // list itself is null
            let avg_sb = dummy_sb.field_builder::<StructBuilder>(1).unwrap();
            avg_sb.field_builder::<StringBuilder>(0).unwrap().append_null();
            avg_sb.field_builder::<StringBuilder>(1).unwrap().append_null();
            avg_sb.append(false);
            
            dummy_sb.append(false);
            methods_sb.append(false);
            filtered_builder.append(false);
        }

        finish_and_write(schema, vec![
            Arc::new(sid_builder.finish()),
            Arc::new(name_builder.finish()),
            Arc::new(m_builder.finish()),
            Arc::new(args_builder.finish()),
            Arc::new(computed_builder.finish()),
            Arc::new(errors_builder.finish()),
            Arc::new(events_builder.finish()),
            Arc::new(filtered_builder.finish()),
        ], path)
    }
}

// ---------------------------------------------------------------------------
// Schema / builder helpers
// ---------------------------------------------------------------------------

fn real_imag_field_vec() -> Vec<Field> {
    vec![
        Field::new("real", DataType::Utf8, false),
        Field::new("imag", DataType::Utf8, false),
    ]
}

fn profiling_field_vec() -> Vec<Field> {
    vec![
        Field::new("add",     DataType::Int64, true),
        Field::new("mul",     DataType::Int64, true),
        Field::new("div",     DataType::Int64, true),
        Field::new("special", DataType::Int64, true),
    ]
}

fn arg_key_fields(keys: &[String]) -> Vec<Field> {
    keys.iter().map(|k| Field::new(k, DataType::Utf8, true)).collect()
}

/// Build a `DataType::List` field with the given item struct fields (nullable items).
fn list_field(item_name: &str, item_fields: &[Field]) -> DataType {
    DataType::List(Arc::new(Field::new(
        item_name,
        DataType::Struct(item_fields.to_vec().into()),
        true,
    )))
}

fn make_args_builder(fields: &[Field]) -> StructBuilder {
    let builders: Vec<Box<dyn arrow::array::ArrayBuilder>> = fields
        .iter()
        .map(|_| Box::new(StringBuilder::new()) as Box<dyn arrow::array::ArrayBuilder>)
        .collect();
    StructBuilder::new(fields.to_vec(), builders)
}

fn make_real_imag_builder(fields: &[Field]) -> StructBuilder {
    StructBuilder::new(
        fields.to_vec(),
        vec![Box::new(StringBuilder::new()), Box::new(StringBuilder::new())],
    )
}

fn make_profiling_builder(fields: &[Field]) -> StructBuilder {
    StructBuilder::new(
        fields.to_vec(),
        vec![
            Box::new(Int64Builder::new()),
            Box::new(Int64Builder::new()),
            Box::new(Int64Builder::new()),
            Box::new(Int64Builder::new()),
        ],
    )
}

/// Append all-null profiling struct entry.
fn append_null_profiling(sb: &mut StructBuilder) {
    sb.field_builder::<Int64Builder>(0).unwrap().append_null();
    sb.field_builder::<Int64Builder>(1).unwrap().append_null();
    sb.field_builder::<Int64Builder>(2).unwrap().append_null();
    sb.field_builder::<Int64Builder>(3).unwrap().append_null();
    sb.append(false);
}

// ---------------------------------------------------------------------------
// Data helpers
// ---------------------------------------------------------------------------

/// Collect all unique argument keys across all rows, sorted.
fn collect_arg_keys<'a>(
    iter: impl Iterator<Item = &'a HashMap<String, String>>,
) -> Vec<String> {
    let mut keys: std::collections::BTreeSet<String> = std::collections::BTreeSet::new();
    for map in iter {
        for k in map.keys() {
            keys.insert(k.clone());
        }
    }
    if keys.is_empty() {
        keys.insert("__dummy__".to_string());
    }
    keys.into_iter().collect()
}

/// Append one entry to a dynamic-key string struct builder.
fn append_args_struct(
    builder: &mut StructBuilder,
    keys: &[String],
    args: &HashMap<String, String>,
) {
    for (i, key) in keys.iter().enumerate() {
        let b = builder.field_builder::<StringBuilder>(i).expect("Missing builder");
        match args.get(key) {
            Some(val) => b.append_value(val),
            None      => b.append_null(),
        }
    }
    builder.append(true);
}

/// Number of elements in an `Arr`.
fn arr_len(arr: &Arr) -> usize {
    match arr {
        Arr::Real(v)      => v.len(),
        Arr::Complex(c)   => c.real.len(),
        Arr::Interval(iv) => iv.inf.len(),
        Arr::CInterval(c) => c.real.inf.len(),
    }
}

/// Get the `idx`-th element of an `Arr` as `(real_str, imag_str)`.
fn arr_index_to_strings(arr: &Arr, idx: usize) -> (String, String) {
    let zero = RealValue { mantissa: 0.0, exponent: 0 };
    match arr {
        Arr::Real(v) => {
            let rv = v.get(idx).copied().unwrap_or(zero);
            (rv.to_f64().to_string(), "0.0".to_string())
        }
        Arr::Complex(c) => {
            let r = c.real.get(idx).copied().unwrap_or(zero);
            let i = c.imag.get(idx).copied().unwrap_or(zero);
            (r.to_f64().to_string(), i.to_f64().to_string())
        }
        Arr::Interval(iv) => {
            let inf = iv.inf.get(idx).copied().unwrap_or(zero);
            let sup = iv.sup.get(idx).copied().unwrap_or(zero);
            let mid = (inf.to_f64() + sup.to_f64()) / 2.0;
            (mid.to_string(), "0.0".to_string())
        }
        Arr::CInterval(c) => {
            let ri  = c.real.inf.get(idx).copied().unwrap_or(zero);
            let rs  = c.real.sup.get(idx).copied().unwrap_or(zero);
            let ii  = c.imag.inf.get(idx).copied().unwrap_or(zero);
            let is_ = c.imag.sup.get(idx).copied().unwrap_or(zero);
            ((ri.to_f64() + rs.to_f64()) / 2.0).to_string()
                .pipe(|r_mid| (r_mid, ((ii.to_f64() + is_.to_f64()) / 2.0).to_string()))
        }
    }
}

/// Get the `idx`-th element of an `Arr` as a plain `f64` (used for deviations).
fn arr_index_f64(arr: &Arr, idx: usize) -> Option<f64> {
    match arr {
        Arr::Real(v)      => v.get(idx).map(|rv| rv.to_f64()),
        Arr::Complex(c)   => c.real.get(idx).map(|rv| rv.to_f64()),
        Arr::Interval(iv) => iv.inf.get(idx).map(|rv| rv.to_f64()),
        Arr::CInterval(c) => c.real.inf.get(idx).map(|rv| rv.to_f64()),
    }
}

/// Convert a `Value` (series limit) to `(real_str, imag_str)`.
fn value_to_strings(val: &Value) -> (String, String) {
    match val {
        Value::Real(rv) => (rv.to_f64().to_string(), "0.0".to_string()),
        Value::Complex(ComplexOf { real, imag }) => {
            (real.to_f64().to_string(), imag.to_f64().to_string())
        }
        Value::Interval(IntervalOf { inf, sup }) => {
            let mid = (inf.to_f64() + sup.to_f64()) / 2.0;
            (mid.to_string(), "0.0".to_string())
        }
        Value::CInterval(c) => {
            let r_mid = (c.real.inf.to_f64() + c.real.sup.to_f64()) / 2.0;
            let i_mid = (c.imag.inf.to_f64() + c.imag.sup.to_f64()) / 2.0;
            (r_mid.to_string(), i_mid.to_string())
        }
    }
}

/// Escape a hive partition component (URL-encode spaces and slashes).
fn hive_escape(s: &str) -> String {
    s.replace('/', "%2F").replace(' ', "%20")
}

/// Write a `RecordBatch` to a parquet file with ZSTD compression.
fn finish_and_write(
    schema: Arc<Schema>,
    columns: Vec<Arc<dyn arrow::array::Array>>,
    path: &Path,
) -> Result<()> {
    let batch = RecordBatch::try_new(schema.clone(), columns)?;
    let file = std::fs::File::create(path)?;
    let props = WriterProperties::builder()
        .set_compression(parquet::basic::Compression::ZSTD(Default::default()))
        .build();
    let mut writer = ArrowWriter::try_new(file, schema, Some(props))?;
    writer.write(&batch)?;
    writer.close()?;
    Ok(())
}

// ---------------------------------------------------------------------------
// Tiny pipe helper (avoids a temp variable in the CInterval arm)
// ---------------------------------------------------------------------------
trait Pipe: Sized {
    fn pipe<R>(self, f: impl FnOnce(Self) -> R) -> R {
        f(self)
    }
}
impl<T> Pipe for T {}
