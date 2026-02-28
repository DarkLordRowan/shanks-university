use anyhow::Result;
use std::collections::HashMap;
use std::path::Path;
use std::sync::Arc;

use arrow::array::{
    Int64Builder, StringBuilder, StructBuilder, ListBuilder,
};
use arrow::datatypes::{DataType, Field, Schema};
use arrow::record_batch::RecordBatch;
use parquet::arrow::arrow_writer::ArrowWriter;
use parquet::file::properties::WriterProperties;

use crate::ffi::{SeriesResult, AccelResult, SeriesPoint};

/// Data collected during the headless run for export.
pub struct ExportData {
    pub series_results: Vec<(i64, String, String, HashMap<String, String>, SeriesResult)>,
    pub accel_results: Vec<(i64, String, i64, String, HashMap<String, String>, AccelResult)>,
}

/// Data for a single partition (series_id).
pub struct PartitionData {
    pub series_id: i64,
    pub series_result: (i64, String, String, HashMap<String, String>, SeriesResult),
    pub accel_results: Vec<(i64, String, i64, String, HashMap<String, String>, AccelResult)>,
}

pub struct ParquetExporter;

impl ParquetExporter {
    pub fn export(data: ExportData, path: &Path) -> Result<()> {
        std::fs::create_dir_all(path)?;

        Self::export_series(&data.series_results, &path.join("series.parquet"))?;
        Self::export_accelerations(&data.accel_results, &path.join("accelerations.parquet"))?;

        Ok(())
    }

    pub fn export_partition(data: PartitionData, path: &Path) -> Result<()> {
        let series_path = path.join("series").join(format!("series_id={}", data.series_id));
        std::fs::create_dir_all(&series_path)?;
        Self::export_series(&[data.series_result], &series_path.join("series.parquet"))?;

        let accel_path = path.join("accelerations").join(format!("series_id={}", data.series_id));
        std::fs::create_dir_all(&accel_path)?;
        Self::export_accelerations(&data.accel_results, &accel_path.join("accelerations.parquet"))?;

        Ok(())
    }

    fn export_series(
        results: &[(i64, String, String, HashMap<String, String>, SeriesResult)],
        path: &Path,
    ) -> Result<()> {
        // Identify all unique argument keys
        let mut arg_keys: std::collections::BTreeSet<String> = std::collections::BTreeSet::new();
        for (_, _, _, args, _) in results {
            for k in args.keys() {
                arg_keys.insert(k.clone());
            }
        }
        if arg_keys.is_empty() {
            arg_keys.insert("_placeholder".to_string());
        }
        let arg_fields: Vec<Field> = arg_keys.iter()
            .map(|k| Field::new(k, DataType::Utf8, true))
            .collect();
        let arg_struct_type = DataType::Struct(arg_fields.clone().into());

        // Schema definition matching to_parquet.py
        let real_imag_fields = vec![
            Field::new("real", DataType::Utf8, false),
            Field::new("imag", DataType::Utf8, false),
        ];
        let real_imag_struct = DataType::Struct(real_imag_fields.clone().into());

        let profiling_fields = vec![
            Field::new("add", DataType::Int64, false),
            Field::new("mul", DataType::Int64, false),
            Field::new("div", DataType::Int64, false),
            Field::new("special", DataType::Int64, false),
        ];

        let computed_fields = vec![
            Field::new("n", DataType::Int64, false),
            Field::new("value", real_imag_struct.clone(), false),
            Field::new("deviation", DataType::Utf8, false),
            Field::new("profiling", DataType::Struct(profiling_fields.into()), true),
        ];

        let schema = Arc::new(Schema::new(vec![
            Field::new("series_name", DataType::Utf8, false),
            Field::new("series_id", DataType::Int64, false),
            Field::new("precision", DataType::Utf8, false),
            Field::new("arguments", arg_struct_type.clone(), false),
            Field::new("series_limit", real_imag_struct.clone(), true),
            Field::new("computed", DataType::List(Arc::new(Field::new("item", DataType::Struct(computed_fields.clone().into()), true))), false),
        ]));

        // Builders
        let mut name_builder = StringBuilder::new();
        let mut id_builder = Int64Builder::new();
        let mut prec_builder = StringBuilder::new();
        
        let arg_builders: Vec<Box<dyn arrow::array::ArrayBuilder>> = arg_fields.iter()
            .map(|_| Box::new(StringBuilder::new()) as Box<dyn arrow::array::ArrayBuilder>)
            .collect();
        let mut args_builder = StructBuilder::new(arg_fields, arg_builders);
        
        let limit_builders: Vec<Box<dyn arrow::array::ArrayBuilder>> = vec![
            Box::new(StringBuilder::new()),
            Box::new(StringBuilder::new()),
        ];
        let mut limit_builder = StructBuilder::new(real_imag_fields.clone(), limit_builders);
        
        let computed_item_builders: Vec<Box<dyn arrow::array::ArrayBuilder>> = vec![
            Box::new(Int64Builder::new()),
            Box::new(StructBuilder::new(real_imag_fields, vec![
                Box::new(StringBuilder::new()),
                Box::new(StringBuilder::new()),
            ])),
            Box::new(StringBuilder::new()),
            Box::new(StructBuilder::new(vec![
                Field::new("add", DataType::Int64, false),
                Field::new("mul", DataType::Int64, false),
                Field::new("div", DataType::Int64, false),
                Field::new("special", DataType::Int64, false),
            ], vec![
                Box::new(Int64Builder::new()),
                Box::new(Int64Builder::new()),
                Box::new(Int64Builder::new()),
                Box::new(Int64Builder::new()),
            ])),
        ];
        
        let mut computed_builder = ListBuilder::new(StructBuilder::new(computed_fields, computed_item_builders));

        for (id, name, prec, args, res) in results {
            name_builder.append_value(name);
            id_builder.append_value(*id);
            prec_builder.append_value(prec);
            
            for (i, key) in arg_keys.iter().enumerate() {
                let builder = args_builder.field_builder::<StringBuilder>(i).expect("Missing builder");
                if let Some(val) = args.get(key) {
                    builder.append_value(val);
                } else {
                    builder.append_null();
                }
            }
            args_builder.append(true);

            if let Some(ref lim) = res.sum {
                let (r, i) = self::point_to_strings(lim);
                limit_builder.field_builder::<StringBuilder>(0).unwrap().append_value(r);
                limit_builder.field_builder::<StringBuilder>(1).unwrap().append_value(i);
                limit_builder.append(true);
            } else {
                limit_builder.field_builder::<StringBuilder>(0).unwrap().append_null();
                limit_builder.field_builder::<StringBuilder>(1).unwrap().append_null();
                limit_builder.append(false);
            }

            let list_item_builder = computed_builder.values();
            for idx in 0..res.sn.len() {
                let val = res.sn.get(idx);
                list_item_builder.field_builder::<Int64Builder>(0).unwrap().append_value(idx as i64);
                
                let (r, i) = point_to_strings(&val);
                let val_struct_builder = list_item_builder.field_builder::<StructBuilder>(1).unwrap();
                val_struct_builder.field_builder::<StringBuilder>(0).unwrap().append_value(r);
                val_struct_builder.field_builder::<StringBuilder>(1).unwrap().append_value(i);
                val_struct_builder.append(true);

                list_item_builder.field_builder::<StringBuilder>(2).unwrap().append_value("");
                
                let prof_struct_builder = list_item_builder.field_builder::<StructBuilder>(3).unwrap();
                prof_struct_builder.field_builder::<Int64Builder>(0).unwrap().append_null();
                prof_struct_builder.field_builder::<Int64Builder>(1).unwrap().append_null();
                prof_struct_builder.field_builder::<Int64Builder>(2).unwrap().append_null();
                prof_struct_builder.field_builder::<Int64Builder>(3).unwrap().append_null();
                prof_struct_builder.append(false);
                
                list_item_builder.append(true);
            }
            computed_builder.append(true);
        }

        let batch = RecordBatch::try_new(schema.clone(), vec![
            Arc::new(name_builder.finish()),
            Arc::new(id_builder.finish()),
            Arc::new(prec_builder.finish()),
            Arc::new(args_builder.finish()),
            Arc::new(limit_builder.finish()),
            Arc::new(computed_builder.finish()),
        ])?;

        let file = std::fs::File::create(path)?;
        let props = WriterProperties::builder()
            .set_compression(parquet::basic::Compression::ZSTD(Default::default()))
            .build();
        let mut writer = ArrowWriter::try_new(file, schema, Some(props))?;
        writer.write(&batch)?;
        writer.close()?;

        Ok(())
    }

    fn export_accelerations(
        results: &[(i64, String, i64, String, HashMap<String, String>, AccelResult)],
        path: &Path,
    ) -> Result<()> {
        // Identify all unique argument keys
        let mut arg_keys: std::collections::BTreeSet<String> = std::collections::BTreeSet::new();
        for (_, _, _, _, args, _) in results {
            for k in args.keys() {
                arg_keys.insert(k.clone());
            }
        }
        if arg_keys.is_empty() {
            arg_keys.insert("_placeholder".to_string());
        }
        let arg_fields: Vec<Field> = arg_keys.iter()
            .map(|k| Field::new(k, DataType::Utf8, true))
            .collect();
        let arg_struct_type = DataType::Struct(arg_fields.clone().into());

        let real_imag_fields = vec![
            Field::new("real", DataType::Utf8, false),
            Field::new("imag", DataType::Utf8, false),
        ];
        let real_imag_struct = DataType::Struct(real_imag_fields.clone().into());

        let profiling_fields = vec![
            Field::new("add", DataType::Int64, false),
            Field::new("mul", DataType::Int64, false),
            Field::new("div", DataType::Int64, false),
            Field::new("special", DataType::Int64, false),
        ];

        let computed_fields = vec![
            Field::new("value", real_imag_struct.clone(), false),
            Field::new("deviation", DataType::Utf8, false),
            Field::new("profiling", DataType::Struct(profiling_fields.into()), true),
        ];

        let error_fields = vec![
            Field::new("n", DataType::Int64, false),
            Field::new("message", DataType::Utf8, false),
        ];
        let event_fields = vec![
            Field::new("n", DataType::Int64, false),
            Field::new("name", DataType::Utf8, false),
            Field::new("description", DataType::Utf8, false),
        ];

        // Placeholder for filtered struct - just a dummy field for now
        let filtered_fields = vec![
            Field::new("start_n", DataType::Int64, false),
            Field::new("segment_length", DataType::Int64, false),
            Field::new("methods", DataType::Struct(vec![Field::new("__dummy__", DataType::Utf8, true)].into()), true),
        ];

        let schema = Arc::new(Schema::new(vec![
            Field::new("series_id", DataType::Int64, false),
            Field::new("accel_name", DataType::Utf8, false),
            Field::new("m_value", DataType::Int64, false),
            Field::new("additional_args", arg_struct_type.clone(), false),
            Field::new("computed", DataType::List(Arc::new(Field::new("item", DataType::Struct(computed_fields.clone().into()), true))), false),
            Field::new("errors", DataType::List(Arc::new(Field::new("item", DataType::Struct(error_fields.clone().into()), true))), false),
            Field::new("events", DataType::List(Arc::new(Field::new("item", DataType::Struct(event_fields.clone().into()), true))), false),
            Field::new("noise_str", DataType::Utf8, false),
            Field::new("filtered", DataType::Struct(filtered_fields.clone().into()), true),
        ]));

        let mut id_builder = Int64Builder::new();
        let mut name_builder = StringBuilder::new();
        let mut m_builder = Int64Builder::new();
        
        let arg_builders: Vec<Box<dyn arrow::array::ArrayBuilder>> = arg_keys.iter()
            .map(|_| Box::new(StringBuilder::new()) as Box<dyn arrow::array::ArrayBuilder>)
            .collect();
        let mut args_builder = StructBuilder::new(arg_fields, arg_builders);
        
        let computed_item_builders: Vec<Box<dyn arrow::array::ArrayBuilder>> = vec![
            Box::new(StructBuilder::new(real_imag_fields.clone(), vec![
                Box::new(StringBuilder::new()),
                Box::new(StringBuilder::new()),
            ])),
            Box::new(StringBuilder::new()),
            Box::new(StructBuilder::new(vec![
                Field::new("add", DataType::Int64, false),
                Field::new("mul", DataType::Int64, false),
                Field::new("div", DataType::Int64, false),
                Field::new("special", DataType::Int64, false),
            ], vec![
                Box::new(Int64Builder::new()),
                Box::new(Int64Builder::new()),
                Box::new(Int64Builder::new()),
                Box::new(Int64Builder::new()),
            ])),
        ];
        let mut computed_builder = ListBuilder::new(StructBuilder::new(computed_fields, computed_item_builders));

        let mut errors_builder = ListBuilder::new(StructBuilder::new(error_fields, vec![
            Box::new(Int64Builder::new()),
            Box::new(StringBuilder::new()),
        ]));
        let mut events_builder = ListBuilder::new(StructBuilder::new(event_fields, vec![
            Box::new(Int64Builder::new()),
            Box::new(StringBuilder::new()),
            Box::new(StringBuilder::new()),
        ]));
        let mut noise_builder = StringBuilder::new();
        
        let mut filtered_builder = StructBuilder::new(filtered_fields, vec![
            Box::new(Int64Builder::new()),
            Box::new(Int64Builder::new()),
            Box::new(StructBuilder::new(vec![Field::new("__dummy__", DataType::Utf8, true)], vec![Box::new(StringBuilder::new())])),
        ]);

        for (id, name, m, _prec, args, res) in results {
            id_builder.append_value(*id);
            name_builder.append_value(name);
            m_builder.append_value(*m);
            
            for (i, key) in arg_keys.iter().enumerate() {
                let builder = args_builder.field_builder::<StringBuilder>(i).expect("Missing builder");
                if let Some(val) = args.get(key) {
                    builder.append_value(val);
                } else {
                    builder.append_null();
                }
            }
            args_builder.append(true);

            let list_item_builder = computed_builder.values();
            for i in 0..res.values.len() {
                let (r, i_val) = point_to_strings(&res.values.get(i));
                let val_struct_builder = list_item_builder.field_builder::<StructBuilder>(0).unwrap();
                val_struct_builder.field_builder::<StringBuilder>(0).unwrap().append_value(r);
                val_struct_builder.field_builder::<StringBuilder>(1).unwrap().append_value(i_val);
                val_struct_builder.append(true);

                list_item_builder.field_builder::<StringBuilder>(1).unwrap().append_value(res.deviations.as_f64(i).to_string());
                
                if let Some(ref prof) = res.profiling {
                    let prof_struct_builder = list_item_builder.field_builder::<StructBuilder>(2).unwrap();
                    prof_struct_builder.field_builder::<Int64Builder>(0).unwrap().append_value(prof.add.get(i).copied().unwrap_or(0) as i64);
                    prof_struct_builder.field_builder::<Int64Builder>(1).unwrap().append_value(prof.mul.get(i).copied().unwrap_or(0) as i64);
                    prof_struct_builder.field_builder::<Int64Builder>(2).unwrap().append_value(prof.div.get(i).copied().unwrap_or(0) as i64);
                    prof_struct_builder.field_builder::<Int64Builder>(3).unwrap().append_value(prof.special.get(i).copied().unwrap_or(0) as i64);
                    prof_struct_builder.append(true);
                } else {
                    let prof_struct_builder = list_item_builder.field_builder::<StructBuilder>(2).unwrap();
                    prof_struct_builder.field_builder::<Int64Builder>(0).unwrap().append_null();
                    prof_struct_builder.field_builder::<Int64Builder>(1).unwrap().append_null();
                    prof_struct_builder.field_builder::<Int64Builder>(2).unwrap().append_null();
                    prof_struct_builder.field_builder::<Int64Builder>(3).unwrap().append_null();
                    prof_struct_builder.append(false);
                }
                
                list_item_builder.append(true);
            }
            computed_builder.append(true);

            // Empty placeholders for errors, events, filtered
            errors_builder.append(true);
            events_builder.append(true);
            noise_builder.append_value("None");
            
            // Populate filtered with nulls
            filtered_builder.field_builder::<Int64Builder>(0).unwrap().append_null();
            filtered_builder.field_builder::<Int64Builder>(1).unwrap().append_null();
            let methods_dummy = filtered_builder.field_builder::<StructBuilder>(2).unwrap();
            methods_dummy.field_builder::<StringBuilder>(0).unwrap().append_null();
            methods_dummy.append(false);
            filtered_builder.append(false);
        }

        let batch = RecordBatch::try_new(schema.clone(), vec![
            Arc::new(id_builder.finish()),
            Arc::new(name_builder.finish()),
            Arc::new(m_builder.finish()),
            Arc::new(args_builder.finish()),
            Arc::new(computed_builder.finish()),
            Arc::new(errors_builder.finish()),
            Arc::new(events_builder.finish()),
            Arc::new(noise_builder.finish()),
            Arc::new(filtered_builder.finish()),
        ])?;

        let file = std::fs::File::create(path)?;
        let props = WriterProperties::builder()
            .set_compression(parquet::basic::Compression::ZSTD(Default::default()))
            .build();
        let mut writer = ArrowWriter::try_new(file, schema, Some(props))?;
        writer.write(&batch)?;
        writer.close()?;

        Ok(())
    }
}

fn point_to_strings(point: &SeriesPoint) -> (String, String) {
    match point {
        SeriesPoint::Real(r) => (r.format(), "0.0".to_string()),
        SeriesPoint::Complex(c) => (c.real.format(), c.imag.format()),
        SeriesPoint::Interval(_) | SeriesPoint::CInterval(_) => {
            (point.magnitude().to_string(), "0.0".to_string())
        }
    }
}
