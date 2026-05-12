//! Event calculation logic for series accelerations.
//! 
//! This module handles the computation of derived events (SlowAccel, Monotone, etc.)
//! from series and accel data. C++ errors are preserved and merged with derived events.

use crate::ffi::bridge::ffi::ErrorEvent;
use crate::compute::{Arr, SeriesEvent, SeriesEventKind};
use crate::ffi::bridge::ffi::RealValue;
use std::cmp;
use std::collections::BTreeMap;

/// C++ error events that should be cached (not recalculated).
/// Only stores n and description - kind is re-derived from message on load.
#[derive(Debug, Clone, Default, serde::Serialize, serde::Deserialize)]
pub struct CachedCppEvents {
    pub events: Vec<CppErrorEvent>,
}

/// Single C++ error event (minimal serialization - only n and message).
#[derive(Debug, Clone, serde::Serialize, serde::Deserialize)]
pub struct CppErrorEvent {
    pub n: u64,
    pub description: String,
}

/// Determine SeriesEventKind from error message.
fn kind_from_message(msg: &str) -> SeriesEventKind {
    if msg.to_lowercase().contains("division by zero") {
        SeriesEventKind::DivisionByZero
    } else {
        SeriesEventKind::Error
    }
}

/// All events for a series computation, including both cached C++ errors
/// and derived events that are calculated on the fly.
#[derive(Debug, Clone)]
pub struct SeriesEvents {
    events: Vec<SeriesEvent>,
    stop_ns: Vec<u64>,
}

impl SeriesEvents {
    /// Create a new SeriesEvents from C++ errors and derived event calculation.
    pub fn new(
        series_dev: &Arr,
        accel_sn: &Arr,
        accel_an: &Arr,
        accel_dev: &Arr,
        cpp_errors: Vec<ErrorEvent>,
        config: &BTreeMap<SeriesEventKind, crate::experiment::EventConfig>,
    ) -> Self {
        let mut events = Vec::new();

        // Add C++ errors first
        for err in cpp_errors {
            events.push(SeriesEvent {
                n: err.n,
                kind: kind_from_message(&err.message),
                description: err.message,
            });
        }

        // Calculate derived events
        let (derived_events, stop_ns) = calculate_derived_events(
            series_dev,
            accel_sn,
            accel_an,
            accel_dev,
            config,
        );

        events.extend(derived_events);
        events.sort_by_key(|e| e.n);

        Self { events, stop_ns }
    }

    /// Create from cached C++ errors (for cache hits).
    pub fn from_cached(
        series_dev: &Arr,
        accel_sn: &Arr,
        accel_an: &Arr,
        accel_dev: &Arr,
        cached: CachedCppEvents,
        config: &BTreeMap<SeriesEventKind, crate::experiment::EventConfig>,
    ) -> Self {
        let cpp_errors: Vec<ErrorEvent> = cached
            .events
            .into_iter()
            .map(|e| ErrorEvent {
                n: e.n,
                message: e.description,
            })
            .collect();

        Self::new(series_dev, accel_sn, accel_an, accel_dev, cpp_errors, config)
    }

    /// Create from existing events (for serialization use case).
    pub fn from_events(events: Vec<SeriesEvent>) -> Self {
        Self { events, stop_ns: Vec::new() }
    }

    /// Get all events (C++ + derived).
    pub fn all_events(&self) -> &[SeriesEvent] {
        &self.events
    }

    /// Get all stop_n values for filtering (from Trigger events).
    /// Each value corresponds to a different `filter_after` threshold being reached.
    pub fn stop_ns(&self) -> &[u64] {
        &self.stop_ns
    }

    /// Convert to Vec for AccelData.
    pub fn into_vec(self) -> Vec<SeriesEvent> {
        self.events
    }

    /// Get cached events only (for serialization).
    /// Only DivisionByZero and Error kinds are cached (not derived events).
    pub fn cached_events(&self) -> CachedCppEvents {
        let events = self
            .events
            .iter()
            .filter_map(|e| match e.kind {
                SeriesEventKind::DivisionByZero | SeriesEventKind::Error => Some(CppErrorEvent {
                    n: e.n,
                    description: e.description.clone(),
                }),
                _ => None,
            })
            .collect();

        CachedCppEvents { events }
    }
}

/// Calculate derived events (not cached, recalculated every time).
/// 
/// # Arguments
/// - `series_dev`: Series deviation data (for SlowAccel comparison)
/// - `accel_sn`: Accel Sn data (reserved for future use - currently unused)
/// - `accel_an`: Accel An data (reserved for future use - currently unused)
/// - `accel_dev`: Accel deviation data (for all other derived events)
fn calculate_derived_events(
    series_dev: &Arr,
    _accel_sn: &Arr,
    _accel_an: &Arr,
    accel_dev: &Arr,
    config: &BTreeMap<SeriesEventKind, crate::experiment::EventConfig>,
) -> (Vec<SeriesEvent>, Vec<u64>) {
    let mut events = Vec::new();
    let mut stop_ns: Vec<u64> = Vec::new();
    let mut triggered_limits: BTreeMap<SeriesEventKind, Vec<i64>> = BTreeMap::new();

    let s_dev_r = match series_dev {
        Arr::Real(v) => v,
        _ => return (events, Vec::new()),
    };
    let dev_r = match accel_dev {
        Arr::Real(v) => v,
        _ => return (events, Vec::new()),
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
            if (s_k > 0.0 && s_k1 < 0.0) || (s_k < 0.0 && s_k1 > 0.0) {
                triggered.push((
                    SeriesEventKind::SignChanged,
                    format!("Error sign flipped from {} to {}", s_k1, s_k),
                ));
            }
        }

        // 4. Second Diff
        if k >= 2 {
            let v_k = rv_to_f64(&dev_r[k]);
            let v_k1 = rv_to_f64(&dev_r[k - 1]);
            let v_k2 = rv_to_f64(&dev_r[k - 2]);
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
                let triggered = triggered_limits.entry(kind).or_default();
                for limit in &cfg.filter_after {
                    if *count >= *limit && !triggered.contains(limit) {
                        triggered.push(*limit);
                        stop_ns.push(k as u64);
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

    stop_ns.sort();
    stop_ns.dedup();
    (events, stop_ns)
}

fn rv_abs_cmp(a: &RealValue, b: &RealValue) -> std::cmp::Ordering {
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

fn rv_to_f64(rv: &RealValue) -> f64 {
    rv.mantissa * 2.0f64.powi(rv.exponent as i32)
}
