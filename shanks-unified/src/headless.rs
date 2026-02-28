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
use crate::experiment::{ExperimentConfig, NoiseDef};
use crate::ffi::{ComputeEventBody, ShanksLibrary, Series};

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
    export_path: Option<std::path::PathBuf>,
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
            export_path: None,
        })
    }

    /// Set a progress callback.
    pub fn set_progress_callback<F: Fn(ProgressInfo) + Send + Sync + 'static>(&mut self, f: F) {
        self.progress_callback = Some(Box::new(f));
    }

    /// Set export path.
    pub fn set_export_path(&mut self, path: std::path::PathBuf) {
        self.export_path = Some(path);
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

        // For incremental export: task_id -> (series_result, accel_results)
        let mut task_results: std::collections::HashMap<uuid::Uuid, (
            Option<(i64, String, String, std::collections::HashMap<String, String>, crate::ffi::SeriesResult)>,
            Vec<(i64, String, i64, String, std::collections::HashMap<String, String>, crate::ffi::AccelResult)>
        )> = std::collections::HashMap::new();

        let noise_iter: Vec<Option<&NoiseDef>> = if noises.is_empty() {
            vec![None]
        } else {
            noises.iter().map(Some).collect()
        };

        let mut series_id_counter = 0;
        for precision in &self.precisions {
            for series_inst in &series {
                for noise_opt in &noise_iter {
                    series_id_counter += 1;
                    let current_series_id = series_id_counter;
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
                    let n_points = self.config.n_points.unwrap_or(33);

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
                    }

                    let task = ComputeTask {
                        id: uuid::Uuid::new_v4(),
                        series_id: current_series_id,
                        precision: precision.to_string(),
                        series: series_params.clone(),
                        n_points: n_points as u64,
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
                    ComputeEventBody::SeriesComplete { name: _, result } => {
                        if self.export_path.is_some() {
                            let task = task_mappings.get(&event.task_id).unwrap();
                            let mut args_str_map = std::collections::HashMap::new();
                            for (k, v) in &task.series.params {
                                args_str_map.insert(k.clone(), v.to_string());
                            }
                            
                            let entry = task_results.entry(event.task_id).or_insert((None, Vec::new()));
                            entry.0 = Some((task.series_id, task.series.name.clone(), task.precision.clone(), args_str_map, result));
                        }
                    }
                    ComputeEventBody::AccelComplete { name, result } => {
                        let task = task_mappings.get(&event.task_id).unwrap();
                        self.report_progress(ProgressInfo {
                            current: successful + failed,
                            total,
                            series_name: task.series.name.clone(),
                            method_name: name.clone(),
                            precision: task.precision.clone(),
                            status: Status::Complete,
                            elapsed_secs: start_time.elapsed().as_secs_f64(),
                        });

                        if self.export_path.is_some() {
                            // Stable series_id from task
                            let series_id = task.series_id;
                            
                            // Find accel name and m_value
                            // Event name is usually "Series - Accel"
                            let accel_name = name.split(" - ").last().unwrap_or(&name).to_string();

                            // Try to find m_value in task algorithms
                            let m_val = task
                                .algorithms
                                .iter()
                                .find(|a| name.contains(&a.name))
                                .and_then(|a| a.params.get("m"))
                                .and_then(|v| match v {
                                    crate::ffi::ParamValue::Int(i) => Some(*i),
                                    _ => None,
                                })
                                .unwrap_or(0);

                            let mut args_str_map = std::collections::HashMap::new();
                            if let Some(algo) =
                                task.algorithms.iter().find(|a| name.contains(&a.name))
                            {
                                for (k, v) in &algo.params {
                                    if k != "m" {
                                        args_str_map.insert(k.clone(), v.to_string());
                                    }
                                }
                            }

                            let entry = task_results.entry(event.task_id).or_insert((None, Vec::new()));
                            entry.1.push((
                                series_id,
                                accel_name,
                                m_val,
                                task.precision.clone(),
                                args_str_map,
                                result,
                            ));
                        }
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
                        
                        // Incremental export if requested
                        if let Some(ref path) = self.export_path {
                            if let Some((Some(series_res), accels)) = task_results.remove(&event.task_id) {
                                log::info!("Exporting series_id={} ({} accelerations) to {:?}", task.series_id, accels.len(), path);
                                let partition = crate::export::parquet::PartitionData {
                                    series_id: task.series_id,
                                    series_result: series_res,
                                    accel_results: accels,
                                };
                                if let Err(e) = crate::export::parquet::ParquetExporter::export_partition(partition, path) {
                                    log::error!("Failed to export partition for series_id={}: {}", task.series_id, e);
                                    errors.push(format!("Partition export failed: {}", e));
                                }
                            }
                        }

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

        // Export summary / cleanup
        if let Some(ref path) = self.export_path {
            log::info!("Incremental export finished at {:?}", path);
        }

        Ok(RunSummary {
            total_trials: total,
            successful,
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
