//! Core computation engine for executing FFI and caching results.
//!
//! This module provides a unified `ComputeCore` executor used by both the GUI
//! and Headless modes to ensure identical execution paths and caching behavior.
//!
//! ## Contract
//! `execute_and_cache` ALWAYS emits `SeriesComplete` and `AccelComplete` events
//! for every algorithm — whether the data came from cache or was freshly computed.
//! Callers (GUI, headless) must NOT read the cache directly; they purely react
//! to events.

use anyhow::Result;
use std::collections::BTreeMap;
use std::sync::mpsc as std_mpsc;
use std::sync::{Arc, Mutex};
use uuid::Uuid;

use super::task::{AccelParams, SeriesParams};
use crate::cache::Cache;
use crate::experiment::NoiseDef;
use crate::ffi::{AccelResult, ComputeEvent, ComputeEventBody, SeriesResult, ShanksLibrary};

/// Deterministic serialization of parameters to ensure caching consistency.
/// Always produces a sorted-key JSON string regardless of HashMap iteration order.
pub fn to_sorted_json<T: serde::Serialize>(
    map: &std::collections::HashMap<String, T>,
) -> Result<String> {
    let sorted_map: BTreeMap<_, _> = map.iter().collect();
    Ok(serde_json::to_string(&sorted_map)?)
}

/// Convert NoiseDef to JSON parameters specifically formatted for the FFI library.
pub fn build_noise_json(noise: &NoiseDef) -> String {
    serde_json::json!({
        "type": noise.noise_type.to_lowercase(),
        "method": noise.method,
        "param1": noise.param1,
        "param2": noise.param2,
        "seed": noise.seed
    })
    .to_string()
}

/// Helper to generate a distinct algorithm name if multiple algorithms
/// have the exact same base name (differentiating by parameters).
pub fn build_distinct_name(accel: &AccelParams, all_accels: &[AccelParams]) -> String {
    let same_name_accels: Vec<_> = all_accels.iter().filter(|a| a.name == accel.name).collect();
    if same_name_accels.len() <= 1 {
        return accel.name.clone();
    }

    let mut varying_keys = Vec::new();
    let keys: Vec<_> = accel.params.keys().collect();
    for &k in &keys {
        let val = accel.params.get(k).unwrap();
        let mut all_same = true;
        for other in &same_name_accels {
            if let Some(other_val) = other.params.get(k) {
                if other_val != val {
                    all_same = false;
                    break;
                }
            } else {
                all_same = false;
                break;
            }
        }
        if !all_same {
            varying_keys.push(k);
        }
    }

    if varying_keys.is_empty() {
        return accel.name.clone();
    }

    let mut param_strs = Vec::new();
    for k in varying_keys {
        let v = accel.params.get(k).unwrap();
        let v_str = match v {
            crate::ffi::ParamValue::Float(f) => f.to_string(),
            crate::ffi::ParamValue::Int(i) => i.to_string(),
            crate::ffi::ParamValue::Bool(b) => b.to_string(),
            crate::ffi::ParamValue::String(s) => s.clone(),
        };
        param_strs.push(format!("{}={}", k, v_str));
    }
    param_strs.sort();
    format!("{} ({})", accel.name, param_strs.join(", "))
}

/// Core execution engine for series and acceleration computation.
///
/// ## Single source of truth
/// Always emit `SeriesComplete` and `AccelComplete` events — for cached or
/// freshly computed data alike. Never rely on callers to read the cache.
#[derive(Clone)]
pub struct ComputeCore {
    pub library: Arc<ShanksLibrary>,
    pub cache: Arc<Mutex<Cache>>,
}

impl ComputeCore {
    /// Create a new ComputeCore backend.
    pub fn new(library: Arc<ShanksLibrary>, cache: Arc<Mutex<Cache>>) -> Self {
        Self { library, cache }
    }

    /// Run a full computation pipeline, emitting events for every result.
    ///
    /// **Events emitted (always, cache or fresh):**
    /// - `SeriesComplete` — after series data is available
    /// - `AccelComplete`  — after each algorithm result is available
    /// - `Complete`       — when the whole task is done
    /// - `Error`          — on any failure
    /// - `Cancelled`      — if the task was cancelled mid-flight
    ///
    /// Returns `(fully_cached, errors)`.
    pub fn execute_and_cache(
        &self,
        task_id: Option<Uuid>,
        precision: &str,
        series: &SeriesParams,
        algorithms: &[AccelParams],
        n_points: u64,
        noise: Option<&NoiseDef>,
        filters: &[crate::experiment::FilterDef],
        event_tx: Option<std_mpsc::Sender<ComputeEvent>>,
        cancel_flags: Option<Arc<Mutex<std::collections::HashSet<Uuid>>>>,
    ) -> Result<(bool, Vec<String>)> {
        let mut errors = Vec::new();

        let send_event = |body: ComputeEventBody| {
            if let (Some(id), Some(tx)) = (task_id, &event_tx) {
                let _ = tx.send(ComputeEvent { task_id: id, body });
            }
        };

        let check_cancel = || -> bool {
            if let (Some(id), Some(flags)) = (task_id, &cancel_flags) {
                if flags.lock().unwrap().contains(&id) {
                    send_event(ComputeEventBody::Cancelled);
                    return true;
                }
            }
            false
        };

        if check_cancel() {
            return Ok((false, errors));
        }

        // --- Deterministic key generation ---
        let args_json = to_sorted_json(&series.params)?;
        let x_value = series
            .params
            .get("x")
            .and_then(|v| v.as_f64())
            .map(|v| v.to_string())
            .unwrap_or_else(|| series.x_value.clone());

        let noise_json_opt = noise.map(|n| build_noise_json(n));

        // --- Cache check: classify each algorithm ---
        // cached_results: algorithms whose data is fully available in cache at n_points
        // algorithms_to_compute: algorithms that must be (re-)computed
        let mut cached_series_result: Option<SeriesResult> = None;
        let mut cached_results: Vec<(AccelParams, AccelResult)> = Vec::new();
        let mut algorithms_to_compute: Vec<(AccelParams, String, Option<i64>)> = Vec::new();
        let mut cached_series_id: Option<i64> = None;

        {
            let cache_lock = self.cache.lock().unwrap();

            let series_exists_id = cache_lock
                .series_exists(
                    &series.name,
                    precision,
                    &x_value,
                    &args_json,
                    noise_json_opt.as_deref(),
                )
                .unwrap_or(None);

            if let Some(s_id) = series_exists_id {
                cached_series_id = Some(s_id);

                // Try loading the cached series result
                if let Ok(Some(res)) = cache_lock.get_series_result(s_id) {
                    if res.sn.len() >= n_points as usize {
                        cached_series_result = Some(res);
                    }
                }

                for accel in algorithms {
                    let method_args_json = to_sorted_json(&accel.params)?;
                    let m_val = accel
                        .params
                        .get("m")
                        .and_then(|v| v.as_f64().map(|f| f as i64));

                    let mut needs_compute = true;
                    if let Ok(Some(accel_id)) =
                        cache_lock.acceleration_exists(s_id, &accel.name, m_val, &method_args_json)
                    {
                        if let Ok(Some(accel_res)) = cache_lock.get_accel_result(accel_id) {
                            if accel_res.values.len() >= n_points as usize {
                                cached_results.push((accel.clone(), accel_res));
                                needs_compute = false;
                            }
                        }
                    }
                    if needs_compute {
                        algorithms_to_compute.push((accel.clone(), method_args_json, m_val));
                    }
                }
            } else {
                // Series not in cache at all — compute everything
                for accel in algorithms {
                    let method_args_json = to_sorted_json(&accel.params)?;
                    let m_val = accel
                        .params
                        .get("m")
                        .and_then(|v| v.as_f64().map(|f| f as i64));
                    algorithms_to_compute.push((accel.clone(), method_args_json, m_val));
                }
            }
        }

        let fully_cached = algorithms_to_compute.is_empty() && cached_series_result.is_some();

        let unique_series_name = format!(
            "{} ({}{})",
            series.name,
            precision,
            noise
                .map(|n| format!(", {}", n.noise_type))
                .unwrap_or_default()
        );

        // --- Emit cached results immediately ---
        // This is the key change: the GUI only ever hears about results via events.
        if let Some(ref res) = cached_series_result {
            send_event(ComputeEventBody::SeriesComplete {
                name: unique_series_name.clone(),
                result: res.clone(),
            });
        }
        for (accel, res) in &cached_results {
            send_event(ComputeEventBody::AccelComplete {
                name: format!(
                    "{} - {}",
                    unique_series_name,
                    build_distinct_name(accel, algorithms)
                ),
                result: res.clone(),
            });
        }

        if fully_cached {
            send_event(ComputeEventBody::Complete);
            return Ok((true, errors));
        }

        // --- Compute what's missing ---
        // We need to generate the series via FFI (either we don't have it, or
        // we need it to run algorithms).
        if check_cancel() {
            return Ok((false, errors));
        }

        let series_handle = match noise {
            Some(n) => self.library.series_create_with_noise(
                &series.name,
                precision,
                &x_value,
                &args_json,
                &n.noise_type.to_lowercase(),
                &n.method.to_lowercase(),
                n.param1,
                n.param2,
                n.seed,
            ),
            None => self
                .library
                .series_create(&series.name, precision, &x_value, &args_json),
        };

        let series_handle = match series_handle {
            Ok(h) => h,
            Err(e) => {
                let msg = format!("Failed to create series '{}': {}", series.name, e);
                log::error!("{}", msg);
                errors.push(msg.clone());
                send_event(ComputeEventBody::Error { error: msg });
                return Ok((false, errors));
            }
        };

        if check_cancel() {
            self.library.series_destroy(series_handle);
            return Ok((false, errors));
        }

        let series_result_json = match self
            .library
            .series_generate(&series_handle, n_points, false)
        {
            Ok(r) => r,
            Err(e) => {
                let msg = format!("Failed to generate series '{}': {}", series.name, e);
                log::error!("{}", msg);
                errors.push(msg.clone());
                self.library.series_destroy(series_handle);
                send_event(ComputeEventBody::Error { error: msg });
                return Ok((false, errors));
            }
        };

        let mut series_result: SeriesResult = serde_json::from_str(&series_result_json)?;

        if series_result.sum.is_none() {
            if let Ok(sum_str) = self.library.series_get_sum(&series_handle) {
                if !sum_str.is_empty() {
                    if let Ok(sum_point) = serde_json::from_str::<crate::ffi::SeriesPoint>(&sum_str)
                    {
                        series_result.sum = Some(sum_point);
                    }
                }
            }
        }

        // Emit SeriesComplete only if we didn't already emit it from cache
        if cached_series_result.is_none() {
            send_event(ComputeEventBody::SeriesComplete {
                name: unique_series_name.clone(),
                result: series_result.clone(),
            });
        }

        // --- Persist series ---
        let final_series_id = {
            let mut cache = self.cache.lock().unwrap();
            let id = if let Some(s_id) = cached_series_id {
                s_id
            } else {
                let sum_json = series_result
                    .sum
                    .as_ref()
                    .and_then(|s| serde_json::to_string(s).ok());
                cache
                    .insert_series(
                        &series.name,
                        precision,
                        &x_value,
                        &args_json,
                        noise_json_opt.as_deref(),
                        None,
                        sum_json.as_deref(),
                    )
                    .map_err(|e| {
                        log::error!("Failed to insert series into cache: {}", e);
                        e
                    })
                    .unwrap_or(-1)
            };

            if id != -1 {
                let mut db_points = Vec::with_capacity(series_result.sn.len());
                let empty_point = crate::ffi::SeriesPoint::Real(crate::ffi::ScientificValue {
                    mantissa: 0.0,
                    exponent: 0,
                });
                for i in 0..series_result.sn.len() {
                    let n = (i + 1) as i64;
                    let sn = series_result.sn.get(i);
                    let an = if i < series_result.an.len() {
                        series_result.an.get(i)
                    } else {
                        empty_point.clone()
                    };
                    let (sn_real, sn_imag, sn_exp) = match sn {
                        crate::ffi::SeriesPoint::Real(r) => {
                            (r.mantissa.to_string(), String::new(), r.exponent)
                        }
                        crate::ffi::SeriesPoint::Complex(c) => (
                            c.real.mantissa.to_string(),
                            c.imag.mantissa.to_string(),
                            c.real.exponent,
                        ),
                        crate::ffi::SeriesPoint::Interval(ref i) => {
                            (serde_json::to_string(i).unwrap(), String::new(), 0)
                        }
                        crate::ffi::SeriesPoint::CInterval(ref ci) => {
                            (serde_json::to_string(ci).unwrap(), String::new(), 0)
                        }
                    };
                    let (an_real, an_imag, an_exp) = match an {
                        crate::ffi::SeriesPoint::Real(r) => {
                            (r.mantissa.to_string(), String::new(), r.exponent)
                        }
                        crate::ffi::SeriesPoint::Complex(c) => (
                            c.real.mantissa.to_string(),
                            c.imag.mantissa.to_string(),
                            c.real.exponent,
                        ),
                        crate::ffi::SeriesPoint::Interval(ref i_val) => {
                            (serde_json::to_string(i_val).unwrap(), String::new(), 0)
                        }
                        crate::ffi::SeriesPoint::CInterval(ref ci) => {
                            (serde_json::to_string(ci).unwrap(), String::new(), 0)
                        }
                    };
                    let dev_str = series_result.deviations.get(i).format();
                    db_points.push((
                        n,
                        sn_real,
                        sn_imag,
                        sn_exp,
                        an_real,
                        an_imag,
                        an_exp,
                        dev_str,
                    ));
                }
                if !db_points.is_empty() {
                    if let Err(e) = cache.insert_series_points(id, &db_points) {
                        log::error!("Failed to insert series points into cache for id={}: {}", id, e);
                    }
                }
            }
            id
        };

        // --- Compute missing algorithms ---
        for (accel, method_args_json, m_val) in algorithms_to_compute {
            if check_cancel() {
                self.library.series_destroy(series_handle);
                return Ok((false, errors));
            }

            let accel_handle =
                match self
                    .library
                    .accel_create(&accel.name, precision, &method_args_json)
                {
                    Ok(h) => h,
                    Err(e) => {
                        let msg = format!("Failed to create algorithm '{}': {}", accel.name, e);
                        errors.push(msg.clone());
                        send_event(ComputeEventBody::Error { error: msg });
                        continue;
                    }
                };

            let accel_result_json = match self.library.accel_apply(
                &accel_handle,
                &series_handle,
                n_points,
                m_val.unwrap_or(5) as u64,
                true,
            ) {
                Ok(r) => r,
                Err(e) => {
                    self.library.accel_destroy(accel_handle);
                    let msg = format!("Failed to apply algorithm '{}': {}", accel.name, e);
                    errors.push(msg.clone());
                    send_event(ComputeEventBody::Error { error: msg });
                    continue;
                }
            };

            self.library.accel_destroy(accel_handle);

            let mut parsed_accel: AccelResult = match serde_json::from_str(&accel_result_json) {
                Ok(r) => r,
                Err(e) => {
                    let msg = format!("JSON parse error for '{}': {}\n", accel.name, e);
                    log::debug!("{accel_result_json}");
                    errors.push(msg.clone());
                    send_event(ComputeEventBody::Error { error: msg });
                    continue;
                }
            };

            // --- Event Detection & Smoothing ---
            // Simple divergence detection: if deviation increases 3 times consecutively
            let mut divergence_start = None;
            let mut inc_count = 0;
            for i in 1..parsed_accel.deviations.len() {
                if parsed_accel.deviations.to_f64(i) > parsed_accel.deviations.to_f64(i - 1) {
                    inc_count += 1;
                    if inc_count >= 3 && divergence_start.is_none() {
                        divergence_start = Some(i - 3); // Start of divergence
                        break;
                    }
                } else {
                    inc_count = 0;
                }
            }

            if let Some(start_n) = divergence_start {
                let tail_len = parsed_accel.values.len().saturating_sub(start_n);
                if tail_len >= 5 {
                    // Extract tail values as strings
                    let mut tail_strings = Vec::with_capacity(tail_len);
                    for i in start_n..parsed_accel.values.len() {
                        let val_str = match parsed_accel.values.get(i) {
                            crate::ffi::SeriesPoint::Real(r) => r.format(),
                            crate::ffi::SeriesPoint::Complex(c) => c.format(),
                            crate::ffi::SeriesPoint::Interval(_) => continue, // Skip unsupported precision
                            crate::ffi::SeriesPoint::CInterval(_) => continue,
                        };
                        tail_strings.push(val_str);
                    }

                    if tail_strings.len() == tail_len {
                        for filter_def in filters {
                            let filter_type = &filter_def.filter_type;
                            let args_json = match serde_json::to_string(&filter_def.args) {
                                Ok(json) => json,
                                Err(e) => {
                                    log::error!("Failed to serialize filter args: {}", e);
                                    continue;
                                }
                            };

                            if let Ok(limit_json) = self.library.compute_smoothed_limit(
                                precision,
                                &tail_strings,
                                filter_type,
                                &args_json,
                            ) {
                                log::info!("Filtered limit JSON ({}): {}", filter_type, limit_json);
                                if !limit_json.is_empty() {
                                    if let Ok(limit_points) = serde_json::from_str::<Vec<crate::ffi::SeriesPoint>>(&limit_json) {
                                        log::info!("Successfully parsed {} limit points for filter {}", limit_points.len(), filter_type);
                                        // Only add the event once
                                        if parsed_accel.filtered_estimates.is_empty() {
                                            parsed_accel.events.push(crate::ffi::ComputeEventEntry {
                                                n: start_n as u64,
                                                name: "divergent_accel".to_string(),
                                                description: format!("Divergence detected, applied filters to tail."),
                                            });
                                        }
                                        parsed_accel.filtered_estimates.push(crate::ffi::SmoothedEstimate {
                                            event_name: "divergent_accel".to_string(),
                                            filter: filter_type.clone(),
                                            limit: limit_points,
                                            start_n: start_n as u64,
                                            length: tail_len as u64,
                                        });
                                    } else {
                                        log::error!("Failed to parse smoothed limit JSON into Vec<SeriesPoint> for filter {}!", filter_type);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            send_event(ComputeEventBody::AccelComplete {
                name: format!(
                    "{} - {}",
                    unique_series_name,
                    build_distinct_name(&accel, algorithms)
                ),
                result: parsed_accel.clone(),
            });

            // Persist acceleration result
            if final_series_id != -1 {
                let mut cache = self.cache.lock().unwrap();
                let accel_id = if let Ok(Some(id)) = cache.acceleration_exists(
                    final_series_id,
                    &accel.name,
                    m_val,
                    &method_args_json,
                ) {
                    id
                } else {
                    let profiling_json = parsed_accel
                        .profiling
                        .as_ref()
                        .map(|p| serde_json::to_string(p).unwrap());
                    cache
                        .insert_acceleration(
                            final_series_id,
                            &accel.name,
                            m_val,
                            &method_args_json,
                            profiling_json.as_deref(),
                        )
                        .map_err(|e| {
                            log::error!("Failed to insert acceleration into cache: {}", e);
                            e
                        })
                        .unwrap_or(-1)
                };

                if accel_id != -1 {
                    let mut db_points = Vec::with_capacity(parsed_accel.values.len());

                    for i in 0..parsed_accel.values.len() {
                        let n = (i + 1) as i64;
                        let val_opt = if parsed_accel.valid.get(i).copied().unwrap_or(false) {
                            Some(parsed_accel.values.get(i))
                        } else {
                            None
                        };
                        let dev = parsed_accel.deviations.get(i);
                        let prof_json = if let Some(ref p) = parsed_accel.profiling {
                            serde_json::json!({
                                "add": p.add.get(i).copied().unwrap_or(0),
                                "mul": p.mul.get(i).copied().unwrap_or(0),
                                "div": p.div.get(i).copied().unwrap_or(0),
                                "special": p.special.get(i).copied().unwrap_or(0),
                            })
                            .to_string()
                        } else {
                            String::new()
                        };

                        if let Some(val) = val_opt {
                            let (v_real, v_imag, v_exp) = match val {
                                crate::ffi::SeriesPoint::Real(r) => {
                                    (r.mantissa.to_string(), String::new(), r.exponent)
                                }
                                crate::ffi::SeriesPoint::Complex(c) => (
                                    c.real.mantissa.to_string(),
                                    c.imag.mantissa.to_string(),
                                    c.real.exponent,
                                ),
                                crate::ffi::SeriesPoint::Interval(ref i_val) => {
                                    (serde_json::to_string(i_val).unwrap(), String::new(), 0)
                                }
                                crate::ffi::SeriesPoint::CInterval(ref ci) => {
                                    (serde_json::to_string(ci).unwrap(), String::new(), 0)
                                }
                            };
                            let dev_str = dev.format();
                            db_points.push((n, v_real, v_imag, v_exp, dev_str, prof_json));
                        } else {
                            db_points.push((
                                n,
                                String::new(),
                                String::new(),
                                0,
                                String::new(),
                                prof_json,
                            ));
                        }
                    }
                    if !db_points.is_empty() {
                        let _ = cache.insert_accel_points(accel_id, &db_points);
                    }

                    if !parsed_accel.events.is_empty() {
                        let _ = cache.insert_accel_events(accel_id, &parsed_accel.events);
                    }
                    if !parsed_accel.filtered_estimates.is_empty() {
                        let _ = cache.insert_filtered_estimates(accel_id, &parsed_accel.filtered_estimates);
                    }
                }
            }
        }

        self.library.series_destroy(series_handle);
        send_event(ComputeEventBody::Complete);
        Ok((false, errors))
    }
}
