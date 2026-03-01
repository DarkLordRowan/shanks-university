//! Headless runner — being rewritten to use async compute pipeline.
//! This file is a transitional stub preserving the public API.

use anyhow::Result;
use std::path::PathBuf;
use std::sync::Arc;
use crate::cache::Cache;
use crate::experiment::ExperimentConfig;
use crate::ffi::ShanksLibrary;

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
    pub current:      usize,
    pub total:        usize,
    pub series_name:  String,
    pub precision:    String,
    pub method_name:  String,
    pub elapsed_secs: f64,
    pub status:       Status,
}

/// Summary returned by run_all.
#[derive(Debug, Clone, Default)]
pub struct RunSummary {
    pub total_trials:   usize,
    pub successful:     usize,
    pub failed:         usize,
    pub total_time_secs: f64,
    pub errors:         Vec<String>,
}

/// Headless batch runner (transitional stub).
pub struct HeadlessRunner {
    #[allow(dead_code)]
    config:    ExperimentConfig,
    #[allow(dead_code)]
    library:   Arc<ShanksLibrary>,
    #[allow(dead_code)]
    cache:     Cache,
    #[allow(dead_code)]
    export:    Option<PathBuf>,
    #[allow(dead_code)]
    progress:  Option<Box<dyn FnMut(ProgressInfo) + Send>>,
}

impl HeadlessRunner {
    pub fn new(
        config:  ExperimentConfig,
        library: Arc<ShanksLibrary>,
        cache:   Cache,
    ) -> Result<Self> {
        Ok(Self { config, library, cache, export: None, progress: None })
    }

    pub fn set_export_path(&mut self, path: PathBuf) {
        self.export = Some(path);
    }

    pub fn set_progress_callback<F: FnMut(ProgressInfo) + Send + 'static>(&mut self, cb: F) {
        self.progress = Some(Box::new(cb));
    }

    /// Run all tasks.  TODO: implement with compute::spawn_task.
    pub fn run_all(&mut self) -> Result<RunSummary> {
        Ok(RunSummary::default())
    }
}
