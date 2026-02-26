//! Headless runner for batch computation without UI.
//!
//! This module provides functionality to run all computations from an
//! experiment configuration file without launching the GUI.

use anyhow::Result;
use std::sync::{Arc, Mutex};
use std::time::Instant;

use crate::cache::Cache;
use crate::compute::engine::ComputeEngine;
use crate::compute::task::{AccelParams, ComputeTask, SeriesParams};
use crate::config::{ExperimentConfig, NoiseDef};
use crate::ffi::{ComputeEventBody, ShanksLibrary};

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
    cache: Arc<Mutex<Cache>>,
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
            cache: Arc::new(Mutex::new(cache)),
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

        let (tx, rx) = std::sync::mpsc::channel();
        let mut engine = ComputeEngine::new(self.library.clone(), self.cache.clone(), tx);
        let mut active_tasks = 0;
        let mut task_mappings = std::collections::HashMap::new();

        let noise_iter: Vec<Option<&NoiseDef>> = if noises.is_empty() {
            vec![None]
        } else {
            noises.iter().map(Some).collect()
        };

        for precision in &self.precisions {
            for series_inst in &series {
                for noise_opt in &noise_iter {
                    let mut series_params_map = std::collections::HashMap::new();
                    for (k, v) in &series_inst.args {
                        let pv = match v {
                            serde_json::Value::Number(n) => {
                                if let Some(i) = n.as_i64() {
                                    crate::ffi::ParamValue::Int(i)
                                } else if let Some(f) = n.as_f64() {
                                    crate::ffi::ParamValue::Float(f)
                                } else {
                                    crate::ffi::ParamValue::String(n.to_string())
                                }
                            }
                            serde_json::Value::String(s) => {
                                crate::ffi::ParamValue::String(s.clone())
                            }
                            serde_json::Value::Bool(b) => crate::ffi::ParamValue::Bool(*b),
                            _ => crate::ffi::ParamValue::String(v.to_string()),
                        };
                        series_params_map.insert(k.clone(), pv);
                    }

                    let series_params = SeriesParams {
                        name: series_inst.name.clone(),
                        x_value: series_inst
                            .args
                            .get("x")
                            .and_then(|v| v.as_f64())
                            .map(|v| v.to_string())
                            .unwrap_or_else(|| "1.0".to_string()),
                        params: series_params_map,
                    };

                    let mut bundled_algorithms = Vec::new();
                    let mut max_n_points = 10;

                    for method_inst in &methods {
                        let mut accel_args = std::collections::HashMap::new();
                        for (k, v) in &method_inst.args {
                            let pv = match v {
                                serde_json::Value::Number(n) => {
                                    if let Some(i) = n.as_i64() {
                                        crate::ffi::ParamValue::Int(i)
                                    } else if let Some(f) = n.as_f64() {
                                        crate::ffi::ParamValue::Float(f)
                                    } else {
                                        crate::ffi::ParamValue::String(n.to_string())
                                    }
                                }
                                serde_json::Value::String(s) => {
                                    crate::ffi::ParamValue::String(s.clone())
                                }
                                serde_json::Value::Bool(b) => crate::ffi::ParamValue::Bool(*b),
                                _ => crate::ffi::ParamValue::String(v.to_string()),
                            };
                            accel_args.insert(k.clone(), pv);
                        }

                        if !accel_args.contains_key("m") {
                            accel_args.insert(
                                "m".to_string(),
                                crate::ffi::ParamValue::Int(method_inst.m as i64),
                            );
                        }

                        bundled_algorithms.push(AccelParams {
                            name: method_inst.name.clone(),
                            params: accel_args,
                        });

                        if method_inst.n > max_n_points {
                            max_n_points = method_inst.n;
                        }
                    }

                    let task = ComputeTask {
                        id: uuid::Uuid::new_v4(),
                        precision: precision.to_string(),
                        series: series_params.clone(),
                        n_points: max_n_points as u64,
                        noise: noise_opt.cloned(),
                        algorithms: bundled_algorithms.clone(),
                        filters: self.config.filters.clone(),
                    };

                    if let Ok(id) = engine.start_task(task.clone()) {
                        task_mappings.insert(id, task);
                        active_tasks += 1;
                    }
                }
            }
        }

        let mut successful = 0;
        let mut failed = 0;
        let mut errors = Vec::new();

        while active_tasks > 0 {
            if let Ok(event) = rx.try_recv() {
                match event.body {
                    ComputeEventBody::Started => {}
                    ComputeEventBody::Progress { .. } => {}
                    ComputeEventBody::SeriesComplete { .. } => {}
                    ComputeEventBody::AccelComplete { .. } => {
                        self.report_progress(ProgressInfo {
                            current: successful + failed,
                            total,
                            series_name: task_mappings
                                .get(&event.task_id)
                                .unwrap()
                                .series
                                .name
                                .clone(),
                            method_name: "(batch item)".to_string(),
                            precision: task_mappings.get(&event.task_id).unwrap().precision.clone(),
                            status: Status::Complete,
                            elapsed_secs: start_time.elapsed().as_secs_f64(),
                        });
                    }
                    ComputeEventBody::Error { error } => {
                        let task = task_mappings.get(&event.task_id).unwrap();
                        let error_msg = format!(
                            "Task {} failed for series {}: {}",
                            event.task_id, task.series.name, error
                        );
                        log::error!("{}", error_msg);
                        errors.push(error_msg);
                        failed += task.algorithms.len();
                        active_tasks -= 1;
                        engine.wait_for_task(event.task_id);
                    }
                    ComputeEventBody::Complete => {
                        let task = task_mappings.get(&event.task_id).unwrap();
                        successful += task.algorithms.len();
                        active_tasks -= 1;
                        engine.wait_for_task(event.task_id);
                    }
                    ComputeEventBody::Cancelled => {
                        active_tasks -= 1;
                        engine.wait_for_task(event.task_id);
                    }
                }
            } else {
                std::thread::sleep(std::time::Duration::from_millis(10));
            }
        }

        engine.cleanup_completed();

        Ok(RunSummary {
            total_trials: total,
            successful,
            cached: 0,
            failed,
            total_time_secs: start_time.elapsed().as_secs_f64(),
            errors,
        })
    }

    fn report_progress(&self, info: ProgressInfo) {
        if let Some(ref callback) = self.progress_callback {
            callback(info);
        }
    }
}
