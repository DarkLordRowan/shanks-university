use crate::app::{AppSelection, Config, PlotCache, ResultKey};
use crate::cache::Cache;
use crate::compute::{self, AccelData, Cancellable, ComputeEvent, SeriesData};
use arc_swap::ArcSwap;
use std::collections::{HashMap, HashSet};
use std::sync::Arc;
use std::sync::atomic::{AtomicBool, Ordering};
use tokio::sync::watch;

pub struct Coordinator {
    cache: Cache,
    series_results: HashMap<compute::SeriesDesc, Option<Arc<SeriesData>>>,
    accel_results: HashMap<ResultKey, Option<Arc<AccelData>>>,
    active_tasks: HashMap<compute::SeriesDesc, Cancellable>,
    n_points_cached: u64,

    // State
    last_config: Config,
    last_selection: AppSelection,
    bake_handle: Option<Cancellable>,
    cancel_flag: Option<Arc<AtomicBool>>,

    // Outbound state pointers
    plot_cache: Arc<ArcSwap<PlotCache>>,
    data_cache: Arc<ArcSwap<crate::app::data_tab::DataCache>>,
    status_tx: watch::Sender<String>,
    last_err_tx: watch::Sender<String>,

    // Channels
    config_rx: watch::Receiver<Config>,
    combos_rx: watch::Receiver<AppSelection>,
    event_tx: tokio::sync::mpsc::Sender<ComputeEvent<compute::SeriesDesc>>,
    event_rx: tokio::sync::mpsc::Receiver<ComputeEvent<compute::SeriesDesc>>,
}

impl Coordinator {
    pub fn spawn(
        _cfg: Option<crate::experiment::ExperimentConfig>,
        cache: Cache,
        plot_cache: Arc<ArcSwap<PlotCache>>,
        data_cache: Arc<ArcSwap<crate::app::data_tab::DataCache>>,
        status_tx: watch::Sender<String>,
        last_err_tx: watch::Sender<String>,
        config_rx: watch::Receiver<Config>,
        combos_rx: watch::Receiver<AppSelection>,
    ) {
        let (event_tx, event_rx) = tokio::sync::mpsc::channel(100);

        let initial_config = config_rx.borrow().clone();
        let initial_combos = combos_rx.borrow().clone();

        let mut coord = Self {
            cache,
            series_results: HashMap::new(),
            accel_results: HashMap::new(),
            active_tasks: HashMap::new(),
            n_points_cached: initial_config.n_points + 10,
            last_config: initial_config,
            last_selection: initial_combos,
            bake_handle: None,
            cancel_flag: None,
            plot_cache,
            data_cache,
            status_tx,
            last_err_tx,
            config_rx,
            combos_rx,
            event_tx,
            event_rx,
        };

        tokio::spawn(async move {
            coord.run().await;
        });
    }

    async fn run(&mut self) {
        loop {
            tokio::select! {
                Ok(_) = self.config_rx.changed() => {
                    let cfg = self.config_rx.borrow().clone();
                    self.handle_config_update(cfg);
                }
                Ok(_) = self.combos_rx.changed() => {
                    let selection = self.combos_rx.borrow().clone();
                    self.handle_selection_update(selection);
                }
                Some(event) = self.event_rx.recv() => {
                    self.handle_compute_event(event);
                }
                else => break,
            }
        }
    }

    fn handle_config_update(&mut self, cfg: Config) {
        let points_grew = cfg.n_points > self.n_points_cached;
        self.last_config = cfg;

        if points_grew {
            self.series_results.clear();
            self.accel_results.clear();
            self.active_tasks.clear();
            self.n_points_cached = self.last_config.n_points + 10;
            self.sync_with_compute();
        }
        self.trigger_bake();
    }

    fn handle_selection_update(&mut self, selection: AppSelection) {
        self.last_selection = selection;
        self.sync_with_compute();
        self.trigger_bake();
    }

    fn handle_compute_event(&mut self, event: ComputeEvent<compute::SeriesDesc>) {
        match event {
            ComputeEvent::SeriesDone { id, data, .. } => {
                self.series_results.insert(id, Some(Arc::new(data)));
                self.trigger_bake();
            }
            ComputeEvent::AccelDone { id, desc, data, .. } => {
                let rkey = ResultKey {
                    series: id,
                    accel: Some(desc),
                };
                self.accel_results.insert(rkey, Some(Arc::new(data)));
                self.trigger_bake();
            }
            ComputeEvent::Complete(id) => {
                self.active_tasks.remove(&id);
                if self.active_tasks.is_empty() {
                    let _ = self.status_tx.send("Complete".to_string());
                }
            }
            ComputeEvent::Error { id, error } => {
                self.active_tasks.remove(&id);
                self.series_results.entry(id).or_insert(None);
                let _ = self.status_tx.send(format!("Error: {}", error));
                let _ = self.last_err_tx.send(error);
            }
        }
    }

    fn sync_with_compute(&mut self) {
        let selection = &self.last_selection;

        let mut requested_series: HashSet<compute::SeriesDesc> = HashSet::new();
        let mut requested_accels: HashSet<ResultKey> = HashSet::new();

        // series_tasks maps each SeriesDesc → (set of AccelInstances, set of FilterInstances)
        type TaskInfo = (
            HashSet<crate::experiment::AccelInstance>,
            HashSet<crate::experiment::FilterInstance>,
        );
        let mut series_tasks: HashMap<compute::SeriesDesc, TaskInfo> = HashMap::new();

        let accels = &selection.accels;
        let filters = &selection.filters;

        for prec in &selection.precisions {
            for series in &selection.series {
                for noise in &selection.noises {
                    let s_desc = compute::SeriesDesc {
                        precision: prec.clone(),
                        series: series.clone(),
                        noise: noise.clone(),
                    };
                    requested_series.insert(s_desc.clone());

                    let task_info = series_tasks.entry(s_desc.clone()).or_default();

                    for accel in accels {
                        for filter in filters {
                            let a_desc = compute::AccelDesc {
                                accel: accel.clone(),
                                filter: Some(filter.clone()),
                            };
                            let rk = ResultKey {
                                series: s_desc.clone(),
                                accel: Some(a_desc.clone()),
                            };
                            requested_accels.insert(rk);
                            task_info.0.insert(accel.clone());
                            task_info.1.insert(filter.clone());
                        }
                        // No-filter variant
                        if filters.is_empty() {
                            let a_desc = compute::AccelDesc {
                                accel: accel.clone(),
                                filter: None,
                            };
                            let rk = ResultKey {
                                series: s_desc.clone(),
                                accel: Some(a_desc),
                            };
                            requested_accels.insert(rk);
                            task_info.0.insert(accel.clone());
                        }
                    }
                }
            }
        }

        // 1. Cleanup old results and tasks
        self.active_tasks.retain(|desc, handle| {
            if !requested_series.contains(desc) {
                handle.cancel();
                false
            } else {
                true
            }
        });

        let old_series_count = self.series_results.len();
        self.series_results
            .retain(|desc, _| requested_series.contains(desc));

        let old_accel_count = self.accel_results.len();
        self.accel_results
            .retain(|rk, _| requested_accels.contains(rk));

        if self.series_results.len() != old_series_count
            || self.accel_results.len() != old_accel_count
        {
            self.trigger_bake();
        }

        // 2. Spawn new tasks
        for (s_desc, (algos, filters)) in series_tasks {
            if self.active_tasks.contains_key(&s_desc) {
                continue;
            }

            // Check if all results for this desc are already present
            let series_done = self.series_results.contains_key(&s_desc);
            let accels_done = requested_accels
                .iter()
                .filter(|rk| rk.series == s_desc)
                .all(|rk| self.accel_results.contains_key(rk));

            if series_done && accels_done {
                continue;
            }

            let task = compute::ComputeTask {
                id: s_desc.clone(),
                series: s_desc.clone(),
                n_points: self.n_points_cached,
                algorithms: algos.into_iter().collect(),
                filters: filters.into_iter().collect(),
            };

            let handle = compute::spawn_task(task, self.cache.clone(), self.event_tx.clone());
            self.active_tasks.insert(s_desc, handle);
            let _ = self.status_tx.send("Computing...".to_string());
        }
    }

    fn trigger_bake(&mut self) {
        if let Some(handle) = self.bake_handle.take() {
            handle.cancel();
        }

        let cancel_flag = Arc::new(AtomicBool::new(false));
        self.cancel_flag = Some(cancel_flag.clone());

        let series_results = self.series_results.clone();
        let accel_results = self.accel_results.clone();
        let config = self.last_config.clone();
        let plot_cache_ref = self.plot_cache.clone();
        let data_cache_ref = self.data_cache.clone();

        self.bake_handle = Some(Cancellable::new(|is_cancelled| {
            tokio::task::spawn_blocking(move || {
                let new_cache = crate::app::ShanksApp::bake_plot_cache_task(
                    &series_results,
                    &accel_results,
                    &config.main_tab_state,
                    &config.dev_tab_state,
                    config.deviation_mode,
                    is_cancelled.clone(),
                );

                if is_cancelled.cancelled() {
                    return;
                }
                plot_cache_ref.store(Arc::new(new_cache));

                if config.upd_data {
                    let mut new_data_cache = crate::app::data_tab::DataCache {
                        dirty: true,
                        ..Default::default()
                    };
                    new_data_cache.rebuild(&series_results, &accel_results, cancel_flag.clone());
                    if cancel_flag.load(Ordering::Relaxed) {
                        return;
                    }
                    data_cache_ref.store(Arc::new(new_data_cache));
                }
            })
        }));
    }
}
