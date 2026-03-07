//! Headless runner — executes batch computations using the async compute pipeline.

use crate::cache::Cache;
use crate::compute::{self, ComputeEvent, ComputeTask, SeriesDesc};
use crate::experiment::{
    AccelInstance, ExperimentConfig, FilterInstance, NoiseInstance, SeriesInstance,
};
use crate::export::parquet::{AccelExportRow, ExportData, ParquetExporter, SeriesExportRow};
use anyhow::Result;
use std::collections::HashMap;
use std::path::PathBuf;
use tokio::sync::mpsc;

/// Status reported during headless computation.
#[derive(Debug, Clone)]
pub enum Status {
    Computing,
    Complete,
    Error(String),
}

/// Progress info passed to the progress callback.
#[derive(Debug, Clone)]
pub struct ProgressInfo {
    pub current: usize,
    pub total: usize,
    pub series_name: String,
    pub precision: String,
    pub method_name: String,
    pub elapsed_secs: f64,
    pub status: Status,
}

/// Summary returned by run_all.
#[derive(Debug, Clone, Default)]
pub struct RunSummary {
    pub total_trials: usize,
    pub successful: usize,
    pub failed: usize,
    pub total_time_secs: f64,
    pub errors: Vec<String>,
}

/// Headless batch runner.
pub struct HeadlessRunner {
    pub config: ExperimentConfig,
    pub cache: Cache,
    pub export: Option<PathBuf>,
    pub progress: Option<Box<dyn FnMut(ProgressInfo) + Send>>,
}

impl HeadlessRunner {
    pub fn new(config: ExperimentConfig, cache: Cache, export: Option<PathBuf>) -> Result<Self> {
        let export = export.map(|p| {
            let now = chrono::Local::now();
            let suffix = now.format("%Y-%m-%d_%H-%M-%S").to_string();
            let mut name = p.file_name().unwrap_or_default().to_os_string();
            name.push("_");
            name.push(suffix);
            p.with_file_name(name)
        });

        Ok(Self {
            config,
            cache,
            export,
            progress: None,
        })
    }

    pub fn with_progress<F>(mut self, cb: F) -> Self
    where
        F: FnMut(ProgressInfo) + Send + 'static,
    {
        self.progress = Some(Box::new(cb));
        self
    }
    /// Run all tasks defined in the config.
    pub async fn run_all(&mut self) -> Result<RunSummary> {
        let start_time = std::time::Instant::now();

        // 1. Expand tasks
        let tasks = self.expand_tasks()?;
        let total_tasks = tasks.len();
        if total_tasks == 0 {
            return Ok(RunSummary::default());
        }

        let mut summary = RunSummary::default();
        summary.total_trials = total_tasks;

        let mut series_results = Vec::new();

        let (tx, mut rx) = mpsc::channel(32);

        let mut pending_accels: HashMap<usize, Vec<AccelExportRow>> = HashMap::new();

        for task in tasks {
            compute::spawn_task(task, self.cache.clone(), tx.clone());
        }
        drop(tx); // Close the sender so rx ends when all tasks finish

        let mut finished_tasks = 0;
        while let Some(event) = rx.recv().await {
            match event {
                ComputeEvent::SeriesDone { id, data } => {
                    let mut arguments = HashMap::new();
                    // Explicitly add 'x' because Python previously expected it inside args
                    arguments.insert("x".to_string(), id.1.series.x.to_string());
                    for (k, v) in &id.1.series.args {
                        let vs = match v {
                            serde_json::Value::String(s) => s.clone(),
                            _ => v.to_string(),
                        };
                        arguments.insert(k.clone(), vs);
                    }
                    
                    if let Some(ref noise) = id.1.noise {
                        arguments.insert("noise_type".to_string(), noise.noise_type.clone());
                        arguments.insert("noise_method".to_string(), noise.method.clone());
                        arguments.insert("noise_seed".to_string(), noise.seed.to_string());
                        
                        for (k, v) in &noise.args {
                            let vs = match v {
                                serde_json::Value::String(s) => s.clone(),
                                _ => v.to_string(),
                            };
                            if vs != "null" {
                                arguments.insert(format!("noise_{}", k), vs);
                            }
                        }
                    }

                    series_results.push(SeriesExportRow {
                        series_id: id.0 as i64,
                        series_name: id.1.series.name.clone(),
                        precision: id.1.precision.clone(),
                        arguments,
                        data,
                    });

                    if let Some(ref mut cb) = self.progress {
                        cb(ProgressInfo {
                            current: finished_tasks,
                            total: total_tasks,
                            series_name: id.1.series.name.clone(),
                            precision: id.1.precision.clone(),
                            method_name: "none".to_string(),
                            elapsed_secs: start_time.elapsed().as_secs_f64(),
                            status: Status::Computing,
                        });
                    }
                }
                ComputeEvent::AccelDone { id, desc, data } => {
                    let mut arguments = HashMap::new();
                    for (k, v) in &desc.accel.args {
                        let vs = match v {
                            serde_json::Value::String(s) => s.clone(),
                            _ => v.to_string(),
                        };
                        arguments.insert(k.clone(), vs);
                    }

                    let entry = pending_accels.entry(id.0).or_insert_with(Vec::new);
                    entry.push(AccelExportRow {
                        series_id: id.0 as i64,
                        m_value: desc.accel.m,
                        accel_name: desc.accel.name.clone(),
                        arguments,
                        data: data.clone(), // we need to clone Arc inside or clone row if needed
                    });

                    if let Some(ref mut cb) = self.progress {
                        cb(ProgressInfo {
                            current: finished_tasks,
                            total: total_tasks,
                            series_name: id.1.series.name.clone(),
                            precision: id.1.precision.clone(),
                            method_name: desc.accel.name.clone(),
                            elapsed_secs: start_time.elapsed().as_secs_f64(),
                            status: Status::Computing,
                        });
                    }
                }
                ComputeEvent::Complete(id) => {
                    let task_idx = id.0;
                    if let Some(ref path) = self.export {
                        if let Some(task_accels) = pending_accels.remove(&task_idx) {
                            if !task_accels.is_empty() {
                                if let Err(e) = ParquetExporter::export_incremental_accel_batch(task_idx as i64, &task_accels, path) {
                                    log::error!("Failed to export incremental accels for task {}: {}", task_idx, e);
                                }
                            }
                        }
                    }
                    finished_tasks += 1;
                    summary.successful += 1;
                }
                ComputeEvent::Error { error, .. } => {
                    finished_tasks += 1;
                    summary.failed += 1;
                    summary.errors.push(error);
                }
            }
        }

        if let Some(ref path) = self.export {
            ParquetExporter::export(
                ExportData {
                    series_results,
                },
                path,
            )?;
        }

        summary.total_time_secs = start_time.elapsed().as_secs_f64();

        Ok(summary)
    }

    /// Expand ExperimentConfig into individual ComputeTasks.
    fn expand_tasks(&self) -> Result<Vec<ComputeTask<(usize, SeriesDesc)>>> {
        let mut tasks = Vec::new();

        let n_points = self.config.n_points.unwrap_or(100);
        let precisions = self
            .config
            .precisions
            .clone()
            .unwrap_or_else(|| vec!["F64".to_string()]);

        // Expand all components into vectors first
        let series_instances: Vec<SeriesInstance> =
            self.config.series.iter().flat_map(|s| s.expand()).collect();

        let noise_instances: Vec<NoiseInstance> =
            self.config.noises.iter().flat_map(|n| n.expand()).collect();

        let accel_instances: Vec<AccelInstance> =
            self.config.accels.iter().flat_map(|m| m.expand()).collect();

        let filter_instances: Vec<FilterInstance> = self
            .config
            .filters
            .iter()
            .flat_map(|f| f.expand())
            .collect();

        // Cross-product expansion
        let mut task_id = 0;
        for precision in &precisions {
            for series in &series_instances {
                // Option 1: No noise
                let desc = SeriesDesc {
                    precision: precision.clone(),
                    series: series.clone(),
                    noise: None,
                };
                tasks.push(ComputeTask {
                    id: (task_id, desc.clone()),
                    series: desc,
                    n_points,
                    algorithms: accel_instances.clone(),
                    filters: filter_instances.clone(),
                });
                task_id += 1;

                // Option 2: All noises
                for noise in &noise_instances {
                    let desc = SeriesDesc {
                        precision: precision.clone(),
                        series: series.clone(),
                        noise: Some(noise.clone()),
                    };
                    tasks.push(ComputeTask {
                        id: (task_id, desc.clone()),
                        series: desc,
                        n_points,
                        algorithms: accel_instances.clone(),
                        filters: filter_instances.clone(),
                    });
                    task_id += 1;
                }
            }
        }

        Ok(tasks)
    }
}
