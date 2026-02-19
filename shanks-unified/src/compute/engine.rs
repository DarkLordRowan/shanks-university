//! Compute engine for task execution using standard threads.

use std::collections::HashSet;
use std::sync::Arc;
use std::sync::RwLock;
use std::sync::mpsc as std_mpsc;
use std::thread::{self, JoinHandle};
use uuid::Uuid;

use crate::cache::Cache;
use crate::ffi::{ShanksLibrary, SeriesResult, AccelResult, ComputeEvent, ComputeEventBody};
use super::task::ComputeTask;

/// Compute engine for running series generation and acceleration.
pub struct ComputeEngine {
    /// FFI library handle
    library: Arc<ShanksLibrary>,
    /// SQLite cache
    cache: Arc<RwLock<Cache>>,
    /// Running tasks
    tasks: std::collections::HashMap<Uuid, JoinHandle<()>>,
    /// Cancel flags for tasks
    cancel_flags: Arc<RwLock<HashSet<Uuid>>>,
    /// Event sender
    event_tx: std_mpsc::Sender<ComputeEvent>,
}

impl ComputeEngine {
    /// Create a new compute engine.
    pub fn new(
        library: Arc<ShanksLibrary>,
        cache: Arc<RwLock<Cache>>,
        event_tx: std_mpsc::Sender<ComputeEvent>,
    ) -> Self {
        Self {
            library,
            cache,
            tasks: std::collections::HashMap::new(),
            cancel_flags: Arc::new(RwLock::new(HashSet::new())),
            event_tx,
        }
    }

    /// Start a computation task.
    pub fn start_task(&mut self, task: ComputeTask) -> Result<Uuid, String> {
        let task_id = task.id;
        let library = self.library.clone();
        let _cache = self.cache.clone();
        let cancel_flags = self.cancel_flags.clone();
        let event_tx = self.event_tx.clone();

        log::info!("Starting task {} for series '{}' with precision {}", 
            task_id, task.series.name, task.precision);

        let handle = thread::spawn(move || {
            // Send started event
            let _ = event_tx.send(ComputeEvent {
                task_id,
                body: ComputeEventBody::Started,
            });

            // Check cancellation
            if cancel_flags.read().unwrap().contains(&task_id) {
                let _ = event_tx.send(ComputeEvent {
                    task_id,
                    body: ComputeEventBody::Cancelled,
                });
                return;
            }

            // Create series
            let series_params_json = serde_json::to_string(&task.series.params).unwrap_or_else(|_| "{}".to_string());
            log::info!("Creating series '{}' with precision '{}', x='{}', n_points={}, params='{}'",
                task.series.name, task.precision, task.series.x_value, task.n_points, series_params_json);
            
            let series_handle = match library.series_create(
                &task.series.name,
                &task.precision,
                &task.series.x_value,
                &series_params_json,
            ) {
                Ok(h) => {
                    log::info!("Series created successfully");
                    h
                }
                Err(e) => {
                    log::error!("Failed to create series: {}", e);
                    let _ = event_tx.send(ComputeEvent {
                        task_id,
                        body: ComputeEventBody::Error { error: e.to_string() },
                    });
                    return;
                }
            };

            // Check cancellation
            if cancel_flags.read().unwrap().contains(&task_id) {
                library.series_destroy(series_handle);
                let _ = event_tx.send(ComputeEvent {
                    task_id,
                    body: ComputeEventBody::Cancelled,
                });
                return;
            }

            // Generate series
            let series_result = match library.series_generate(&series_handle, task.n_points, false) {
                Ok(r) => {
                    log::info!("Series generated: {} bytes", r.len());
                    r
                }
                Err(e) => {
                    log::error!("Failed to generate series: {}", e);
                    library.series_destroy(series_handle);
                    let _ = event_tx.send(ComputeEvent {
                        task_id,
                        body: ComputeEventBody::Error { error: e.to_string() },
                    });
                    return;
                }
            };

            // Parse series result
            log::info!("Series JSON (first 500 chars): {}", &series_result[..series_result.len().min(500)]);
            let parsed_result: SeriesResult = match serde_json::from_str::<SeriesResult>(&series_result) {
                Ok(r) => {
                    log::info!("Parsed {} Sn points", r.sn.len());
                    r
                }
                Err(e) => {
                    log::error!("Failed to parse series JSON: {}", e);
                    library.series_destroy(series_handle);
                    let _ = event_tx.send(ComputeEvent {
                        task_id,
                        body: ComputeEventBody::Error { error: format!("JSON parse error: {}", e) },
                    });
                    return;
                }
            };

            let _ = event_tx.send(ComputeEvent {
                task_id,
                body: ComputeEventBody::SeriesComplete { result: parsed_result },
            });

            // Apply each algorithm
            log::info!("Applying {} algorithms", task.algorithms.len());
            for accel in &task.algorithms {
                log::info!("Creating algorithm '{}' with precision '{}'", accel.name, task.precision);
                // Check cancellation
                if cancel_flags.read().unwrap().contains(&task_id) {
                    library.series_destroy(series_handle);
                    let _ = event_tx.send(ComputeEvent {
                        task_id,
                        body: ComputeEventBody::Cancelled,
                    });
                    return;
                }

                let accel_params_json = serde_json::to_string(&accel.params).unwrap_or_else(|_| "{}".to_string());
                
                let accel_handle = match library.accel_create(
                    &accel.name,
                    &task.precision,
                    &accel_params_json,
                ) {
                    Ok(h) => h,
                    Err(e) => {
                        let _ = event_tx.send(ComputeEvent {
                            task_id,
                            body: ComputeEventBody::Error { error: format!("Failed to create algorithm '{}': {}", accel.name, e) },
                        });
                        continue;
                    }
                };

                // Apply acceleration (use n_points and default order of 5)
                let accel_result = match library.accel_apply(&accel_handle, &series_handle, task.n_points, 5) {
                    Ok(r) => r,
                    Err(e) => {
                        library.accel_destroy(accel_handle);
                        let _ = event_tx.send(ComputeEvent {
                            task_id,
                            body: ComputeEventBody::Error { error: format!("Failed to apply algorithm '{}': {}", accel.name, e) },
                        });
                        continue;
                    }
                };

                // Parse acceleration result
                let parsed_accel: AccelResult = match serde_json::from_str::<AccelResult>(&accel_result) {
                    Ok(r) => r,
                    Err(e) => {
                        library.accel_destroy(accel_handle);
                        let _ = event_tx.send(ComputeEvent {
                            task_id,
                            body: ComputeEventBody::Error { error: format!("JSON parse error for algorithm result: {}", e) },
                        });
                        continue;
                    }
                };

                let _ = event_tx.send(ComputeEvent {
                    task_id,
                    body: ComputeEventBody::AccelComplete { 
                        name: build_distinct_name(accel, &task.algorithms),
                        result: parsed_accel,
                    },
                });

                library.accel_destroy(accel_handle);
            }

            // Cleanup
            library.series_destroy(series_handle);

            // Send completion event
            let _ = event_tx.send(ComputeEvent {
                task_id,
                body: ComputeEventBody::Complete,
            });

            // Cache the result
            // TODO: Implement caching
        });

        self.tasks.insert(task_id, handle);
        Ok(task_id)
    }

    /// Cancel a running task.
    pub fn cancel_task(&mut self, task_id: Uuid) {
        self.cancel_flags.write().unwrap().insert(task_id);
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

fn build_distinct_name(accel: &crate::compute::task::AccelParams, all_accels: &[crate::compute::task::AccelParams]) -> String {
    let same_name_accels: Vec<_> = all_accels.iter().filter(|a| a.name == accel.name).collect();
    if same_name_accels.len() <= 1 {
        // Only one of this name, so just use parameters directly if we want, or just the name. 
        // Wait, the user still wants to see parameters if they added specific params. Maybe always show params if they exist?
        // "skip the common parts" heavily implies comparing multiple elements.
        // For simplicity, let's just return the name if alone? Or return name + all params?
        // Let's return just the name if there's only one.
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
