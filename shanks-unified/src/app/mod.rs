//! Application state and UI implementation.

mod selection;
mod tree_ui;
mod ui;

use crate::cache::Cache;
use crate::compute::ComputeEngine;
use crate::config::{AppConfig, ExperimentConfig, MethodInstance, NoiseDef, SeriesInstance};
use crate::ffi::{
    AccelResult, ComputeEvent, ComputeEventBody, SeriesPoint, SeriesResult, ShanksLibrary,
};
use std::collections::HashMap;
use std::sync::{mpsc, Arc, Mutex, RwLock};

/// Parse a string value into the appropriate ParamValue type.
/// Uses the same precedence as the C++ backend: Int → Float → Bool → String.
fn parse_param_value(v: &str) -> crate::ffi::ParamValue {
    if let Ok(i) = v.parse::<i64>() {
        crate::ffi::ParamValue::Int(i)
    } else if let Ok(f) = v.parse::<f64>() {
        crate::ffi::ParamValue::Float(f)
    } else if let Ok(b) = v.parse::<bool>() {
        crate::ffi::ParamValue::Bool(b)
    } else {
        crate::ffi::ParamValue::String(v.to_string())
    }
}

pub use selection::{SelectedCombination, SelectionNode, SelectionState};

#[derive(Clone, PartialEq)]
struct ComputeInputState {
    series_tree: Option<SelectionNode>,
    accel_tree: Option<SelectionNode>,
    noise_tree: Option<SelectionNode>,
    precision_tree: Option<SelectionNode>,
}

/// Application state shared across the UI.
pub struct AppState {
    /// Application configuration
    pub config: Arc<RwLock<AppConfig>>,
    /// Experiment configuration (from config file)
    pub experiment: Option<Arc<RwLock<ExperimentConfig>>>,
    /// Database cache
    pub cache: Arc<Mutex<Cache>>,
    /// C++ library
    pub library: Option<Arc<ShanksLibrary>>,
    /// Available series names (from library)
    pub series_names: RwLock<Vec<String>>,
    /// Available acceleration algorithms
    pub accel_names: RwLock<Vec<String>>,
    /// Available precision types
    pub precision_names: RwLock<Vec<String>>,
    /// Available noise types
    pub noise_names: RwLock<Vec<String>>,
    /// Available noise methods
    pub noise_methods: RwLock<Vec<String>>,
}

impl AppState {
    /// Create new application state.
    pub fn new(
        config: AppConfig,
        experiment: Option<ExperimentConfig>,
        cache: Cache,
        library: Option<Arc<ShanksLibrary>>,
    ) -> Self {
        // Initialize lists from library or defaults
        let (series_names, accel_names, precision_names, noise_names, noise_methods) =
            if let Some(ref lib) = library {
                let series = lib.list_series().unwrap_or_default();
                let accels = lib.list_accels().unwrap_or_default();
                let precisions = lib.list_precisions().unwrap_or_default();
                let noises = lib.list_noises().unwrap_or_default();
                let methods = lib.list_noise_methods().unwrap_or_default();
                (series, accels, precisions, noises, methods)
            } else {
                // Default precision types
                let precisions = vec![
                    "F32".to_string(),
                    "F64".to_string(),
                    "FLong".to_string(),
                    "Arb".to_string(),
                    "CF32".to_string(),
                    "CF64".to_string(),
                    "CFLong".to_string(),
                    "CArb".to_string(),
                ];
                (vec![], vec![], precisions, vec![], vec![])
            };

        Self {
            config: Arc::new(RwLock::new(config)),
            experiment: experiment.map(|e| Arc::new(RwLock::new(e))),
            cache: Arc::new(Mutex::new(cache)),
            library,
            series_names: RwLock::new(series_names),
            accel_names: RwLock::new(accel_names),
            precision_names: RwLock::new(precision_names),
            noise_names: RwLock::new(noise_names),
            noise_methods: RwLock::new(noise_methods),
        }
    }

    /// Check if we're in offline mode (no library loaded).
    pub fn is_offline(&self) -> bool {
        self.library.is_none()
    }

    /// Get cache statistics.
    pub fn cache_stats(&self) -> crate::cache::CacheStats {
        self.cache
            .lock()
            .unwrap()
            .stats()
            .unwrap_or(crate::cache::CacheStats {
                series_count: 0,
                accel_count: 0,
                points_count: 0,
            })
    }

    /// Get expanded series instances from experiment config.
    pub fn get_series_instances(&self) -> Vec<SeriesInstance> {
        if let Some(ref exp) = self.experiment {
            exp.read().unwrap().expand_series()
        } else {
            vec![]
        }
    }

    /// Get expanded method instances from experiment config.
    pub fn get_method_instances(&self) -> Vec<MethodInstance> {
        if let Some(ref exp) = self.experiment {
            exp.read().unwrap().expand_methods()
        } else {
            vec![]
        }
    }

    /// Get noise definitions from experiment config.
    pub fn get_noises(&self) -> Vec<NoiseDef> {
        if let Some(ref exp) = self.experiment {
            exp.read().unwrap().noises.clone()
        } else {
            vec![]
        }
    }

    /// Get precisions from experiment config or defaults.
    pub fn get_precisions(&self) -> Vec<String> {
        if let Some(ref exp) = self.experiment {
            exp.read().unwrap().get_precisions()
        } else {
            self.precision_names.read().unwrap().clone()
        }
    }
}

#[derive(Clone)]
struct CachedPlotLine {
    name: String,
    color: egui::Color32,
    points: Vec<egui_plot::PlotPoint>,
}

#[derive(Clone)]
struct CachedIntervalData {
    name: String,
    fill_color: egui::Color32,
    points: Vec<egui_plot::PlotPoint>,
}

#[derive(Default)]
struct PlotCache {
    symlog: bool,
    log_linthresh: f64,
    main_sn: Vec<CachedPlotLine>,
    main_accel: Vec<CachedPlotLine>,
    limits: Vec<(String, f64)>,
    max_n: f64,
    deviations: Vec<CachedPlotLine>,
    intervals: Vec<CachedIntervalData>,
    prof_add: Vec<CachedPlotLine>,
    prof_mul: Vec<CachedPlotLine>,
    prof_div: Vec<CachedPlotLine>,
    prof_special: Vec<CachedPlotLine>,
}

/// Main application struct for egui.
pub struct ShanksApp {
    state: AppState,

    // Compute engine
    compute_engine: Option<ComputeEngine>,
    event_rx: Option<mpsc::Receiver<ComputeEvent>>,
    current_task_id: Option<uuid::Uuid>,

    // Selection trees
    series_tree: Option<SelectionNode>,
    accel_tree: Option<SelectionNode>,
    noise_tree: Option<SelectionNode>,
    precision_tree: Option<SelectionNode>,

    // Results
    current_results: std::collections::BTreeMap<String, SeriesResult>,
    current_accel_results: std::collections::BTreeMap<String, AccelResult>,

    // Debounce and state
    last_input_change: Option<std::time::Instant>,
    last_computed_state: Option<ComputeInputState>,

    // UI options
    show_partial_sums: bool,
    show_accel_values: bool,
    show_limit_lines: bool,
    show_interval_sup: bool,
    show_interval_inf: bool,
    show_interval_shade: bool,

    // Tab selection
    selected_tab: PlotTab,
    tab_configs: HashMap<PlotTab, TabConfig>,

    // Status
    status_message: String,
    is_computing: bool,
    results_dirty: bool,
    plot_cache: PlotCache,

    // Profiling plot options
    prof_show_add: bool,
    prof_show_mul: bool,
    prof_show_div: bool,
    prof_show_special: bool,
}

#[derive(Clone)]
struct TabConfig {
    symlog: bool,
    log_linthresh: f64,
    reset_plot: bool,
    enable_aspect_ratio: bool,
    aspect_x: String,
    aspect_y: String,
}

impl TabConfig {
    fn new(symlog: bool, log_linthresh: f64) -> Self {
        Self {
            symlog,
            log_linthresh,
            reset_plot: false,
            enable_aspect_ratio: true,
            aspect_x: "10.0".to_string(),
            aspect_y: "1.0".to_string(),
        }
    }
}

#[derive(Debug, PartialEq, Eq, Hash, Copy, Clone)]
enum PlotTab {
    Main,
    Deviation,
    Profiling,
}

impl ShanksApp {
    /// Create a new application instance.
    pub fn new(state: AppState) -> Self {
        let config = state.config.read().unwrap();
        let show_partial_sums = config.ui.show_partial_sums;
        let use_symlog = config.ui.use_symlog;
        drop(config);

        // Build selection trees if experiment config is loaded
        let series_instances = state.get_series_instances();
        let method_instances = state.get_method_instances();
        let noises = state.get_noises();
        let precisions = state.get_precisions();

        let series_tree = if !series_instances.is_empty() {
            Some(selection::build_series_tree(&series_instances))
        } else {
            None
        };

        let accel_tree = if !method_instances.is_empty() {
            Some(selection::build_accel_tree(&method_instances))
        } else {
            None
        };

        let noise_tree = if !noises.is_empty() {
            Some(selection::build_noise_tree(&noises))
        } else {
            None
        };

        let precision_tree = if !precisions.is_empty() {
            Some(selection::build_precision_tree(&precisions))
        } else {
            None
        };

        // Create compute engine if library is available
        let (compute_engine, event_rx) = if let Some(ref lib) = state.library {
            let cache = state.cache.clone();
            let (tx, rx) = mpsc::channel();
            let engine = ComputeEngine::new(lib.clone(), cache, tx);
            (Some(engine), Some(rx))
        } else {
            (None, None)
        };

        let mut tab_configs = HashMap::new();
        tab_configs.insert(PlotTab::Main, TabConfig::new(use_symlog, -50.0));
        tab_configs.insert(PlotTab::Deviation, TabConfig::new(use_symlog, -50.0));
        tab_configs.insert(PlotTab::Profiling, TabConfig::new(false, 0.0));

        Self {
            state,
            compute_engine,
            event_rx,
            current_task_id: None,
            series_tree,
            accel_tree,
            noise_tree,
            precision_tree,
            current_results: std::collections::BTreeMap::new(),
            current_accel_results: std::collections::BTreeMap::new(),
            last_input_change: None,
            last_computed_state: None,
            show_partial_sums,
            show_accel_values: true,
            show_limit_lines: true,
            show_interval_sup: true,
            show_interval_inf: true,
            show_interval_shade: true,
            selected_tab: PlotTab::Main,
            tab_configs,
            status_message: String::new(),
            is_computing: false,
            results_dirty: false,
            plot_cache: PlotCache::default(),
            prof_show_add: true,
            prof_show_mul: true,
            prof_show_div: true,
            prof_show_special: true,
        }
    }

    /// Synchronize the requested combination with the cache and start compute if necessary.
    /// Build compute tasks from current selection and submit them to the engine.
    /// Results (series + accel) arrive purely via ComputeEngine events.
    /// No direct cache access happens here.
    fn sync_with_cache_and_compute(&mut self) {
        let series_tree = match &self.series_tree {
            Some(t) => t,
            None => return,
        };
        let accel_tree = match &self.accel_tree {
            Some(t) => t,
            None => return,
        };
        let noise_tree = self.noise_tree.as_ref();
        let precision_tree = self.precision_tree.as_ref();

        let combinations = selection::generate_combinations(
            series_tree,
            accel_tree,
            noise_tree.unwrap_or(&SelectionNode::new("empty", "Empty")),
            precision_tree.unwrap_or(&SelectionNode::new("empty", "Empty")),
        );

        if combinations.is_empty() {
            self.current_results.clear();
            self.current_accel_results.clear();
            return;
        }

        // Cancel previous task if still running
        if let (Some(ref mut engine), Some(id)) = (&mut self.compute_engine, self.current_task_id) {
            if self.is_computing {
                engine.cancel_task(id);
            }
        }

        // Clear results — they will be repopulated entirely from incoming events
        self.current_results.clear();
        self.current_accel_results.clear();
        self.results_dirty = true;

        // --- Build one ComputeTask per unique (series, precision, noise) combination ---
        // Group: series_name+params_json+precision+noise_idx → (algorithms, max_n, params)
        type TaskKey = (String, String, String, Option<usize>);
        let mut task_map: HashMap<
            TaskKey,
            (
                Vec<crate::compute::AccelParams>,
                i64,
                HashMap<String, crate::ffi::ParamValue>,
            ),
        > = HashMap::new();

        for combo in &combinations {
            // Convert series params string→ParamValue
            let mut series_params: HashMap<String, crate::ffi::ParamValue> = HashMap::new();
            for (k, v) in &combo.series_params {
                let pv = parse_param_value(v);
                series_params.insert(k.clone(), pv);
            }
            let series_json = crate::compute::core::to_sorted_json(&series_params)
                .unwrap_or_else(|_| "{}".to_string());

            let key: TaskKey = (
                combo.series_name.clone(),
                series_json,
                combo.precision.clone(),
                combo.noise_idx,
            );

            // Build AccelParams for this combo
            let mut accel_params: HashMap<String, crate::ffi::ParamValue> = HashMap::new();
            accel_params.insert("m".to_string(), crate::ffi::ParamValue::Int(combo.method_m));
            for (k, v) in &combo.method_args {
                accel_params.insert(k.clone(), parse_param_value(v));
            }
            let accel = crate::compute::AccelParams {
                name: combo.method_name.clone(),
                params: accel_params,
            };

            let entry = task_map
                .entry(key)
                .or_insert_with(|| (Vec::new(), combo.method_n, series_params.clone()));
            // Track max n_points
            if combo.method_n > entry.1 {
                entry.1 = combo.method_n;
            }
            // Push accel if not already there (deduplicate by serialization)
            let accel_json =
                crate::compute::core::to_sorted_json(&accel.params).unwrap_or_default();
            let already = entry.0.iter().any(|a: &crate::compute::AccelParams| {
                a.name == accel.name
                    && crate::compute::core::to_sorted_json(&a.params).unwrap_or_default()
                        == accel_json
            });
            if !already {
                entry.0.push(accel);
            }
        }

        // Get noises list from experiment config
        let noises = self.state.get_noises();

        for (
            (series_name, _series_json, precision, noise_idx),
            (algorithms, n_points, series_params),
        ) in task_map
        {
            let noise = noise_idx.and_then(|i| noises.get(i));

            let task = crate::compute::task::ComputeTask {
                id: uuid::Uuid::new_v4(),
                precision: precision.clone(),
                series: crate::compute::task::SeriesParams {
                    name: series_name.clone(),
                    x_value: series_params
                        .get("x")
                        .and_then(|v| v.as_f64())
                        .map(|f| f.to_string())
                        .unwrap_or_else(|| "1.0".to_string()),
                    params: series_params,
                },
                n_points: n_points as u64,
                noise: noise.cloned(),
                algorithms,
            };

            if let Some(ref mut engine) = self.compute_engine {
                match engine.start_task(task) {
                    Ok(id) => {
                        self.current_task_id = Some(id);
                        self.is_computing = true;
                        self.status_message = "Computing...".to_string();
                    }
                    Err(e) => {
                        self.status_message = format!("Error starting task: {}", e);
                    }
                }
            }
        }
    }

    /// Process events from compute engine.
    fn process_events(&mut self) {
        if let Some(ref mut rx) = self.event_rx {
            while let Ok(event) = rx.try_recv() {
                log::debug!("Received event: {:?}", event.body);
                match event.body {
                    ComputeEventBody::Started => {
                        self.status_message = "Computation started...".to_string();
                    }
                    ComputeEventBody::Progress {
                        stage,
                        current,
                        total,
                    } => {
                        self.status_message = format!("{}: {}/{}", stage, current, total);
                    }
                    ComputeEventBody::SeriesComplete { name, result } => {
                        log::info!("SeriesComplete: {} Sn points", result.sn.len());
                        self.current_results.insert(name, result);
                        self.status_message = "Series complete, applying algorithms...".to_string();
                        self.results_dirty = true;
                    }
                    ComputeEventBody::AccelComplete { name, result } => {
                        log::info!("AccelComplete: {} values", result.values.len());
                        self.current_accel_results.insert(name, result);
                        self.results_dirty = true;
                    }
                    ComputeEventBody::Complete => {
                        self.status_message = "Computation complete".to_string();
                        self.is_computing = false;
                    }
                    ComputeEventBody::Error { error } => {
                        self.status_message = format!("Error: {}", error);
                        self.is_computing = false;
                    }
                    ComputeEventBody::Cancelled => {
                        self.status_message = "Computation cancelled".to_string();
                        self.is_computing = false;
                    }
                }
            }
        }
    }

    /// Convert a series point to f64, optionally applying symlog.
    fn point_to_f64(
        &self,
        point: &SeriesPoint,
        use_symlog: bool,
        log_linthresh: f64,
    ) -> Option<f64> {
        let sci_val = match point {
            SeriesPoint::Real(v) => v,
            SeriesPoint::Complex(c) => &c.real,
            SeriesPoint::Interval(_) => return None,
        };

        if use_symlog {
            // Convert to plot Scientific format directly to avoid f64 Infinity overflow
            let plot_sci = crate::plot::Scientific(sci_val.mantissa, sci_val.exponent as i32);
            Some(plot_sci.symlog(log_linthresh))
        } else {
            let value = sci_val.to_f64();
            if value.is_finite() {
                Some(value)
            } else {
                None
            }
        }
    }

    /// Extract interval bounds to f64, optionally applying symlog.
    fn point_to_bounds_f64(
        &self,
        point: &SeriesPoint,
        use_symlog: bool,
        log_linthresh: f64,
    ) -> Option<(f64, f64)> {
        match point {
            SeriesPoint::Interval(v) => {
                let mut inf = v.inf.to_f64();
                let mut sup = v.sup.to_f64();
                if use_symlog {
                    inf = crate::plot::Scientific(v.inf.mantissa, v.inf.exponent as i32)
                        .symlog(log_linthresh);
                    sup = crate::plot::Scientific(v.sup.mantissa, v.sup.exponent as i32)
                        .symlog(log_linthresh);
                }
                if inf.is_finite() && sup.is_finite() {
                    Some((inf, sup))
                } else {
                    None
                }
            }
            _ => None,
        }
    }

    fn get_compute_state(&self) -> ComputeInputState {
        ComputeInputState {
            series_tree: self.series_tree.clone(),
            accel_tree: self.accel_tree.clone(),
            noise_tree: self.noise_tree.clone(),
            precision_tree: self.precision_tree.clone(),
        }
    }

    fn recompute_plot_cache(&mut self, use_symlog: bool, log_linthresh: f64) {
        self.plot_cache.symlog = use_symlog;
        self.plot_cache.log_linthresh = log_linthresh;
        self.plot_cache.main_sn.clear();
        self.plot_cache.main_accel.clear();
        self.plot_cache.limits.clear();
        self.plot_cache.deviations.clear();
        self.plot_cache.intervals.clear();
        self.plot_cache.prof_add.clear();
        self.plot_cache.prof_mul.clear();
        self.plot_cache.prof_div.clear();
        self.plot_cache.prof_special.clear();
        self.plot_cache.max_n = 0.0;

        let mut max_n_seen = 0;
        for res in self.current_results.values() {
            max_n_seen = max_n_seen.max(res.sn.len());
        }
        for res in self.current_accel_results.values() {
            max_n_seen = max_n_seen.max(res.values.len());
        }
        self.plot_cache.max_n = max_n_seen as f64;

        let colors_sn = [
            egui::Color32::BLUE,
            egui::Color32::LIGHT_BLUE,
            egui::Color32::DARK_BLUE,
            egui::Color32::from_rgb(100, 100, 255),
        ];

        for (i, (series_name, results)) in self.current_results.iter().enumerate() {
            if let Some(sum_str) = &results.sum {
                let mut sum_val_opt = sum_str.parse::<f64>().ok();
                if sum_val_opt.is_none() {
                    if let Some(real_part) = sum_str.split(" + ").next() {
                        sum_val_opt = real_part.parse::<f64>().ok();
                    }
                }

                if let Some(sum_val) = sum_val_opt {
                    let mut scaled_val = sum_val;
                    if use_symlog {
                        scaled_val =
                            crate::plot::Scientific::from_f64(sum_val).symlog(log_linthresh);
                    }
                    self.plot_cache
                        .limits
                        .push((format!("Limit - {}", series_name), scaled_val));
                }
            }

            if self.show_partial_sums {
                let pts: Vec<egui_plot::PlotPoint> = results
                    .sn
                    .iter()
                    .enumerate()
                    .filter_map(|(j, p)| {
                        self.point_to_f64(p, use_symlog, log_linthresh)
                            .map(|v| [j as f64, v].into())
                    })
                    .collect();

                self.plot_cache.main_sn.push(CachedPlotLine {
                    name: format!("Sn - {}", series_name),
                    color: colors_sn[i % colors_sn.len()],
                    points: pts,
                });
            }

            if self.show_interval_sup || self.show_interval_inf || self.show_interval_shade {
                let mut inf_pts = Vec::new();
                let mut sup_pts = Vec::new();
                for (j, p) in results.sn.iter().enumerate() {
                    if let Some((inf, sup)) = self.point_to_bounds_f64(p, use_symlog, log_linthresh) {
                        inf_pts.push([j as f64, inf].into());
                        sup_pts.push([j as f64, sup].into());
                    }
                }
                if !inf_pts.is_empty() {
                    if self.show_interval_shade {
                        let mut poly_pts = sup_pts.clone();
                        for pt in inf_pts.iter().rev() {
                            poly_pts.push(*pt);
                        }
                        self.plot_cache.intervals.push(CachedIntervalData {
                            name: format!("Bounds - {}", series_name),
                            fill_color: colors_sn[i % colors_sn.len()].gamma_multiply(0.2),
                            points: poly_pts,
                        });
                    }
                    
                    if self.show_interval_sup {
                        // Add dashed outline lines as well
                        self.plot_cache.main_sn.push(CachedPlotLine {
                            name: format!("sup Sn - {}", series_name),
                            color: colors_sn[i % colors_sn.len()].gamma_multiply(0.8),
                            points: sup_pts,
                        });
                    }
                    if self.show_interval_inf {
                        self.plot_cache.main_sn.push(CachedPlotLine {
                            name: format!("inf Sn - {}", series_name),
                            color: colors_sn[i % colors_sn.len()].gamma_multiply(0.8),
                            points: inf_pts,
                        });
                    }
                }
            }
        }

        let colors_accel = [
            egui::Color32::RED,
            egui::Color32::GREEN,
            egui::Color32::YELLOW,
            egui::Color32::from_rgb(0, 255, 255), // Cyan
            egui::Color32::from_rgb(255, 0, 255), // Magenta
        ];

        let colors_prof = [
            egui::Color32::BLUE,
            egui::Color32::RED,
            egui::Color32::GREEN,
            egui::Color32::YELLOW,
            egui::Color32::from_rgb(0, 255, 255),
            egui::Color32::from_rgb(255, 0, 255),
        ];

        for (i, (name, results)) in self.current_accel_results.iter().enumerate() {
            if self.show_accel_values {
                let pts: Vec<egui_plot::PlotPoint> = results
                    .values
                    .iter()
                    .enumerate()
                    .filter_map(|(j, opt_p)| {
                        opt_p
                            .as_ref()
                            .and_then(|p| self.point_to_f64(p, use_symlog, log_linthresh))
                            .map(|v| [j as f64, v].into())
                    })
                    .collect();
                self.plot_cache.main_accel.push(CachedPlotLine {
                    name: name.clone(),
                    color: colors_accel[i % colors_accel.len()],
                    points: pts,
                });
            }

            if self.show_interval_sup || self.show_interval_inf || self.show_interval_shade {
                let mut inf_pts = Vec::new();
                let mut sup_pts = Vec::new();
                for (j, opt_p) in results.values.iter().enumerate() {
                    if let Some(p) = opt_p {
                        if let Some((inf, sup)) = self.point_to_bounds_f64(p, use_symlog, log_linthresh) {
                            inf_pts.push([j as f64, inf].into());
                            sup_pts.push([j as f64, sup].into());
                        }
                    }
                }
                if !inf_pts.is_empty() {
                    if self.show_interval_shade {
                        let mut poly_pts = sup_pts.clone();
                        for pt in inf_pts.iter().rev() {
                            poly_pts.push(*pt);
                        }
                        self.plot_cache.intervals.push(CachedIntervalData {
                            name: format!("Bounds Dev - {}", name),
                            fill_color: colors_accel[i % colors_accel.len()].gamma_multiply(0.2),
                            points: poly_pts,
                        });
                    }
                    if self.show_interval_sup {
                        self.plot_cache.main_accel.push(CachedPlotLine {
                            name: format!("sup - {}", name),
                            color: colors_accel[i % colors_accel.len()].gamma_multiply(0.8),
                            points: sup_pts,
                        });
                    }
                    if self.show_interval_inf {
                        self.plot_cache.main_accel.push(CachedPlotLine {
                            name: format!("inf - {}", name),
                            color: colors_accel[i % colors_accel.len()].gamma_multiply(0.8),
                            points: inf_pts,
                        });
                    }
                }
            }

            // Deviations
            let dev_pts: Vec<egui_plot::PlotPoint> = results
                .deviations
                .iter()
                .enumerate()
                .filter_map(|(j, d)| {
                    let mut val = d.to_f64();
                    if use_symlog {
                        val = crate::plot::Scientific(d.mantissa, d.exponent as i32)
                            .symlog(log_linthresh);
                    }
                    if val.is_finite() {
                        Some([j as f64 + 1.0, val].into())
                    } else {
                        None
                    }
                })
                .collect();
            self.plot_cache.deviations.push(CachedPlotLine {
                name: format!("{} Dev", name),
                color: colors_accel[i % colors_accel.len()],
                points: dev_pts,
            });

            // Profiling
            if let Some(prof) = &results.profiling {
                let base_color = colors_prof[i % colors_prof.len()];

                let add_pts: Vec<egui_plot::PlotPoint> = prof
                    .add
                    .iter()
                    .enumerate()
                    .map(|(j, &v)| {
                        let mut val = v as f64;
                        if use_symlog {
                            val = crate::plot::Scientific::from_f64(val).symlog(log_linthresh);
                        }
                        [(j + 1) as f64, val].into()
                    })
                    .collect();
                self.plot_cache.prof_add.push(CachedPlotLine {
                    name: format!("{} (Add)", name),
                    color: base_color,
                    points: add_pts,
                });

                let mul_pts: Vec<egui_plot::PlotPoint> = prof
                    .mul
                    .iter()
                    .enumerate()
                    .map(|(j, &v)| {
                        let mut val = v as f64;
                        if use_symlog {
                            val = crate::plot::Scientific::from_f64(val).symlog(log_linthresh);
                        }
                        [(j + 1) as f64, val].into()
                    })
                    .collect();
                self.plot_cache.prof_mul.push(CachedPlotLine {
                    name: format!("{} (Mul)", name),
                    color: base_color.gamma_multiply(0.8),
                    points: mul_pts,
                });

                let div_pts: Vec<egui_plot::PlotPoint> = prof
                    .div
                    .iter()
                    .enumerate()
                    .map(|(j, &v)| {
                        let mut val = v as f64;
                        if use_symlog {
                            val = crate::plot::Scientific::from_f64(val).symlog(log_linthresh);
                        }
                        [(j + 1) as f64, val].into()
                    })
                    .collect();
                self.plot_cache.prof_div.push(CachedPlotLine {
                    name: format!("{} (Div)", name),
                    color: base_color.gamma_multiply(0.6),
                    points: div_pts,
                });

                let special_pts: Vec<egui_plot::PlotPoint> = prof
                    .special
                    .iter()
                    .enumerate()
                    .map(|(j, &v)| {
                        let mut val = v as f64;
                        if use_symlog {
                            val = crate::plot::Scientific::from_f64(val).symlog(log_linthresh);
                        }
                        [(j + 1) as f64, val].into()
                    })
                    .collect();
                self.plot_cache.prof_special.push(CachedPlotLine {
                    name: format!("{} (Special)", name),
                    color: base_color.gamma_multiply(0.4),
                    points: special_pts,
                });
            }
        }
    }
}

impl eframe::App for ShanksApp {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        // Capture initial state for debounce checking
        let current_state = self.get_compute_state();

        if Some(&current_state) != self.last_computed_state.as_ref() {
            // State changed
            self.last_input_change = Some(std::time::Instant::now());
            self.last_computed_state = Some(current_state.clone());
        }

        if let Some(change_time) = self.last_input_change {
            if change_time.elapsed().as_millis() > 100 {
                // Debounce threshold passed, sync with cache
                self.last_input_change = None;

                // Cancel existing task if any
                if let (Some(ref mut engine), Some(id)) =
                    (&mut self.compute_engine, self.current_task_id)
                {
                    if self.is_computing {
                        engine.cancel_task(id);
                    }
                }

                self.sync_with_cache_and_compute();
            }
        }

        // Process events from compute engine
        self.process_events();

        // Request repaint if computing
        if self.is_computing {
            ctx.request_repaint();
        }

        // Menu bar
        egui::TopBottomPanel::top("menu_bar").show(ctx, |ui| {
            egui::menu::bar(ui, |ui| {
                ui.menu_button("File", |ui| {
                    if ui.button("Clear Cache").clicked() {
                        if let Err(e) = self.state.cache.lock().unwrap().clear_all() {
                            log::error!("Failed to clear cache: {}", e);
                        }
                    }
                    if ui.button("Quit").clicked() {
                        ctx.send_viewport_cmd(egui::ViewportCommand::Close);
                    }
                });
                ui.menu_button("View", |ui| {
                    if ui.checkbox(&mut self.show_partial_sums, "Show Partial Sums").changed() {
                        self.results_dirty = true;
                    }
                    if ui.checkbox(&mut self.show_accel_values, "Show Accelerated Values").changed() {
                        self.results_dirty = true;
                    }
                    if ui.checkbox(&mut self.show_limit_lines, "Show Limit Lines").changed() {
                        self.results_dirty = true;
                    }
                    if ui.checkbox(&mut self.show_interval_sup, "Show Interval Sup").changed() {
                        self.results_dirty = true;
                    }
                    if ui.checkbox(&mut self.show_interval_inf, "Show Interval Inf").changed() {
                        self.results_dirty = true;
                    }
                    if ui.checkbox(&mut self.show_interval_shade, "Shade Interval Bounds").changed() {
                        self.results_dirty = true;
                    }
                    ui.separator();
                });
            });
        });

        // Left panel - controls
        egui::SidePanel::left("controls").show(ctx, |ui| {
            // Show config info if loaded
            if self.state.experiment.is_some() {
                ui.heading("Experiment Config");
                ui.label("Loaded from config file");

                let series_instances = self.state.get_series_instances();
                let method_instances = self.state.get_method_instances();
                let noises = self.state.get_noises();

                ui.label(format!("Series: {} definitions", series_instances.len()));
                ui.label(format!("Methods: {} definitions", method_instances.len()));
                ui.label(format!("Noises: {} definitions", noises.len()));

                ui.separator();
            }

            // Tree selection mode
            if self.series_tree.is_some() {
                ui.heading("Tree Selection");

                egui::ScrollArea::vertical()
                    .max_height(300.0)
                    .show(ui, |ui| {
                        // Series tree
                        if let Some(ref mut tree) = self.series_tree {
                            tree_ui::draw_tree(ui, tree);
                        }

                        // Acceleration tree
                        if let Some(ref mut tree) = self.accel_tree {
                            tree_ui::draw_tree(ui, tree);
                        }

                        // Noise tree
                        if let Some(ref mut tree) = self.noise_tree {
                            tree_ui::draw_tree(ui, tree);
                        }

                        // Precision tree
                        if let Some(ref mut tree) = self.precision_tree {
                            tree_ui::draw_tree(ui, tree);
                        }
                    });

                ui.separator();

                // Show selected count
                // let series_count = self.series_tree.as_ref().map(|t| t.count_selected()).unwrap_or(0);
                // let accel_count = self.accel_tree.as_ref().map(|t| t.count_selected()).unwrap_or(0);
                // ui.label(format!("Selected: {} series × {} accels", series_count, accel_count));

                ui.add_space(8.0);
                if ui
                    .add_enabled(
                        !self.is_computing
                            && (!self.current_results.is_empty()
                                || !self.current_accel_results.is_empty()),
                        egui::Button::new("Export JSON"),
                    )
                    .clicked()
                {
                    self.export_json();
                }

                // Removed explicit compute buttons for Debounce Cache-first flow
            } else {
                ui.heading("No Series Tree Available");
            }

            // Status
            if !self.status_message.is_empty() {
                ui.colored_label(egui::Color32::LIGHT_GRAY, &self.status_message);
            }

            if self.state.is_offline() {
                ui.colored_label(egui::Color32::YELLOW, "Offline mode - cache only");
            }

            // Cache stats
            ui.separator();
            ui.heading("Cache");
            let stats = self.state.cache_stats();
            ui.label(format!("Series: {}", stats.series_count));
            ui.label(format!("Accelerations: {}", stats.accel_count));
            ui.label(format!("Points: {}", stats.points_count));
        });

        // Central panel - plots
        egui::CentralPanel::default().show(ctx, |ui| {
            if self.current_results.is_empty() && self.current_accel_results.is_empty() {
                ui.centered_and_justified(|ui| {
                    ui.heading("Select a series and click Compute");
                });
                return;
            }

            ui.horizontal(|ui| {
                ui.selectable_value(&mut self.selected_tab, PlotTab::Main, "Основной график");
                ui.selectable_value(&mut self.selected_tab, PlotTab::Deviation, "Отклонения");
                ui.selectable_value(&mut self.selected_tab, PlotTab::Profiling, "Профилирование");

                let config = self
                    .tab_configs
                    .get_mut(&self.selected_tab)
                    .expect("Tab config missing");

                ui.separator();
                ui.checkbox(&mut config.symlog, "Symlog Scale");
                if config.symlog {
                    ui.label("Log Linthresh:");
                    ui.add(egui::DragValue::new(&mut config.log_linthresh).speed(1.0));
                }

                ui.separator();
                if ui.button("🏠 Home").clicked() {
                    config.reset_plot = true;
                }

                ui.separator();
                ui.checkbox(&mut config.enable_aspect_ratio, "Aspect Ratio");
                if config.enable_aspect_ratio {
                    ui.label("X:");
                    ui.add(egui::TextEdit::singleline(&mut config.aspect_x).desired_width(40.0));
                    ui.label("Y:");
                    ui.add(egui::TextEdit::singleline(&mut config.aspect_y).desired_width(40.0));
                }
            });
            ui.separator();

            let use_symlog = self
                .tab_configs
                .get(&self.selected_tab)
                .expect("Tab config missing")
                .symlog;
            let log_linthresh = self
                .tab_configs
                .get(&self.selected_tab)
                .expect("Tab config missing")
                .log_linthresh;

            if self.results_dirty
                || self.plot_cache.symlog != use_symlog
                || self.plot_cache.log_linthresh != log_linthresh
            {
                self.recompute_plot_cache(use_symlog, log_linthresh);
                self.results_dirty = false;
            }

            let config = self
                .tab_configs
                .get_mut(&self.selected_tab)
                .expect("Tab config missing");

            // Plot area
            if self.selected_tab == PlotTab::Profiling {
                ui.vertical(|ui| {
                    ui.horizontal(|ui| {
                        ui.checkbox(&mut self.prof_show_add, "Add")
                            .on_hover_text("Addition and Subtraction");
                        ui.checkbox(&mut self.prof_show_mul, "Mul")
                            .on_hover_text("Multiplication");
                        ui.checkbox(&mut self.prof_show_div, "Div")
                            .on_hover_text("Division");
                        ui.checkbox(&mut self.prof_show_special, "Special")
                            .on_hover_text("Special functions (sqrt, exp, etc.)");
                    });

                    ui.separator();

                    let mut prof_plot = egui_plot::Plot::new("profiling_plot")
                        .view_aspect(2.0)
                        .legend(egui_plot::Legend::default().position(egui_plot::Corner::RightTop))
                        .x_axis_label("n")
                        .y_axis_label("Operations");

                    if use_symlog {
                        prof_plot = prof_plot
                            .y_axis_formatter(move |mark, _range| {
                                crate::plot::symlog_formatter(mark.value, log_linthresh)
                            })
                            .label_formatter(move |name, value| {
                                format!(
                                    "{}\nn={}\nops={}",
                                    name,
                                    value.x,
                                    crate::plot::symlog_formatter(value.y, log_linthresh)
                                )
                            });
                    }

                    prof_plot.show(ui, |plot_ui| {
                        for line in &self.plot_cache.prof_add {
                            if self.prof_show_add {
                                plot_ui.line(
                                    egui_plot::Line::new(&*line.points)
                                        .name(&line.name)
                                        .color(line.color),
                                );
                            }
                        }
                        for line in &self.plot_cache.prof_mul {
                            if self.prof_show_mul {
                                plot_ui.line(
                                    egui_plot::Line::new(&*line.points)
                                        .name(&line.name)
                                        .color(line.color),
                                );
                            }
                        }
                        for line in &self.plot_cache.prof_div {
                            if self.prof_show_div {
                                plot_ui.line(
                                    egui_plot::Line::new(&*line.points)
                                        .name(&line.name)
                                        .color(line.color),
                                );
                            }
                        }
                        for line in &self.plot_cache.prof_special {
                            if self.prof_show_special {
                                plot_ui.line(
                                    egui_plot::Line::new(&*line.points)
                                        .name(&line.name)
                                        .color(line.color),
                                );
                            }
                        }
                    });
                });
            } else {
                let mut plot = egui_plot::Plot::new("series_plot")
                    .view_aspect(1.5)
                    .legend(egui_plot::Legend::default().position(egui_plot::Corner::RightTop))
                    .x_axis_label("n")
                    .y_axis_label("Value");

                if config.reset_plot {
                    plot = plot.reset();
                    config.reset_plot = false;
                }

                if config.enable_aspect_ratio {
                    if let (Ok(x), Ok(y)) = (
                        config.aspect_x.parse::<f64>(),
                        config.aspect_y.parse::<f64>(),
                    ) {
                        if x > 0.0 && y > 0.0 {
                            plot = plot.data_aspect((x / y) as f32);
                        }
                    }
                }

                if use_symlog {
                    plot = plot
                        .y_axis_formatter(move |mark, _range| {
                            crate::plot::symlog_formatter(mark.value, log_linthresh)
                        })
                        .label_formatter(move |name, value| {
                            format!(
                                "{}\nx={}\ny={}",
                                name,
                                value.x,
                                crate::plot::symlog_formatter(value.y, log_linthresh)
                            )
                        });
                }

                plot.show(ui, |plot_ui| {
                    if self.selected_tab == PlotTab::Main {
                        // Plot partial sums if available
                        if self.show_partial_sums {
                            for line in &self.plot_cache.main_sn {
                                let mut l = egui_plot::Line::new(&*line.points)
                                    .name(&line.name)
                                    .color(line.color);
                                // Hack: distinguish bounds lines by checking the name
                                if line.name.starts_with("sup") || line.name.starts_with("inf") {
                                    l = l.style(egui_plot::LineStyle::Dashed { length: 5.0 });
                                }
                                plot_ui.line(l);
                            }
                        }
                        
                        // Plot interval polygons
                        if self.show_interval_shade {
                            for poly_data in &self.plot_cache.intervals {
                                plot_ui.polygon(
                                    egui_plot::Polygon::new(&*poly_data.points)
                                        .name(&poly_data.name)
                                        .fill_color(poly_data.fill_color),
                                );
                            }
                        }

                        // Plot limits if enabled
                        if self.show_limit_lines {
                            let max_n = self.plot_cache.max_n;
                            for (name, val) in &self.plot_cache.limits {
                                plot_ui.line(
                                    egui_plot::Line::new(vec![
                                        [-1.0, *val],
                                        [max_n + 1.0, *val],
                                    ])
                                    .name(name)
                                    .color(egui::Color32::RED)
                                    .width(2.0),
                                );
                            }
                        }

                        // Plot accelerated values if available
                        if self.show_accel_values {
                            for line in &self.plot_cache.main_accel {
                                let mut l = egui_plot::Line::new(&*line.points)
                                    .name(&line.name)
                                    .color(line.color);
                                if line.name.starts_with("sup") || line.name.starts_with("inf") {
                                    l = l.style(egui_plot::LineStyle::Dashed { length: 5.0 });
                                }
                                plot_ui.line(l);
                            }
                        }
                    } else if self.selected_tab == PlotTab::Deviation {
                        for line in &self.plot_cache.deviations {
                            plot_ui.line(
                                egui_plot::Line::new(&*line.points)
                                    .name(&line.name)
                                    .color(line.color),
                            );
                        }
                    }
                });
            }
        });

        // Removed old Auto block
    }
}

impl ShanksApp {
    fn export_json(&self) {
        let timestamp = chrono::Local::now().format("%Y%m%d_%H%M%S").to_string();
        let path = format!("export_{}.json", timestamp);

        let data = serde_json::json!({
            "series": self.current_results,
            "accelerations": self.current_accel_results,
        });

        match serde_json::to_string_pretty(&data) {
            Ok(json) => {
                if let Err(e) = std::fs::write(&path, json) {
                    log::error!("Failed to save export to {}: {}", path, e);
                } else {
                    log::info!("Successfully exported JSON to {}", path);
                }
            }
            Err(e) => log::error!("Failed to serialize export data: {}", e),
        }
    }
}
