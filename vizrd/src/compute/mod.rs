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
use serde::{Deserialize, Serialize};
use std::{
    cmp,
    collections::BTreeMap,
    fmt::Display,
    sync::{
        Arc,
        atomic::{self, Atomic},
    },
};
use tokio::{sync::mpsc, task::JoinHandle};

use crate::{
    cache::{Cache, CachedAccelData, CachedResultData, CachedSeriesData, RawArrBlobs},
    experiment::{AccelInstance, FilterInstance, NoiseInstance, SeriesInstance},
    ffi::{
        Arr, ComplexOf, IntervalOf, Value,
        bridge::ffi::{self as bridge, ArrKind, RawArr, RawValue, RealValue, ValueKind},
    },
};

pub struct Cancellable {
    handle: JoinHandle<()>,
    cancelled: Arc<Atomic<bool>>,
}

#[derive(Clone)]
pub struct IsCancelled(Arc<Atomic<bool>>);

impl Cancellable {
    pub fn new(f: impl FnOnce(IsCancelled) -> JoinHandle<()>) -> Self {
        let cancelled = Arc::new(Atomic::<bool>::new(false));
        Self {
            handle: f(IsCancelled(cancelled.clone())),
            cancelled,
        }
    }
    pub fn cancel(&self) {
        self.handle.abort();
        self.cancelled.store(true, atomic::Ordering::Relaxed);
    }
}

impl IsCancelled {
    pub fn cancelled(&self) -> bool {
        self.0.load(atomic::Ordering::Relaxed)
    }
}

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
    /// Global event configuration (stop limits).
    pub events: BTreeMap<SeriesEventKind, crate::experiment::EventConfig>,
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
    pub sn: Arr,
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
    /// The event configuration used for this computation.
    pub events: BTreeMap<SeriesEventKind, crate::experiment::EventConfig>,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Serialize, Deserialize, Hash)]
#[serde(rename_all = "snake_case")]
pub enum SeriesEventKind {
    SlowAccel,
    Monotone,
    DivergentAccel,
    SignChanged,
    SecondDiff,
    Trigger,
    Error,
}

impl Display for SeriesEventKind {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            SeriesEventKind::SlowAccel => write!(f, "slow_accel"),
            SeriesEventKind::Monotone => write!(f, "monotone"),
            SeriesEventKind::DivergentAccel => write!(f, "divergent_accel"),
            SeriesEventKind::SignChanged => write!(f, "sign_changed"),
            SeriesEventKind::SecondDiff => write!(f, "second_diff"),
            SeriesEventKind::Trigger => write!(f, "trigger"),
            SeriesEventKind::Error => write!(f, "error"),
        }
    }
}

impl SeriesEventKind {
    pub fn symbol(&self) -> &'static str {
        match self {
            Self::SlowAccel => "🐌",
            Self::Monotone => "⏸",
            Self::DivergentAccel => "⏫",
            Self::SignChanged => "±",
            Self::SecondDiff => "Δ²",
            Self::Trigger => "🎯",
            Self::Error => "❌",
        }
    }
}

/// An event triggered during accel computation.
#[derive(Debug, Clone)]
pub struct SeriesEvent {
    pub n: u64,
    pub kind: SeriesEventKind,
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
) -> Cancellable
where
    T: Clone + Send + 'static,
{
    Cancellable::new(move |is_cancelled| tokio::spawn(execute(is_cancelled, task, cache, tx)))
}

async fn execute<T>(
    is_cancelled: IsCancelled,
    task: ComputeTask<T>,
    cache: Cache,
    tx: mpsc::Sender<ComputeEvent<T>>,
) where
    T: Clone + Send + 'static,
{
    let id = task.id.clone();
    if let Err(e) = run_task(is_cancelled, task, cache, &tx).await {
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
    is_cancelled: IsCancelled,
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

    let mut sdata_opt: Option<Arc<SeriesData>> = None;
    if let Some(sid) = cached_id {
        if let Ok(Some(sd)) = cache.get_series_data(sid).await {
            let mut sdata = series_data_from_cache(&sd);
            if let Some(sj) = cached_sum_json {
                if let Ok(val) = serde_json::from_str(&sj) {
                    sdata.sum = Some(val);
                }
            }
            let s_done = sdata.clone();
            let _ = tx
                .send(ComputeEvent::SeriesDone {
                    id: id.clone(),
                    data: s_done,
                })
                .await;
            sdata_opt = Some(Arc::new(sdata));
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
                    serde_json::to_string(&task.events).unwrap_or_default(),
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
                let mut adata = accel_data_from_cache(&ad, events);

                // Derived events for cache results
                if let Some(ref sdata) = sdata_opt {
                    let (volatile, _trigger_n) = process_events(
                        &adata.result.sn,
                        &adata.result.an,
                        &adata.result.deviations,
                        &sdata.result.deviations,
                        &task.events,
                    );
                    adata.events.extend(volatile);
                    adata.events.sort_by_key(|e| e.n);
                }

                let _ = tx
                    .send(ComputeEvent::AccelDone {
                        id: id.clone(),
                        desc: AccelDesc {
                            accel: accel.clone(),
                            filter: None,
                            events: task.events.clone(),
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
                        serde_json::to_string(&task.events).unwrap_or_default(),
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
                    let mut adata = accel_data_from_cache(&ad, events);

                    // Derived events for cache filtered results
                    if let Some(ref sdata) = sdata_opt {
                        let (volatile, _trigger_n) = process_events(
                            &adata.result.sn,
                            &adata.result.an,
                            &adata.result.deviations,
                            &sdata.result.deviations,
                            &task.events,
                        );
                        adata.events.extend(volatile);
                        adata.events.sort_by_key(|e| e.n);
                    }

                    let _ = tx
                        .send(ComputeEvent::AccelDone {
                            id: id.clone(),
                            desc: AccelDesc {
                                accel: accel.clone(),
                                filter: Some(filter.clone()),
                                events: task.events.clone(),
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

    if is_cancelled.cancelled() || (!series_short && todo.is_empty()) {
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

    let is_cancelled2 = is_cancelled.clone();
    let blocking_handle = tokio::task::spawn_blocking(move || -> Result<()> {
        let mut lazy_series: Option<cxx::UniquePtr<bridge::CSeries>> = None;

        // Ensure series is computed if needed for anything in this block
        if series_short || !todo.is_empty() {
            debug!("Computing {s_name}");
            let mut ptr =
                match bridge::mk_series(&s_name, &s_prec, &s_args, s_n_needed as usize, &s_x) {
                    Ok(p) => p,
                    Err(e) => return Err(anyhow::anyhow!("mk_series failed: {}", e)),
                };
            if let Some(ref ni) = s_noise {
                let njson = match serde_json::to_string(ni) {
                    Ok(j) => j,
                    Err(e) => return Err(anyhow::anyhow!("serde_json failed: {}", e)),
                };
                ptr = match bridge::apply_noise(&*ptr, &ni.noise_type.to_lowercase(), &njson, 0) {
                    Ok(p) => p,
                    Err(e) => return Err(anyhow::anyhow!("apply_noise failed: {}", e)),
                };
            }
            let sum = value_from_raw(&bridge::get_limit(&*ptr));
            let sdata = SeriesData {
                result: ResultData {
                    sn: arr_from_raw(bridge::get_sn(&*ptr)),
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
            if is_cancelled.cancelled() {
                return Ok(());
            };
            debug!("Computing {}", a_inst.name);
            // Lazy compute/fetch the acceleration pointer
            if !lazy_accels.contains_key(&a_idx) {
                let s_ptr = lazy_series.as_ref().unwrap();
                let aargs_json = match sorted_args_json(&a_inst.args) {
                    Ok(j) => j,
                    Err(e) => {
                        let _ = internal_tx.blocking_send(ComputeEvent::Error {
                            id: s_id.clone(),
                            error: format!("JSON serialization failed for args: {}", e),
                        });
                        continue;
                    }
                };
                let ptr = match bridge::run_algo(
                    &**s_ptr,
                    &a_inst.name,
                    &aargs_json,
                    a_inst.m as usize,
                    s_n_needed as usize,
                ) {
                    Ok(p) => p,
                    Err(e) => {
                        // Produce a synthetic error event for the dataview
                        let adata = AccelData {
                            start_offset: 0,
                            result: ResultData {
                                sn: Arr::Real(Vec::new()),
                                an: Arr::Real(Vec::new()),
                                deviations: Arr::Real(Vec::new()),
                            },
                            events: vec![SeriesEvent {
                                n: 0,
                                kind: SeriesEventKind::Error,
                                description: format!("run_algo failed: {}", e),
                            }],
                        };
                        let _ = internal_tx.blocking_send(ComputeEvent::AccelDone {
                            id: s_id.clone(),
                            desc: AccelDesc {
                                accel: a_inst.clone(),
                                filter: None,
                                events: task.events.clone(),
                            },
                            data: adata,
                        });
                        let _ = internal_tx.blocking_send(ComputeEvent::Error {
                            id: s_id.clone(),
                            error: format!("Algorithm {} failed: {}", a_inst.name, e),
                        });
                        continue;
                    }
                };
                let sn = arr_from_raw(bridge::get_sn(&*ptr));
                let an = arr_from_raw(bridge::get_an(&*ptr));
                let dev = arr_from_raw(bridge::get_deviation(&*ptr));
                let s_dev = arr_from_raw(bridge::get_deviation(&**s_ptr));

                let (mut processed_events, stop_n) =
                    process_events(&sn, &an, &dev, &s_dev, &task.events);

                // Pull C++ errors
                let cpp_errors = bridge::get_errors(&*ptr);
                for err in cpp_errors {
                    processed_events.push(SeriesEvent {
                        n: err.n,
                        kind: SeriesEventKind::Error,
                        description: err.message,
                    });
                }
                processed_events.sort_by_key(|e| e.n);

                lazy_accels.insert(a_idx, (ptr, stop_n, processed_events));
            }
            let (ref a_ptr, stop_n, ref events) = lazy_accels[&a_idx];

            if need_unfiltered {
                let adata = AccelData {
                    start_offset: 0,
                    result: ResultData {
                        sn: arr_from_raw(bridge::get_sn(&**a_ptr)),
                        an: arr_from_raw(bridge::get_an(&**a_ptr)),
                        deviations: arr_from_raw(bridge::get_deviation(&**a_ptr)),
                    },
                    events: events.clone(),
                };
                let _ = internal_tx.blocking_send(ComputeEvent::AccelDone {
                    id: s_id.clone(),
                    desc: AccelDesc {
                        accel: a_inst.clone(),
                        filter: None,
                        events: task.events.clone(),
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
                        let adata = AccelData {
                            start_offset: stop_n.unwrap_or(0),
                            result: ResultData {
                                sn: Arr::Real(Vec::new()),
                                an: Arr::Real(Vec::new()),
                                deviations: Arr::Real(Vec::new()),
                            },
                            events: vec![SeriesEvent {
                                n: 0,
                                kind: SeriesEventKind::Error,
                                description: format!("filter failed: {}", e),
                            }],
                        };
                        let _ = internal_tx.blocking_send(ComputeEvent::AccelDone {
                            id: s_id.clone(),
                            desc: AccelDesc {
                                accel: a_inst.clone(),
                                filter: Some(f_inst.clone()),
                                events: task.events.clone(),
                            },
                            data: adata,
                        });
                        let _ = internal_tx.blocking_send(ComputeEvent::Error {
                            id: s_id.clone(),
                            error: format!("Filter {} failed: {}", f_inst.filter_type, e),
                        });
                        continue;
                    }
                };

                let adata = AccelData {
                    start_offset: stop_n.unwrap_or(0),
                    result: ResultData {
                        sn: arr_from_raw(farr),
                        an: Arr::Real(Vec::new()),
                        deviations: Arr::Real(Vec::new()),
                    },
                    events: vec![], // events already emitted with unfiltered or not applicable
                };
                let _ = internal_tx.blocking_send(ComputeEvent::AccelDone {
                    id: s_id.clone(),
                    desc: AccelDesc {
                        accel: a_inst.clone(),
                        filter: Some(f_inst.clone()),
                        events: task.events.clone(),
                    },
                    data: adata,
                });
            }
        }
        Ok(())
    });

    // Forward internal events and handle cache writes
    while let Some(event) = internal_rx.recv().await {
        if is_cancelled2.cancelled() {
            return Ok(());
        };
        let _ = tx.send(event.clone()).await;

        if series_db_id != -1 {
            match event {
                ComputeEvent::SeriesDone { data, .. } => {
                    let c = cache.clone();
                    let sid = series_db_id;
                    let blobs = CachedSeriesData {
                        result: CachedResultData {
                            values: arr_to_blobs(&data.result.sn),
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
                            values: arr_to_blobs(&data.result.sn),
                            an: arr_to_blobs(&data.result.an),
                            deviations: arr_to_blobs(&data.result.deviations),
                        },
                    };
                    tokio::spawn(async move {
                        let aid = c
                            .upsert_accel(
                                sid,
                                aname,
                                Some(m),
                                aargs,
                                serde_json::to_string(&desc.events).unwrap_or_default(),
                                ftype,
                                fargs,
                                npts,
                            )
                            .await
                            .unwrap_or(-1);
                        if aid != -1 {
                            let _ = c.insert_accel_data(aid, ablobs).await;
                            if !data.events.is_empty() {
                                let _ = c.insert_events(aid, data.events).await;
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
            sn: arr_from_blobs(&sd.result.values),
            an: arr_from_blobs(&sd.result.an),
            deviations: arr_from_blobs(&sd.result.deviations),
        },
        sum: None, // sum stored separately in the series row (not loaded here)
    }
}

fn accel_data_from_cache(ad: &CachedAccelData, events: Vec<SeriesEvent>) -> AccelData {
    AccelData {
        start_offset: ad.start_offset,
        result: ResultData {
            sn: arr_from_blobs(&ad.result.values),
            an: arr_from_blobs(&ad.result.an),
            deviations: arr_from_blobs(&ad.result.deviations),
        },
        events: events,
    }
}

// ---------------------------------------------------------------------------
// Misc helpers
// ---------------------------------------------------------------------------

fn rv_abs_cmp(a: &bridge::RealValue, b: &bridge::RealValue) -> std::cmp::Ordering {
    if let Some(d) = a.exponent.partial_cmp(&b.exponent)
        && d != cmp::Ordering::Equal
    {
        d
    } else {
        a.mantissa
            .abs()
            .partial_cmp(&b.mantissa.abs())
            .unwrap_or(std::cmp::Ordering::Equal)
    }
}

fn rv_to_f64(rv: &bridge::RealValue) -> f64 {
    rv.mantissa * 2.0f64.powi(rv.exponent as i32)
}

fn process_events(
    sn: &Arr,
    _an: &Arr,
    dev: &Arr,
    s_dev: &Arr,
    config: &BTreeMap<SeriesEventKind, crate::experiment::EventConfig>,
) -> (Vec<SeriesEvent>, Option<u64>) {
    let mut events = Vec::new();
    let mut stop_n: Option<u64> = None;

    let dev_r = match dev {
        Arr::Real(v) => v,
        _ => return (events, None),
    };
    let s_dev_r = match s_dev {
        Arr::Real(v) => v,
        _ => return (events, None),
    };
    let sn_r = match sn {
        Arr::Real(v) => v,
        _ => return (events, None),
    };

    let mut counters: BTreeMap<SeriesEventKind, i64> = BTreeMap::new();
    let n_total = dev_r.len();

    for k in 0..n_total {
        let mut triggered = Vec::new();

        // 1. Slow Accel
        if k < s_dev_r.len() && rv_abs_cmp(&dev_r[k], &s_dev_r[k]) == std::cmp::Ordering::Greater {
            triggered.push((
                SeriesEventKind::SlowAccel,
                format!(
                    "Deviation {} > series baseline {}",
                    rv_to_f64(&dev_r[k]),
                    rv_to_f64(&s_dev_r[k])
                ),
            ));
        }

        // 2. Monotone/Divergent
        if k > 0 {
            match rv_abs_cmp(&dev_r[k], &dev_r[k - 1]) {
                std::cmp::Ordering::Greater => {
                    triggered.push((
                        SeriesEventKind::DivergentAccel,
                        format!(
                            "Deviation increased from {} to {}",
                            rv_to_f64(&dev_r[k - 1]),
                            rv_to_f64(&dev_r[k])
                        ),
                    ));
                }
                std::cmp::Ordering::Equal => {
                    triggered.push((
                        SeriesEventKind::Monotone,
                        format!("Deviation stuck at {}", rv_to_f64(&dev_r[k])),
                    ));
                }
                _ => {}
            }
        }

        // 3. Sign Changed
        if k > 0 {
            let s_k = rv_to_f64(&dev_r[k]);
            let s_k1 = rv_to_f64(&dev_r[k - 1]);
            // If they have opposite signs, product is negative (or one is zero and other is not)
            // But usually we care about crosses: sign flips.
            if (s_k > 0.0 && s_k1 < 0.0) || (s_k < 0.0 && s_k1 > 0.0) {
                triggered.push((
                    SeriesEventKind::SignChanged,
                    format!("Error sign flipped from {} to {}", s_k1, s_k),
                ));
            }
        }

        // 4. Second Diff
        if k >= 2 {
            let v_k = rv_to_f64(&sn_r[k]);
            let v_k1 = rv_to_f64(&sn_r[k - 1]);
            let v_k2 = rv_to_f64(&sn_r[k - 2]);
            let diff1 = (v_k - v_k1).abs();
            let diff2 = (v_k1 - v_k2).abs();
            if diff1 >= diff2 {
                triggered.push((
                    SeriesEventKind::SecondDiff,
                    format!("Second diff growth: |{:.2e}| >= |{:.2e}|", diff1, diff2),
                ));
            }
        }

        // Process triggered events
        for (kind, desc) in triggered {
            let count = counters.entry(kind).or_insert(0);
            *count += 1;

            events.push(SeriesEvent {
                n: k as u64,
                kind,
                description: desc,
            });

            if let Some(cfg) = config.get(&kind) {
                if let Some(limit) = cfg.filter_after {
                    if *count >= limit && stop_n.is_none() {
                        stop_n = Some(k as u64);
                        events.push(SeriesEvent {
                            n: k as u64,
                            kind: SeriesEventKind::Trigger,
                            description: format!(
                                "Filters triggered due to {} limit ({})",
                                kind.to_string(),
                                limit
                            ),
                        });
                    }
                }
            }
        }
    }

    (events, stop_n)
}
fn sorted_args_json(params: &BTreeMap<String, serde_json::Value>) -> Result<String> {
    Ok(serde_json::to_string(params)?)
}
