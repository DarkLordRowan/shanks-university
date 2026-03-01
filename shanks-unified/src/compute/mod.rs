//! Async compute pipeline. the single entry point for all series computation.

use anyhow::Result;
use std::collections::BTreeMap;
use tokio::sync::mpsc;
use uuid::Uuid;

use crate::{
    cache::{Cache, CachedEstimate, CachedEvent, RawArrBlobs},
    experiment::{
        Accel, AccelInstance, FilterDef, FilterInstance, NoiseDef, NoiseInstance, SeriesInstance,
    },
    ffi::{
        Arr, ComplexOf, IntervalOf, ParamValue, Value, ValueOf,
        bridge::ffi::{self as bridge, ArrKind, RawArr, RawValue, RealValue, ValueKind},
    },
};

#[derive(Debug, Clone)]
pub struct ComputeTask<T> {
    /// Stable numeric id for export correlation (optional, caller-assigned).
    pub id: T,
    pub precision: String,
    pub series: SeriesInstance,
    pub n_points: u64,
    /// Expanded noise instance (single noise, all values resolved).
    pub noise: Option<NoiseInstance>,
    pub algorithms: Vec<AccelInstance>,
    pub filters: Vec<FilterInstance>,
}

/// Descriptor of computed series
#[derive(Debug, Clone)]
pub struct SeriesDesc {
    pub precision: String,
    pub series: SeriesInstance,
    pub noise: Option<NoiseInstance>,
}

#[derive(Debug, Clone)]
pub struct SeriesEvent {
    pub n: u64,
    pub name: String,
    pub description: String,
}

#[derive(Debug, Clone)]
pub struct SeriesData {
    pub sn: Arr,
    pub an: Arr,
    pub sum: Option<Value>,
    pub deviations: Arr,
}

#[derive(Debug, Clone)]
pub struct AccelDesc {
    pub accel: AccelInstance,
    pub filter: Option<FilterInstance>,
}
#[derive(Debug, Clone)]
pub struct AccelData {
    pub events: Vec<SeriesEvent>,
}

#[derive(Debug, Clone)]
pub enum ComputeEvent<T> {
    SeriesDone(T, SeriesDesc, SeriesData, Option<(AccelDesc, AccelData)>),
    Complete(T),
    Error(String), // Attach T?
}

// struct RawAccelResult {
//     name: String,
//     val: RawArr,
//     an: RawArr,
//     dev: RawArr,
//     /// (filter_name, filtered_limit_arr, start_n, tail_len)
//     filters: Vec<(String, RawArr, u64, u64)>,
// }

/// Spawn an async compute task.  Results arrive via `tx`.
/// Cache writes are fire-and-forget — they never delay result delivery.
pub fn spawn_task<T: Clone>(
    task: ComputeTask<T>,
    cache: Cache,
    tx: mpsc::Sender<ComputeEvent<T>>,
) -> tokio::task::JoinHandle<()> {
    tokio::spawn(execute(task, cache, tx))
}

async fn execute<T: Clone>(task: ComputeTask<T>, cache: Cache, tx: mpsc::Sender<ComputeEvent<T>>) {
    let id = task.id.clone();
    match run_task(task, cache, &tx).await {
        Ok(()) => {
            let _ = tx.send(ComputeEvent::Complete(task.id)).await;
        }
        Err(e) => {
            let _ = tx.send(ComputeEvent::Error(e.to_string())).await;
        }
    }
}

async fn run_task<T>(
    task: ComputeTask<T>,
    cache: Cache,
    tx: &mpsc::Sender<ComputeEvent<T>>,
) -> Result<()> {
    let id = task.id;
    let n = task.n_points as usize;
    let args_json = sorted_json(&task.series.args)?;
    let noise_json = task
        .noise
        .as_ref()
        .map(|ni| serde_json::to_string(ni).unwrap_or_default());
    let display = display_name(&task);

    // ------------------------------------------------------------------
    // 1. Cache lookup
    // ------------------------------------------------------------------
    let cached_sid = cache
        .series_exists(
            task.series.name.clone(),
            task.precision.clone(),
            task.series.x_value.clone(),
            args_json.clone(),
            noise_json.clone(),
        )
        .await
        .unwrap_or(None);

    let cached_series = if let Some(sid) = cached_sid {
        cache
            .get_series_data(sid)
            .await
            .unwrap_or(None)
            .map(|(sum_json, data)| {
                let sum = serde_json::from_str::<Value>(&sum_json).ok();
                SeriesResult {
                    name: display.clone(),
                    sn: arr_from_blobs(&data.sn),
                    an: arr_from_blobs(&data.an),
                    sum,
                    deviations: arr_from_blobs(&data.dev),
                }
            })
            .map(|sr| (sid, sr))
    } else {
        None
    };

    // Which algorithms are already cached?
    let mut cached_accels: Vec<AccelResult> = Vec::new();
    let mut algos_to_run: Vec<&AccelParams> = Vec::new();

    if let Some((sid, _)) = &cached_series {
        for accel in &task.algorithms {
            let m_val = m_value(accel);
            let accel_args = sorted_json(&accel.params)?;
            let aid = cache
                .acceleration_exists(*sid, accel.name.clone(), m_val, accel_args)
                .await
                .unwrap_or(None);

            if let Some(accel_id) = aid {
                if let Ok(Some(data)) = cache.get_accel_data(accel_id).await {
                    let evs = cache.get_events(accel_id).await.unwrap_or_default();
                    let ests = cache
                        .get_filtered_estimates(accel_id)
                        .await
                        .unwrap_or_default();
                    cached_accels.push(build_accel_result_from_cache(
                        accel.name.clone(),
                        display.clone(),
                        data,
                        evs,
                        ests,
                    ));
                    continue;
                }
            }
            algos_to_run.push(accel);
        }
    } else {
        algos_to_run = task.algorithms.iter().collect();
    }

    let fully_cached = cached_series.is_some() && algos_to_run.is_empty();

    // Emit cached hits immediately
    if let Some((_, ref sr)) = cached_series {
        let _ = tx
            .send(ComputeEvent::SeriesDone {
                task_id: id,
                result: sr.clone(),
            })
            .await;
    }
    for ar in &cached_accels {
        let _ = tx
            .send(ComputeEvent::AccelDone {
                task_id: id,
                result: ar.clone(),
            })
            .await;
    }

    if fully_cached {
        return Ok(());
    }

    // ------------------------------------------------------------------
    // 2. FFI — do ALL C++ work in ONE spawn_blocking so UniquePtr<CSeries>
    //    never crosses thread boundaries.
    // ------------------------------------------------------------------
    let name = task.series.name.clone();
    let prec = task.precision.clone();
    let x = task.series.x_value.clone();
    let args = args_json.clone();
    let noise = task.noise.clone();
    let filters = task.filters.clone();
    let algos: Vec<(String, String, usize)> = algos_to_run
        .iter()
        .map(|a| {
            (
                a.name.clone(),
                sorted_json(&a.params).unwrap_or_default(),
                m_value(a).unwrap_or(5) as usize,
            )
        })
        .collect();

    let (raw_series, raw_accels) = tokio::task::spawn_blocking(move || -> Result<_> {
        // Build the series
        let series_ptr = if let Some(ref ni) = noise {
            let noise_json = serde_json::to_string(ni)?;
            let base = bridge::mk_series(&name, &prec, &args, 1, &x)?;
            bridge::apply_noise(&*base, &ni.noise_type.to_lowercase(), &noise_json, 0)?
        } else {
            bridge::mk_series(&name, &prec, &args, n, &x)?
        };

        let raw_series = RawSeriesResult {
            sn: bridge::get_sn(&*series_ptr),
            an: bridge::get_an(&*series_ptr),
            dev: bridge::get_deviation(&*series_ptr),
            lim: bridge::get_limit(&*series_ptr),
        };

        // Run all algorithms against the same series pointer
        let mut raw_accels: Vec<RawAccelResult> = Vec::new();
        for (algo_name, params_j, m) in algos {
            let accel_ptr = bridge::run_algo(&*series_ptr, &algo_name, &params_j, m, n)?;

            let dev_pts = bridge::get_deviation(&*accel_ptr);
            // Simple divergence: 3 consecutive rising deviations (on r1 = real/inf component)
            let mut inc = 0usize;
            let mut div_start: Option<usize> = None;
            for i in 1..dev_pts.r1.len() {
                if dev_pts.r1[i].mantissa > dev_pts.r1[i - 1].mantissa {
                    inc += 1;
                    if inc >= 3 && div_start.is_none() {
                        div_start = Some(i - 3);
                        break;
                    }
                } else {
                    inc = 0;
                }
            }

            let mut filter_results = Vec::new();
            if let Some(start) = div_start {
                let tail = dev_pts.r1.len().saturating_sub(start);
                if tail >= 5 {
                    for fdef in &filters {
                        let fargs = serde_json::to_string(&fdef.args).unwrap_or_default();
                        let filt =
                            bridge::filter(&*accel_ptr, &fdef.filter_type, &fargs, start as u64);
                        filter_results.push((
                            fdef.filter_type.clone(),
                            filt,
                            start as u64,
                            tail as u64,
                        ));
                    }
                }
            }

            raw_accels.push(RawAccelResult {
                name: algo_name,
                val: bridge::get_sn(&*accel_ptr),
                an: bridge::get_an(&*accel_ptr),
                dev: dev_pts,
                filters: filter_results,
            });
        }

        Ok((raw_series, raw_accels))
    })
    .await??;

    // ------------------------------------------------------------------
    // 3. Convert raw FFI results → typed results & emit events
    // ------------------------------------------------------------------
    let sum = value_from_raw(&raw_series.lim);
    let series_result = SeriesResult {
        name: display.clone(),
        sn: arr_from_raw(raw_series.sn),
        an: arr_from_raw(raw_series.an),
        sum: Some(sum.clone()),
        deviations: arr_from_raw(raw_series.dev),
    };

    if cached_series.is_none() {
        let _ = tx
            .send(ComputeEvent::SeriesDone {
                task_id: id,
                result: series_result.clone(),
            })
            .await;
    }

    // Fire-and-forget series cache write
    let (cache_bg, sum_j, name_bg, prec_bg, x_bg, args_bg, noise_bg) = (
        cache.clone(),
        serde_json::to_string(&sum).ok(),
        task.series.name.clone(),
        task.precision.clone(),
        task.series.x_value.clone(),
        args_json.clone(),
        noise_json.clone(),
    );
    let series_data = SeriesData {
        sn: arr_to_blobs(&series_result.sn),
        an: arr_to_blobs(&series_result.an),
        dev: arr_to_blobs(&series_result.deviations),
    };
    // We need the series_id for accel writes below — await the upsert (it's one INSERT OR IGNORE, fast)
    let series_db_id = cache_bg
        .upsert_series(name_bg, prec_bg, x_bg, args_bg, noise_bg, sum_j)
        .await
        .unwrap_or(-1);
    if series_db_id != -1 {
        let cache_s = cache.clone();
        let sd = series_data;
        let sid = series_db_id;
        tokio::spawn(async move {
            let _ = cache_s.insert_series_data(sid, sd).await;
        });
    }

    // Process each accel result
    for raw in raw_accels {
        let mut events: Vec<SeriesEvent> = Vec::new();
        let mut estimates: Vec<FilteredEstimate> = Vec::new();

        for (filter_name, raw_limit, start_n, length) in raw.filters {
            let limit_pts = arr_to_values(&arr_from_raw(raw_limit));
            if events.is_empty() {
                events.push(SeriesEvent {
                    n: start_n,
                    name: "divergent_accel".into(),
                    description: "Divergence detected, filters applied.".into(),
                });
            }
            estimates.push(FilteredEstimate {
                event_name: "divergent_accel".into(),
                filter: filter_name,
                limit: limit_pts,
                start_n,
                length,
            });
        }

        let ar = AccelResult {
            accel_name: raw.name.clone(),
            series_name: display.clone(),
            values: arr_from_raw(raw.val),
            an: arr_from_raw(raw.an),
            deviations: arr_from_raw(raw.dev),
            events: events.clone(),
            filtered_estimates: estimates.clone(),
        };

        let _ = tx
            .send(ComputeEvent::AccelDone {
                task_id: id,
                result: ar.clone(),
            })
            .await;

        // Fire-and-forget — accel cache write
        if series_db_id != -1 {
            let cache_a = cache.clone();
            let accel_name_a = raw.name.clone();
            let m_val_a = algos_to_run
                .iter()
                .find(|a| a.name == raw.name)
                .and_then(|a| m_value(a));
            let params_a = algos_to_run
                .iter()
                .find(|a| a.name == raw.name)
                .and_then(|a| sorted_json(&a.params).ok())
                .unwrap_or_default();
            let val_data = AccelData {
                val: arr_to_blobs(&ar.values),
                dev: arr_to_blobs(&ar.deviations),
            };
            let evs_a = events;
            let ests_a = estimates;
            let sid = series_db_id;

            tokio::spawn(async move {
                let accel_id = cache_a
                    .upsert_acceleration(sid, accel_name_a, m_val_a, params_a)
                    .await
                    .unwrap_or(-1);
                if accel_id == -1 {
                    return;
                }
                let _ = cache_a.insert_accel_data(accel_id, val_data).await;
                if !evs_a.is_empty() {
                    let _ = cache_a
                        .insert_events(
                            accel_id,
                            evs_a
                                .into_iter()
                                .map(|e| CachedEvent {
                                    n: e.n,
                                    name: e.name,
                                    description: e.description,
                                })
                                .collect(),
                        )
                        .await;
                }
                if !ests_a.is_empty() {
                    let _ = cache_a
                        .insert_filtered_estimates(
                            accel_id,
                            ests_a
                                .into_iter()
                                .map(|e| CachedEstimate {
                                    event_name: e.event_name,
                                    filter: e.filter,
                                    limit_points_json: serde_json::to_string(&e.limit)
                                        .unwrap_or_default(),
                                    start_n: e.start_n,
                                    length: e.length,
                                })
                                .collect(),
                        )
                        .await;
                }
            });
        }
    }

    Ok(())
}

// ---------------------------------------------------------------------------
// FFI ↔ Arr / Value conversions
// ---------------------------------------------------------------------------

fn arr_from_raw(r: RawArr) -> Arr {
    match r.tag {
        ArrKind::Real => Arr::Real(r.r1),
        ArrKind::Complex => Arr::Complex(ComplexOf {
            real: r.r1,
            imag: r.r2,
        }),
        ArrKind::Interval => Arr::Interval(IntervalOf {
            inf: r.r1,
            sup: r.r2,
        }),
        ArrKind::CInterval => Arr::CInterval(ComplexOf {
            real: IntervalOf {
                inf: r.r1,
                sup: r.r2,
            },
            imag: IntervalOf {
                inf: r.r3,
                sup: r.r4,
            },
        }),
        _ => Arr::Real(Vec::new()),
    }
}

fn value_from_raw(r: &RawValue) -> Value {
    match r.tag {
        ValueKind::Real => Value::Real(r.r1),
        ValueKind::Complex => Value::Complex(ComplexOf {
            real: r.r1,
            imag: r.r2,
        }),
        ValueKind::Interval => Value::Interval(IntervalOf {
            inf: r.r1,
            sup: r.r2,
        }),
        ValueKind::CInterval => Value::CInterval(ComplexOf {
            real: IntervalOf {
                inf: r.r1,
                sup: r.r2,
            },
            imag: IntervalOf {
                inf: r.r3,
                sup: r.r4,
            },
        }),
        _ => Value::Real(RealValue {
            mantissa: 0.0,
            exponent: 0,
        }),
    }
}

fn arr_to_values(arr: &Arr) -> Vec<Value> {
    match arr {
        Arr::Real(v) => v.iter().map(|&x| Value::Real(x)).collect(),
        Arr::Complex(c) => c
            .real
            .iter()
            .zip(&c.imag)
            .map(|(&r, &i)| Value::Complex(ComplexOf { real: r, imag: i }))
            .collect(),
        Arr::Interval(iv) => iv
            .inf
            .iter()
            .zip(&iv.sup)
            .map(|(&lo, &hi)| Value::Interval(IntervalOf { inf: lo, sup: hi }))
            .collect(),
        Arr::CInterval(ci) => ci
            .real
            .inf
            .iter()
            .zip(&ci.real.sup)
            .zip(&ci.imag.inf)
            .zip(&ci.imag.sup)
            .map(|(((ri, rs), ii), is)| {
                Value::CInterval(ComplexOf {
                    real: IntervalOf { inf: *ri, sup: *rs },
                    imag: IntervalOf { inf: *ii, sup: *is },
                })
            })
            .collect(),
    }
}

// ---------------------------------------------------------------------------
// Blob ↔ Arr (cache storage)
// ---------------------------------------------------------------------------

fn arr_to_blobs(arr: &Arr) -> RawArrBlobs {
    let empty = || Vec::<u8>::new();
    match arr {
        Arr::Real(v) => RawArrBlobs {
            kind: 0,
            len: v.len() as i64,
            m: [RealValue::to_bytes(v).to_vec(), empty(), empty(), empty()],
            e: [empty(), empty(), empty(), empty()],
        },
        Arr::Complex(c) => RawArrBlobs {
            kind: 1,
            len: c.real.len() as i64,
            m: [
                RealValue::to_bytes(&c.real).to_vec(),
                RealValue::to_bytes(&c.imag).to_vec(),
                empty(),
                empty(),
            ],
            e: [empty(), empty(), empty(), empty()],
        },
        Arr::Interval(iv) => RawArrBlobs {
            kind: 2,
            len: iv.inf.len() as i64,
            m: [
                RealValue::to_bytes(&iv.inf).to_vec(),
                RealValue::to_bytes(&iv.sup).to_vec(),
                empty(),
                empty(),
            ],
            e: [empty(), empty(), empty(), empty()],
        },
        Arr::CInterval(ci) => RawArrBlobs {
            kind: 3,
            len: ci.real.inf.len() as i64,
            m: [
                RealValue::to_bytes(&ci.real.inf).to_vec(),
                RealValue::to_bytes(&ci.real.sup).to_vec(),
                RealValue::to_bytes(&ci.imag.inf).to_vec(),
                RealValue::to_bytes(&ci.imag.sup).to_vec(),
            ],
            e: [empty(), empty(), empty(), empty()],
        },
    }
}

fn arr_from_blobs(b: &RawArrBlobs) -> Arr {
    let rv = |bytes: &[u8]| RealValue::from_bytes(bytes);
    match b.kind {
        0 => Arr::Real(rv(&b.m[0])),
        1 => Arr::Complex(ComplexOf {
            real: rv(&b.m[0]),
            imag: rv(&b.m[1]),
        }),
        2 => Arr::Interval(IntervalOf {
            inf: rv(&b.m[0]),
            sup: rv(&b.m[1]),
        }),
        3 => Arr::CInterval(ComplexOf {
            real: IntervalOf {
                inf: rv(&b.m[0]),
                sup: rv(&b.m[1]),
            },
            imag: IntervalOf {
                inf: rv(&b.m[2]),
                sup: rv(&b.m[3]),
            },
        }),
        _ => Arr::Real(Vec::new()),
    }
}

fn sorted_json(params: &std::collections::HashMap<String, serde_json::Value>) -> Result<String> {
    let sorted: BTreeMap<_, _> = params.iter().collect();
    Ok(serde_json::to_string(&sorted)?)
}

// fn display_name(task: &ComputeTask) -> String {
//     format!(
//         "{} ({}{}))",
//         task.series.name,
//         task.precision,
//         task.noise
//             .as_ref()
//             .map(|n| format!(", {}", n.noise_type))
//             .unwrap_or_default()
//     )
// }

// fn build_accel_result_from_cache(
//     accel_name: String,
//     series_name: String,
//     data: AccelData,
//     events: Vec<CachedEvent>,
//     estimates: Vec<CachedEstimate>,
// ) -> AccelResult {
//     AccelResult {
//         accel_name,
//         series_name,
//         values: arr_from_blobs(&data.val),
//         an: Arr::Real(Vec::new()), // not stored separately yet
//         deviations: arr_from_blobs(&data.dev),
//         events: events
//             .into_iter()
//             .map(|e| SeriesEvent {
//                 n: e.n,
//                 name: e.name,
//                 description: e.description,
//             })
//             .collect(),
//         filtered_estimates: estimates
//             .into_iter()
//             .map(|e| FilteredEstimate {
//                 event_name: e.event_name,
//                 filter: e.filter,
//                 limit: serde_json::from_str(&e.limit_points_json).unwrap_or_default(),
//                 start_n: e.start_n,
//                 length: e.length,
//             })
//             .collect(),
//     }
// }
