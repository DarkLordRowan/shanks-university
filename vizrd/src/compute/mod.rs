//! Async compute pipeline — single entry point for all series computation.
//!
//! # Flow
//! ```text
//! spawn_task(task, cache, tx)
//!    └─ tokio::spawn execute()
//!          ├─ PASS 1: emit whatever is in cache right now (stale ok, dups ok)
//!          └─ PASS 2: recompute anything missing or with insufficient n_points
//!                ├─ ONE spawn_blocking { all FFI } (UniquePtr stays local)
//!                ├─ emit fresh results via tx
//!                └─ fire-and-forget cache writes
//! ```

use anyhow::Result;
use log::debug;
use std::collections::BTreeMap;
use tokio::sync::mpsc;

use crate::{
    cache::{Cache, CachedAccelData, CachedEvent, CachedResultData, CachedSeriesData, RawArrBlobs},
    experiment::{AccelInstance, FilterInstance, NoiseInstance, SeriesInstance},
    ffi::{
        Arr, ComplexOf, IntervalOf, Value,
        bridge::ffi::{self as bridge, ArrKind, RawArr, RawValue, RealValue, ValueKind},
    },
};

/// A computation request.
#[derive(Debug, Clone)]
pub struct ComputeTask<T> {
    pub id: T,
    pub series: SeriesDesc,
    pub n_points: u64,
    /// Expanded noise (all values resolved). `None` = no noise.
    pub algorithms: Vec<AccelInstance>,
    /// Filters applied independently to every accel that triggers a stop event.
    pub filters: Vec<FilterInstance>,
}

/// Identifies a computed series (what was computed).
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct SeriesDesc {
    pub precision: String,
    pub series: SeriesInstance,
    pub noise: Option<NoiseInstance>,
}

#[derive(Debug, Clone)]
pub struct ResultData {
    pub values: Arr,
    pub an: Arr,
    pub deviations: Arr,
}

/// The numerical result of a series computation.
#[derive(Debug, Clone)]
pub struct SeriesData {
    pub result: ResultData,
    pub sum: Option<Value>,
}

/// Identifies the accel+filter combination that was computed.
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct AccelDesc {
    pub accel: AccelInstance,
    /// `None` = no filter was applied (no stop event, or no filters requested).
    pub filter: Option<FilterInstance>,
}

/// An event triggered during accel computation.
#[derive(Debug, Clone)]
pub struct SeriesEvent {
    pub n: u64,
    pub name: String,
    pub description: String,
}

/// The numerical result of one (accel, filter) computation.
#[derive(Debug, Clone)]
pub struct AccelData {
    pub start_offset: u64,
    pub result: ResultData,
    pub events: Vec<SeriesEvent>,
}

/// Events emitted via the channel.
#[derive(Debug, Clone)]
pub enum ComputeEvent<T> {
    SeriesDone {
        id: T,
        data: SeriesData,
    },
    AccelDone {
        id: T,
        desc: AccelDesc,
        data: AccelData,
    },
    Complete(T),
    Error {
        id: T,
        error: String,
    },
}

// // ---------------------------------------------------------------------------
// // Compute Entry Points
// // ---------------------------------------------------------------------------

/// Spawn an async compute task.  Results arrive via `tx` (one per accel+filter).
pub fn spawn_task<T>(
    task: ComputeTask<T>,
    cache: Cache,
    tx: mpsc::Sender<ComputeEvent<T>>,
) -> tokio::task::JoinHandle<()>
where
    T: Clone + Send + 'static,
{
    tokio::spawn(execute(task, cache, tx))
}

async fn execute<T>(task: ComputeTask<T>, cache: Cache, tx: mpsc::Sender<ComputeEvent<T>>)
where
    T: Clone + Send + 'static,
{
    let id = task.id.clone();
    if let Err(e) = run_task(task, cache, &tx).await {
        let _ = tx
            .send(ComputeEvent::Error {
                id: id.clone(),
                error: e.to_string(),
            })
            .await;
    }
    let _ = tx.send(ComputeEvent::Complete(id)).await;
}

async fn run_task<T>(
    task: ComputeTask<T>,
    cache: Cache,
    tx: &mpsc::Sender<ComputeEvent<T>>,
) -> Result<()>
where
    T: Clone + Send + 'static,
{
    let id = task.id.clone();
    let n_needed = task.n_points;
    let args_json = sorted_args_json(&task.series.series.args)?;
    let noise_json = task
        .series
        .noise
        .as_ref()
        .map(|ni| serde_json::to_string(ni).unwrap_or_default());

    // 1. Initial Cache Scan & Emission (PASS 1)
    let cached_series = cache
        .series_exists(
            task.series.series.name.clone(),
            task.series.precision.clone(),
            task.series.series.x.to_string(),
            args_json.clone(),
            noise_json.clone(),
        )
        .await
        .unwrap_or(None);

    let (cached_id, cached_n, cached_sum_json) = match cached_series {
        Some((id, n, sum)) => (Some(id), Some(n), sum),
        None => (None, None, None),
    };

    if let Some(sid) = cached_id {
        if let Ok(Some(sd)) = cache.get_series_data(sid).await {
            let mut sdata = series_data_from_cache(&sd);
            if let Some(sj) = cached_sum_json {
                if let Ok(val) = serde_json::from_str(&sj) {
                    sdata.sum = Some(val);
                }
            }
            let _ = tx
                .send(ComputeEvent::SeriesDone {
                    id: id.clone(),
                    data: sdata.clone(),
                })
                .await;
        }
    }

    // Build TODO list for recomputation (Workset)
    let mut todo: Vec<(usize, AccelInstance, bool, Vec<FilterInstance>)> = vec![];
    let series_short = cached_n.map(|n| n < n_needed).unwrap_or(true);

    for (a_idx, accel) in task.algorithms.iter().enumerate() {
        let aargs = sorted_args_json(&accel.args)?;
        let cached_accel = if let Some(sid) = cached_id {
            cache
                .accel_exists(
                    sid,
                    accel.name.clone(),
                    Some(accel.m),
                    aargs.clone(),
                    None,
                    None,
                )
                .await
                .unwrap_or(None)
        } else {
            None
        };

        let accel_short = cached_accel.map(|(_, n)| n < n_needed).unwrap_or(true);

        // Emit cached unfiltered if available
        if let Some((aid, _)) = cached_accel {
            if let Ok(Some(ad)) = cache.get_accel_data(aid).await {
                let events = cache.get_events(aid).await.unwrap_or_default();
                let adata = accel_data_from_cache(&ad, events);
                let _ = tx
                    .send(ComputeEvent::AccelDone {
                        id: id.clone(),
                        desc: AccelDesc {
                            accel: accel.clone(),
                            filter: None,
                        },
                        data: adata,
                    })
                    .await;
            }
        }

        let mut filters_to_do = vec![];
        for filter in &task.filters {
            let fargs = sorted_args_json(&filter.args)?;
            let cached_filt = if let Some(sid) = cached_id {
                cache
                    .accel_exists(
                        sid,
                        accel.name.clone(),
                        Some(accel.m),
                        aargs.clone(),
                        Some(filter.filter_type.clone()),
                        Some(fargs),
                    )
                    .await
                    .unwrap_or(None)
            } else {
                None
            };

            if cached_filt.map(|(_, n)| n < n_needed).unwrap_or(true) {
                filters_to_do.push(filter.clone());
            } else if let Some((aid, _)) = cached_filt {
                // Emit cached filtered if available
                if let Ok(Some(ad)) = cache.get_accel_data(aid).await {
                    let events = cache.get_events(aid).await.unwrap_or_default();
                    let adata = accel_data_from_cache(&ad, events);
                    let _ = tx
                        .send(ComputeEvent::AccelDone {
                            id: id.clone(),
                            desc: AccelDesc {
                                accel: accel.clone(),
                                filter: Some(filter.clone()),
                            },
                            data: adata,
                        })
                        .await;
                }
            }
        }

        if accel_short || !filters_to_do.is_empty() {
            todo.push((a_idx, accel.clone(), accel_short, filters_to_do));
        }
    }

    if !series_short && todo.is_empty() {
        return Ok(());
    }

    // 2. PASS 2: Lazy Recomputation (spawn_blocking)
    let (internal_tx, mut internal_rx) = mpsc::channel(32);
    let s_id = id.clone();
    let s_name_raw = task.series.series.name.clone();
    let s_prec_raw = task.series.precision.clone();
    let s_args_raw = args_json.clone();
    let s_x_raw = task.series.series.x.to_string();
    let s_noise_json_raw = noise_json.clone();
    let s_n_needed = task.n_points;

    // Ensure series exists in DB for ID mapping
    let series_db_id = cache
        .upsert_series(
            s_name_raw.clone(),
            s_prec_raw.clone(),
            s_x_raw.clone(),
            s_args_raw.clone(),
            s_noise_json_raw.clone(),
            s_n_needed,
            None,
        )
        .await
        .unwrap_or(-1);

    let s_name = s_name_raw.clone();
    let s_prec = s_prec_raw.clone();
    let s_args = s_args_raw.clone();
    let s_x = s_x_raw.clone();
    let s_noise = task.series.noise.clone();

    let blocking_handle = tokio::task::spawn_blocking(move || -> Result<()> {
        let mut lazy_series: Option<cxx::UniquePtr<bridge::CSeries>> = None;

        // Ensure series is computed if needed for anything in this block
        if series_short || !todo.is_empty() {
            debug!("Computing {s_name}");
            let mut ptr = bridge::mk_series(&s_name, &s_prec, &s_args, s_n_needed as usize, &s_x)?;
            if let Some(ref ni) = s_noise {
                let njson = serde_json::to_string(ni)?;
                ptr = bridge::apply_noise(&*ptr, &ni.noise_type.to_lowercase(), &njson, 0)?;
            }
            let sum = value_from_raw(&bridge::get_limit(&*ptr));
            let sdata = SeriesData {
                result: ResultData {
                    values: arr_from_raw(bridge::get_sn(&*ptr)),
                    an: arr_from_raw(bridge::get_an(&*ptr)),
                    deviations: arr_from_raw(bridge::get_deviation(&*ptr)),
                },
                sum: Some(sum.clone()),
            };
            if series_short {
                let _ = internal_tx.blocking_send(ComputeEvent::SeriesDone {
                    id: s_id.clone(),
                    data: sdata.clone(),
                });
            }
            lazy_series = Some(ptr);
            let _ = sdata; // assigned but never used
        }

        let mut lazy_accels: BTreeMap<
            usize,
            (
                cxx::UniquePtr<bridge::CSeries>,
                Option<u64>,
                Vec<SeriesEvent>,
            ),
        > = BTreeMap::new();

        for (a_idx, a_inst, need_unfiltered, filters) in todo {
            debug!("Computing {}", a_inst.name);
            // Lazy compute/fetch the acceleration pointer
            if !lazy_accels.contains_key(&a_idx) {
                let s_ptr = lazy_series.as_ref().unwrap();
                let aargs_json = sorted_args_json(&a_inst.args)?;
                let ptr = bridge::run_algo(
                    &**s_ptr,
                    &a_inst.name,
                    &aargs_json,
                    a_inst.m as usize,
                    s_n_needed as usize,
                )?;
                // Collect events emitted by the C++ algorithm (per-step errors)
                let cpp_events: Vec<SeriesEvent> = bridge::get_events(&*ptr)
                    .iter()
                    .filter_map(|s| {
                        let mut parts = s.splitn(3, '\t');
                        let n: u64 = parts.next()?.parse().ok()?;
                        let name = parts.next()?.to_string();
                        let description = parts.next().unwrap_or("").to_string();
                        Some(SeriesEvent {
                            n,
                            name,
                            description,
                        })
                    })
                    .collect();
                let dev = bridge::get_deviation(&*ptr);
                let stop_n = detect_divergence(&dev.r1);
                lazy_accels.insert(a_idx, (ptr, stop_n, cpp_events));
            }
            let (ref a_ptr, stop_n, ref cpp_events) = lazy_accels[&a_idx];

            if need_unfiltered {
                let mut events: Vec<SeriesEvent> = cpp_events.clone();
                if let Some(n) = stop_n {
                    events.push(SeriesEvent {
                        n,
                        name: "stop".to_string(),
                        description: "Divergence detected; stop_action_limit reached.".to_string(),
                    });
                }
                let adata = AccelData {
                    start_offset: 0,
                    result: ResultData {
                        values: arr_from_raw(bridge::get_sn(&**a_ptr)),
                        an: arr_from_raw(bridge::get_an(&**a_ptr)),
                        deviations: arr_from_raw(bridge::get_deviation(&**a_ptr)),
                    },
                    events,
                };
                let _ = internal_tx.blocking_send(ComputeEvent::AccelDone {
                    id: s_id.clone(),
                    desc: AccelDesc {
                        accel: a_inst.clone(),
                        filter: None,
                    },
                    data: adata,
                });
            }

            for f_inst in filters {
                debug!("Filtering with {}", f_inst.filter_type);
                let fargs = sorted_args_json(&f_inst.args)?;
                let farr = match bridge::filter(
                    &**a_ptr,
                    &f_inst.filter_type,
                    &fargs,
                    stop_n.unwrap_or(0),
                ) {
                    Ok(arr) => arr,
                    Err(e) => {
                        let _ = internal_tx.blocking_send(ComputeEvent::Error {
                            id: s_id.clone(),
                            error: e.to_string(),
                        });
                        continue;
                    }
                };

                let stop_event = stop_n.map(|n| SeriesEvent {
                    n,
                    name: "stop".to_string(),
                    description: "Divergence detected; stop_action_limit reached.".to_string(),
                });
                let adata = AccelData {
                    start_offset: stop_n.unwrap_or(0),
                    result: ResultData {
                        values: arr_from_raw(farr),
                        an: Arr::Real(Vec::new()),
                        deviations: Arr::Real(Vec::new()),
                    },
                    events: stop_event.into_iter().collect(),
                };
                let _ = internal_tx.blocking_send(ComputeEvent::AccelDone {
                    id: s_id.clone(),
                    desc: AccelDesc {
                        accel: a_inst.clone(),
                        filter: Some(f_inst.clone()),
                    },
                    data: adata,
                });
            }
        }
        Ok(())
    });

    // Forward internal events and handle cache writes
    while let Some(event) = internal_rx.recv().await {
        let _ = tx.send(event.clone()).await;

        if series_db_id != -1 {
            match event {
                ComputeEvent::SeriesDone { data, .. } => {
                    let c = cache.clone();
                    let sid = series_db_id;
                    let blobs = CachedSeriesData {
                        result: CachedResultData {
                            values: arr_to_blobs(&data.result.values),
                            an: arr_to_blobs(&data.result.an),
                            deviations: arr_to_blobs(&data.result.deviations),
                        },
                    };
                    let name = s_name_raw.clone();
                    let prec = s_prec_raw.clone();
                    let x = s_x_raw.clone();
                    let args = s_args_raw.clone();
                    let noise = s_noise_json_raw.clone();
                    let npts = s_n_needed;
                    let sum_opt = data.sum.clone();

                    tokio::spawn(async move {
                        let _ = c.insert_series_data(sid, blobs).await;
                        if let Some(sum) = sum_opt {
                            if let Ok(sj) = serde_json::to_string(&sum) {
                                let _ = c
                                    .upsert_series(name, prec, x, args, noise, npts, Some(sj))
                                    .await;
                            }
                        }
                    });
                }
                ComputeEvent::AccelDone { desc, data, .. } => {
                    let c = cache.clone();
                    let sid = series_db_id;
                    let npts = n_needed;
                    let aname = desc.accel.name.clone();
                    let m = desc.accel.m;
                    let aargs = sorted_args_json(&desc.accel.args).unwrap_or_default();
                    let ftype = desc.filter.as_ref().map(|f| f.filter_type.clone());
                    let fargs = desc
                        .filter
                        .as_ref()
                        .map(|f| sorted_args_json(&f.args).unwrap_or_default());
                    let ablobs = CachedAccelData {
                        start_offset: data.start_offset,
                        result: CachedResultData {
                            values: arr_to_blobs(&data.result.values),
                            an: arr_to_blobs(&data.result.an),
                            deviations: arr_to_blobs(&data.result.deviations),
                        },
                    };
                    let evs: Vec<CachedEvent> = data
                        .events
                        .iter()
                        .map(|e| CachedEvent {
                            n: e.n,
                            name: e.name.clone(),
                            description: e.description.clone(),
                        })
                        .collect();
                    tokio::spawn(async move {
                        let aid = c
                            .upsert_accel(sid, aname, Some(m), aargs, ftype, fargs, npts)
                            .await
                            .unwrap_or(-1);
                        if aid != -1 {
                            let _ = c.insert_accel_data(aid, ablobs).await;
                            if !evs.is_empty() {
                                let _ = c.insert_events(aid, evs).await;
                            }
                        }
                    });
                }
                _ => {}
            }
        }
    }

    blocking_handle.await??;
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
            e: Default::default(),
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
            e: Default::default(),
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
            e: Default::default(),
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
            e: Default::default(),
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

// ---------------------------------------------------------------------------
// Cache → typed helpers
// ---------------------------------------------------------------------------

fn series_data_from_cache(sd: &CachedSeriesData) -> SeriesData {
    SeriesData {
        result: ResultData {
            values: arr_from_blobs(&sd.result.values),
            an: arr_from_blobs(&sd.result.an),
            deviations: arr_from_blobs(&sd.result.deviations),
        },
        sum: None, // sum stored separately in the series row (not loaded here)
    }
}

fn accel_data_from_cache(ad: &CachedAccelData, events: Vec<CachedEvent>) -> AccelData {
    AccelData {
        start_offset: ad.start_offset,
        result: ResultData {
            values: arr_from_blobs(&ad.result.values),
            an: arr_from_blobs(&ad.result.an),
            deviations: arr_from_blobs(&ad.result.deviations),
        },
        events: events
            .into_iter()
            .map(|e| SeriesEvent {
                n: e.n,
                name: e.name,
                description: e.description,
            })
            .collect(),
    }
}

// ---------------------------------------------------------------------------
// Misc helpers
// ---------------------------------------------------------------------------

fn sorted_args_json(params: &BTreeMap<String, serde_json::Value>) -> Result<String> {
    Ok(serde_json::to_string(params)?)
}

/// Detect the first index where there are 3+ consecutive rising deviations.
/// Returns the start of the rising run (already adjusted by -3).
fn detect_divergence(devs: &[RealValue]) -> Option<u64> {
    let mut run = 0usize;
    for i in 1..devs.len() {
        if devs[i].mantissa > devs[i - 1].mantissa {
            run += 1;
            if run >= 3 {
                let start = i.saturating_sub(3);
                return Some(start as u64);
            }
        } else {
            run = 0;
        }
    }
    None
}
