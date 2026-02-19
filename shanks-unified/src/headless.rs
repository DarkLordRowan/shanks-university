//! Headless runner for batch computation without UI.
//!
//! This module provides functionality to run all computations from an
//! experiment configuration file without launching the GUI.

use anyhow::Result;
use std::sync::{Arc, RwLock};
use std::time::Instant;

use crate::cache::Cache;
use crate::config::{ExperimentConfig, MethodInstance, NoiseDef, SeriesInstance};
use crate::ffi::ShanksLibrary;

/// Progress information for headless runs.
#[derive(Debug, Clone)]
pub struct ProgressInfo {
    /// Current trial number
    pub current: usize,
    /// Total number of trials
    pub total: usize,
    /// Current series name
    pub series_name: String,
    /// Current method name
    pub method_name: String,
    /// Current precision
    pub precision: String,
    /// Current status
    pub status: Status,
    /// Elapsed time
    pub elapsed_secs: f64,
}

/// Status of a trial.
#[derive(Debug, Clone)]
pub enum Status {
    /// Computing
    Computing,
    /// Result cached
    Cached,
    /// Completed successfully
    Complete,
    /// Error occurred
    Error(String),
}

/// Summary of a headless run.
#[derive(Debug, Clone)]
pub struct RunSummary {
    /// Total number of trials
    pub total_trials: usize,
    /// Number of successful trials
    pub successful: usize,
    /// Number of cached trials (skipped)
    pub cached: usize,
    /// Number of failed trials
    pub failed: usize,
    /// Total time in seconds
    pub total_time_secs: f64,
    /// Error messages
    pub errors: Vec<String>,
}

/// Headless runner for batch computation.
pub struct HeadlessRunner {
    config: ExperimentConfig,
    library: Arc<ShanksLibrary>,
    cache: Arc<RwLock<Cache>>,
    progress_callback: Option<Box<dyn Fn(ProgressInfo) + Send + Sync>>,
    precisions: Vec<String>,
}

impl HeadlessRunner {
    /// Create a new headless runner.
    pub fn new(
        config: ExperimentConfig,
        library: Arc<ShanksLibrary>,
        cache: Cache,
    ) -> Result<Self> {
        let precisions = config.precisions.clone().unwrap_or_else(|| {
            // Default precisions
            vec!["F64".to_string()]
        });

        Ok(Self {
            config,
            library,
            cache: Arc::new(RwLock::new(cache)),
            progress_callback: None,
            precisions,
        })
    }

    /// Set a progress callback.
    pub fn set_progress_callback<F: Fn(ProgressInfo) + Send + Sync + 'static>(&mut self, f: F) {
        self.progress_callback = Some(Box::new(f));
    }

    /// Run all computations from the config.
    pub fn run_all(&mut self) -> Result<RunSummary> {
        let start_time = Instant::now();

        let series = self.config.expand_series();
        let methods = self.config.expand_methods();
        let noises = &self.config.noises;

        let total = self.config.total_trials(&self.precisions);
        let mut current = 0;
        let mut successful = 0;
        let mut cached = 0;
        let mut failed = 0;
        let mut errors = Vec::new();

        if series.is_empty() {
            log::warn!("No series defined in config");
            return Ok(RunSummary {
                total_trials: 0,
                successful: 0,
                cached: 0,
                failed: 0,
                total_time_secs: start_time.elapsed().as_secs_f64(),
                errors: vec!["No series defined in config".to_string()],
            });
        }

        if methods.is_empty() {
            log::warn!("No methods defined in config");
            return Ok(RunSummary {
                total_trials: 0,
                successful: 0,
                cached: 0,
                failed: 0,
                total_time_secs: start_time.elapsed().as_secs_f64(),
                errors: vec!["No methods defined in config".to_string()],
            });
        }

        log::info!(
            "Starting headless run: {} series × {} methods × {} noises × {} precisions = {} trials",
            series.len(),
            methods.len(),
            noises.len().max(1),
            self.precisions.len(),
            total
        );

        for precision in &self.precisions {
            for series_inst in &series {
                for method_inst in &methods {
                    // Handle noise: if no noises defined, run without noise
                    let noise_iter: Vec<Option<&NoiseDef>> = if noises.is_empty() {
                        vec![None]
                    } else {
                        noises.iter().map(Some).collect()
                    };

                    for noise_opt in noise_iter {
                        current += 1;

                        let result = self.run_trial(
                            precision,
                            series_inst,
                            method_inst,
                            noise_opt,
                            current,
                            total,
                            start_time.elapsed().as_secs_f64(),
                        );

                        match result {
                            Ok(was_cached) => {
                                if was_cached {
                                    cached += 1;
                                } else {
                                    successful += 1;
                                }
                            }
                            Err(e) => {
                                failed += 1;
                                let error_msg = format!(
                                    "Trial {} failed: {} - {} - {}: {}",
                                    current, series_inst.name, method_inst.name, precision, e
                                );
                                log::error!("{}", error_msg);
                                errors.push(error_msg);
                            }
                        }
                    }
                }
            }
        }

        Ok(RunSummary {
            total_trials: current,
            successful,
            cached,
            failed,
            total_time_secs: start_time.elapsed().as_secs_f64(),
            errors,
        })
    }

    /// Run a single trial.
    fn run_trial(
        &self,
        precision: &str,
        series: &SeriesInstance,
        method: &MethodInstance,
        noise: Option<&NoiseDef>,
        current: usize,
        total: usize,
        elapsed: f64,
    ) -> Result<bool> {
        // Report progress
        self.report_progress(ProgressInfo {
            current,
            total,
            series_name: series.name.clone(),
            method_name: method.name.clone(),
            precision: precision.to_string(),
            status: Status::Computing,
            elapsed_secs: elapsed,
        });

        // Build cache key
        let cache_key = self.make_cache_key(precision, series, method, noise);

        // Check cache first
        {
            let cache = self.cache.read().unwrap();
            if let Ok(Some(_)) = cache.series_exists(
                &cache_key.name,
                &cache_key.precision,
                &cache_key.x_value,
                &cache_key.args,
            ) {
                log::debug!("Trial {}/{}: cached", current, total);
                self.report_progress(ProgressInfo {
                    current,
                    total,
                    series_name: series.name.clone(),
                    method_name: method.name.clone(),
                    precision: precision.to_string(),
                    status: Status::Cached,
                    elapsed_secs: elapsed,
                });
                return Ok(true);
            }
        }

        // Create series
        let args_json = serde_json::to_string(&series.args)?;
        let x_value = series
            .args
            .get("x")
            .and_then(|v| v.as_f64())
            .map(|v| v.to_string())
            .unwrap_or_else(|| "1.0".to_string());

        let series_handle = if let Some(noise_def) = noise {
            let noise_json = self.build_noise_json(noise_def);
            self.library.series_create_with_noise(
                &series.name,
                precision,
                &x_value,
                &args_json,
                &noise_json,
            )?
        } else {
            self.library.series_create(&series.name, precision, &x_value, &args_json)?
        };

        // Generate series data
        let n = method.n as u64;
        let series_result_json = self.library.series_generate(&series_handle, n, false)?;
        let series_result: crate::ffi::SeriesResult = serde_json::from_str(&series_result_json)?;

        // Create acceleration algorithm
        let method_args_json = serde_json::to_string(&method.args)?;
        let accel_handle =
            self.library.accel_create(&method.name, precision, &method_args_json)?;

        // Apply acceleration
        let accel_result_json = self.library.accel_apply(&accel_handle, &series_handle, n, method.m as u64)?;
        let _accel_result: crate::ffi::AccelResult = serde_json::from_str(&accel_result_json)?;

        // Cache results
        {
            let mut cache = self.cache.write().unwrap();
            let series_id = cache.insert_series(
                &series.name,
                precision,
                &x_value,
                &args_json,
                noise.map(|n| self.build_noise_json(n)).as_deref(),
            )?;

            // Insert series points
            let points: Vec<(i64, String, String, i64, String, String, i64, String)> = series_result
                .sn
                .iter()
                .enumerate()
                .map(|(i, p)| {
                    let (sn_real, sn_imag, sn_exp) = match p {
                        crate::ffi::SeriesPoint::Real(v) => {
                            (v.mantissa.to_string(), "0".to_string(), v.exponent)
                        }
                        crate::ffi::SeriesPoint::Complex(c) => (
                            c.real.mantissa.to_string(),
                            c.imag.mantissa.to_string(),
                            c.real.exponent,
                        ),
                    };
                    let an = series_result.an.get(i);
                    let (an_real, an_imag, an_exp) = an
                        .map(|p| match p {
                            crate::ffi::SeriesPoint::Real(v) => {
                                (v.mantissa.to_string(), "0".to_string(), v.exponent)
                            }
                            crate::ffi::SeriesPoint::Complex(c) => (
                                c.real.mantissa.to_string(),
                                c.imag.mantissa.to_string(),
                                c.real.exponent,
                            ),
                        })
                        .unwrap_or(("0".to_string(), "0".to_string(), 0));

                    (i as i64, sn_real, sn_imag, sn_exp, an_real, an_imag, an_exp, String::new())
                })
                .collect();

            if !points.is_empty() {
                cache.insert_series_points(series_id, &points)?;
            }
        }

        // Cleanup
        self.library.series_destroy(series_handle);
        self.library.accel_destroy(accel_handle);

        // Report completion
        self.report_progress(ProgressInfo {
            current,
            total,
            series_name: series.name.clone(),
            method_name: method.name.clone(),
            precision: precision.to_string(),
            status: Status::Complete,
            elapsed_secs: elapsed,
        });

        Ok(false)
    }

    fn build_noise_json(&self, noise: &NoiseDef) -> String {
        serde_json::json!({
            "type": noise.noise_type.to_lowercase(),
            "method": noise.method,
            "param1": noise.param1,
            "param2": noise.param2,
            "seed": noise.seed
        })
        .to_string()
    }

    fn make_cache_key(
        &self,
        precision: &str,
        series: &SeriesInstance,
        method: &MethodInstance,
        noise: Option<&NoiseDef>,
    ) -> CacheKey {
        CacheKey {
            name: series.name.clone(),
            precision: precision.to_string(),
            x_value: series
                .args
                .get("x")
                .and_then(|v| v.as_f64())
                .map(|v| v.to_string())
                .unwrap_or_default(),
            args: serde_json::to_string(&series.args).unwrap_or_default(),
            method: method.name.clone(),
            noise: noise.map(|n| format!("{}_{}", n.noise_type, n.seed)),
        }
    }

    fn report_progress(&self, info: ProgressInfo) {
        if let Some(ref callback) = self.progress_callback {
            callback(info);
        }
    }
}

struct CacheKey {
    name: String,
    precision: String,
    x_value: String,
    args: String,
    method: String,
    noise: Option<String>,
}
