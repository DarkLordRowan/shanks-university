//! Compute engine for task execution using standard threads.

use std::collections::HashSet;
use std::sync::mpsc as std_mpsc;
use std::sync::Arc;
use std::sync::Mutex;
use std::thread::{self, JoinHandle};
use uuid::Uuid;

use super::core::ComputeCore;
use super::task::ComputeTask;
use crate::cache::Cache;
use crate::ffi::{ComputeEvent, ComputeEventBody, ShanksLibrary};

/// Compute engine for running series generation and acceleration.
pub struct ComputeEngine {
    /// FFI library handle
    library: Arc<ShanksLibrary>,
    /// SQLite cache
    cache: Arc<Mutex<Cache>>,
    /// Running tasks
    tasks: std::collections::HashMap<Uuid, JoinHandle<()>>,
    /// Cancel flags for tasks
    cancel_flags: Arc<Mutex<HashSet<Uuid>>>,
    /// Event sender
    event_tx: std_mpsc::Sender<ComputeEvent>,
}

impl ComputeEngine {
    /// Create a new compute engine.
    pub fn new(
        library: Arc<ShanksLibrary>,
        cache: Arc<Mutex<Cache>>,
        event_tx: std_mpsc::Sender<ComputeEvent>,
    ) -> Self {
        Self {
            library,
            cache,
            tasks: std::collections::HashMap::new(),
            cancel_flags: Arc::new(Mutex::new(HashSet::new())),
            event_tx,
        }
    }

    /// Start a computation task.
    pub fn start_task(&mut self, task: ComputeTask) -> Result<Uuid, String> {
        let task_id = task.id;
        let library = self.library.clone();
        let cache = self.cache.clone();
        let cancel_flags = self.cancel_flags.clone();
        let event_tx = self.event_tx.clone();

        log::info!(
            "Starting task {} for series '{}' with precision {}",
            task_id,
            task.series.name,
            task.precision
        );

        let core = ComputeCore::new(library, cache);

        let handle = thread::spawn(move || {
            // Send started event
            let _ = event_tx.send(ComputeEvent {
                task_id,
                body: ComputeEventBody::Started,
            });

            // ComputeCore handles EVERYTHING: cache check, FFI calls, event emission.
            // Events emitted by core: SeriesComplete, AccelComplete, Complete (or Error/Cancelled).
            if let Err(e) = core.execute_and_cache(
                Some(task_id),
                &task.precision,
                &task.series,
                &task.algorithms,
                task.n_points,
                task.noise.as_ref(),
                &task.filters,
                Some(event_tx.clone()),
                Some(cancel_flags.clone()),
            ) {
                log::error!("Core execution failed: {}", e);
                let _ = event_tx.send(ComputeEvent {
                    task_id,
                    body: ComputeEventBody::Error {
                        error: e.to_string(),
                    },
                });
            }
        });

        self.tasks.insert(task_id, handle);
        Ok(task_id)
    }

    /// Cancel a running task.
    pub fn cancel_task(&mut self, task_id: Uuid) {
        self.cancel_flags.lock().unwrap().insert(task_id);
    }

    /// Check if a task is running.
    pub fn is_running(&self, task_id: Uuid) -> bool {
        self.tasks.contains_key(&task_id)
    }

    /// Wait for a task to complete.
    pub fn wait_for_task(&mut self, task_id: Uuid) {
        if let Some(handle) = self.tasks.remove(&task_id) {
            let _ = handle.join();
        }
    }

    /// Cleanup completed tasks.
    pub fn cleanup_completed(&mut self) {
        let mut completed = Vec::new();
        for (id, handle) in &self.tasks {
            if handle.is_finished() {
                completed.push(*id);
            }
        }
        for id in completed {
            if let Some(handle) = self.tasks.remove(&id) {
                let _ = handle.join();
            }
        }
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn test_compute_task_builder() {
        use crate::compute::task::ComputeTask;
        use crate::ffi::ParamValue;

        let task = ComputeTask::new("Ln2Series", 100)
            .with_precision("F64")
            .with_x("1.0")
            .with_param("alpha", ParamValue::Float(2.0))
            .with_algorithm("Shanks Transformation");

        assert_eq!(task.series.name, "Ln2Series");
        assert_eq!(task.n_points, 100);
        assert_eq!(task.precision, "F64");
        assert_eq!(task.algorithms.len(), 1);
    }
}
