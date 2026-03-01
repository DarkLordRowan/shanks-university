//! Headless runner — executes batch computations using the async compute pipeline.

use crate::cache::Cache;
use crate::compute::{self, ComputeEvent, ComputeTask};
use crate::experiment::{
    AccelInstance, ExperimentConfig, FilterInstance, NoiseInstance, SeriesInstance,
};
use anyhow::Result;
use std::path::PathBuf;
use std::sync::Arc;
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
    pub fn new(
        config: ExperimentConfig,
        cache: Cache,
        export: Option<PathBuf>,
    ) -> Result<Self> {
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
    pub fn run_all(&mut self) -> Result<RunSummary> {
        let start_time = std::time::Instant::now();

        // 1. Expand tasks
        let tasks = self.expand_tasks()?;
        let total_tasks = tasks.len();
        if total_tasks == 0 {
            return Ok(RunSummary::default());
        }

        // 2. Create runtime
        let rt = tokio::runtime::Runtime::new()?;

        // 3. Run tasks
        let summary = rt.block_on(async {
            let mut summary = RunSummary::default();
            summary.total_trials = total_tasks;

            let (tx, mut rx) = mpsc::channel(32);

            for task in tasks {
                compute::spawn_task(task, self.cache.clone(), tx.clone());
            }
            drop(tx); // Close the sender so rx ends when all tasks finish

            let mut finished_tasks = 0;
            while let Some(event) = rx.recv().await {
                match event {
                    ComputeEvent::SeriesDone { series, accel, .. } => {
                        let method = accel
                            .as_ref()
                            .map(|(d, _)| d.accel.name.clone())
                            .unwrap_or_else(|| "none".to_string());

                        if let Some(ref mut cb) = self.progress {
                            cb(ProgressInfo {
                                current: finished_tasks,
                                total: total_tasks,
                                series_name: series.series.name.clone(),
                                precision: series.precision.clone(),
                                method_name: method,
                                elapsed_secs: start_time.elapsed().as_secs_f64(),
                                status: Status::Computing,
                            });
                        }
                    }
                    ComputeEvent::Complete(_) => {
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

            summary.total_time_secs = start_time.elapsed().as_secs_f64();
            summary
        });

        Ok(summary)
    }

    /// Expand ExperimentConfig into individual ComputeTasks.
    fn expand_tasks(&self) -> Result<Vec<ComputeTask<usize>>> {
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
                tasks.push(ComputeTask {
                    id: task_id,
                    precision: precision.clone(),
                    series: series.clone(),
                    n_points,
                    noise: None,
                    algorithms: accel_instances.clone(),
                    filters: filter_instances.clone(),
                });
                task_id += 1;

                // Option 2: All noises
                for noise in &noise_instances {
                    tasks.push(ComputeTask {
                        id: task_id,
                        precision: precision.clone(),
                        series: series.clone(),
                        n_points,
                        noise: Some(noise.clone()),
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
