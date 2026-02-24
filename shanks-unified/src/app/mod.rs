//! Application state and UI implementation.

mod selection;
mod tree_ui;
mod ui;

use crate::cache::Cache;
use crate::config::{AppConfig, ExperimentConfig, SeriesInstance, MethodInstance, NoiseDef};
use crate::ffi::{ShanksLibrary, SeriesResult, AccelResult, SeriesPoint, ComputeEvent, ComputeEventBody};
use crate::compute::ComputeEngine;
use std::sync::{Arc, RwLock, Mutex, mpsc};
use std::collections::HashMap;

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

pub use selection::{SelectionNode, SelectionState, SelectedCombination};

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
        self.cache.lock().unwrap().stats().unwrap_or(crate::cache::CacheStats {
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
    symlog_main: bool,
    symlog_dev: bool,
    
    // Tab selection
    selected_tab: PlotTab,
    
    // Status
    status_message: String,
    is_computing: bool,

    reset_plot: bool,
    enable_aspect_ratio: bool,
    aspect_x: String,
    aspect_y: String,
    
    // Profiling plot options
    prof_show_add: bool,
    prof_show_mul: bool,
    prof_show_div: bool,
    prof_show_special: bool,
}

#[derive(PartialEq)]
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
            symlog_main: use_symlog,
            symlog_dev: use_symlog,
            selected_tab: PlotTab::Main,
            status_message: String::new(),
            is_computing: false,
            reset_plot: false,
            enable_aspect_ratio: true,
            aspect_x: "10.0".to_string(),
            aspect_y: "1.0".to_string(),
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
        let series_tree = match &self.series_tree { Some(t) => t, None => return };
        let accel_tree = match &self.accel_tree { Some(t) => t, None => return };
        let noise_tree = self.noise_tree.as_ref();
        let precision_tree = self.precision_tree.as_ref();

        let combinations = selection::generate_combinations(
            series_tree, accel_tree,
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

        // --- Build one ComputeTask per unique (series, precision, noise) combination ---
        // Group: series_name+params_json+precision+noise_idx → (algorithms, max_n, params)
        type TaskKey = (String, String, String, Option<usize>);
        let mut task_map: HashMap<TaskKey, (Vec<crate::compute::AccelParams>, i64, HashMap<String, crate::ffi::ParamValue>)> = HashMap::new();


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

            let entry = task_map.entry(key).or_insert_with(|| (Vec::new(), combo.method_n, series_params.clone()));
            // Track max n_points
            if combo.method_n > entry.1 {
                entry.1 = combo.method_n;
            }
            // Push accel if not already there (deduplicate by serialization)
            let accel_json = crate::compute::core::to_sorted_json(&accel.params).unwrap_or_default();
            let already = entry.0.iter().any(|a: &crate::compute::AccelParams| {
                a.name == accel.name &&
                crate::compute::core::to_sorted_json(&a.params).unwrap_or_default() == accel_json
            });
            if !already {
                entry.0.push(accel);
            }
        }

        // Get noises list from experiment config
        let noises = self.state.get_noises();

        for ((series_name, _series_json, precision, noise_idx), (algorithms, n_points, series_params)) in task_map {
            let noise = noise_idx.and_then(|i| noises.get(i));

            let task = crate::compute::task::ComputeTask {
                id: uuid::Uuid::new_v4(),
                precision: precision.clone(),
                series: crate::compute::task::SeriesParams {
                    name: series_name.clone(),
                    x_value: series_params.get("x")
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
                    ComputeEventBody::Progress { stage, current, total } => {
                        self.status_message = format!("{}: {}/{}", stage, current, total);
                    }
                    ComputeEventBody::SeriesComplete { name, result } => {
                        log::info!("SeriesComplete: {} Sn points", result.sn.len());
                        self.current_results.insert(name, result);
                        self.status_message = "Series complete, applying algorithms...".to_string();
                    }
                    ComputeEventBody::AccelComplete { name, result } => {
                        log::info!("AccelComplete: {} values", result.values.len());
                        self.current_accel_results.insert(name, result);
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
    fn point_to_f64(&self, point: &SeriesPoint, use_symlog: bool) -> Option<f64> {
        let sci_val = match point {
            SeriesPoint::Real(v) => v,
            SeriesPoint::Complex(c) => &c.real,
        };
        
        if use_symlog {
            // Convert to plot Scientific format directly to avoid f64 Infinity overflow
            let plot_sci = crate::plot::Scientific(sci_val.mantissa, sci_val.exponent as i32);
            Some(plot_sci.symlog())
        } else {
            let value = sci_val.to_f64();
            if value.is_finite() {
                Some(value)
            } else {
                None
            }
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
                if let (Some(ref mut engine), Some(id)) = (&mut self.compute_engine, self.current_task_id) {
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
                    ui.checkbox(&mut self.show_partial_sums, "Show Partial Sums");
                    ui.checkbox(&mut self.show_accel_values, "Show Accelerated Values");
                    ui.separator();
                });
                ui.menu_button("Help", |ui| {
                    if ui.button("About").clicked() {
                        // Show about dialog
                    }
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
                
                egui::ScrollArea::vertical().max_height(300.0).show(ui, |ui| {
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
                if ui.add_enabled(!self.is_computing && (!self.current_results.is_empty() || !self.current_accel_results.is_empty()), egui::Button::new("Export JSON")).clicked() {
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
                
                ui.separator();
                if self.selected_tab == PlotTab::Main {
                    ui.checkbox(&mut self.symlog_main, "Symlog Scale");
                } else {
                    ui.checkbox(&mut self.symlog_dev, "Symlog Scale");
                }

                ui.separator();
                if ui.button("🏠 Home").clicked() {
                    self.reset_plot = true;
                }

                ui.separator();
                ui.checkbox(&mut self.enable_aspect_ratio, "Aspect Ratio");
                if self.enable_aspect_ratio {
                    ui.label("X:");
                    ui.add(egui::TextEdit::singleline(&mut self.aspect_x).desired_width(40.0));
                    ui.label("Y:");
                    ui.add(egui::TextEdit::singleline(&mut self.aspect_y).desired_width(40.0));
                }
            });
            ui.separator();

            let use_symlog = if self.selected_tab == PlotTab::Main {
                self.symlog_main
            } else {
                self.symlog_dev
            };

            // Plot area
            if self.selected_tab == PlotTab::Profiling {
                ui.vertical(|ui| {
                    ui.horizontal(|ui| {
                        ui.checkbox(&mut self.prof_show_add, "Add").on_hover_text("Addition and Subtraction");
                        ui.checkbox(&mut self.prof_show_mul, "Mul").on_hover_text("Multiplication");
                        ui.checkbox(&mut self.prof_show_div, "Div").on_hover_text("Division");
                        ui.checkbox(&mut self.prof_show_special, "Special").on_hover_text("Special functions (sqrt, exp, etc.)");
                    });

                    ui.separator();

                    let prof_plot = egui_plot::Plot::new("profiling_plot")
                        .view_aspect(2.0)
                        .legend(egui_plot::Legend::default().position(egui_plot::Corner::RightTop))
                        .x_axis_label("n")
                        .y_axis_label("Operations");

                    prof_plot.show(ui, |plot_ui| {
                        let colors = [
                            egui::Color32::BLUE, egui::Color32::RED, egui::Color32::GREEN, egui::Color32::YELLOW,
                            egui::Color32::from_rgb(0, 255, 255), egui::Color32::from_rgb(255, 0, 255),
                        ];

                        for (i, (name, result)) in self.current_accel_results.iter().enumerate() {
                            if let Some(prof) = &result.profiling {
                                let base_color = colors[i % colors.len()];
                                
                                if self.prof_show_add {
                                    let pts: Vec<[f64; 2]> = prof.add.iter().enumerate().map(|(j, &v)| [(j+1) as f64, v as f64]).collect();
                                    plot_ui.line(egui_plot::Line::new(pts).name(format!("{} (Add)", name)).color(base_color));
                                }
                                if self.prof_show_mul {
                                    let pts: Vec<[f64; 2]> = prof.mul.iter().enumerate().map(|(j, &v)| [(j+1) as f64, v as f64]).collect();
                                    plot_ui.line(egui_plot::Line::new(pts).name(format!("{} (Mul)", name)).color(base_color.gamma_multiply(0.8)));
                                }
                                if self.prof_show_div {
                                    let pts: Vec<[f64; 2]> = prof.div.iter().enumerate().map(|(j, &v)| [(j+1) as f64, v as f64]).collect();
                                    plot_ui.line(egui_plot::Line::new(pts).name(format!("{} (Div)", name)).color(base_color.gamma_multiply(0.6)));
                                }
                                if self.prof_show_special {
                                    let pts: Vec<[f64; 2]> = prof.special.iter().enumerate().map(|(j, &v)| [(j+1) as f64, v as f64]).collect();
                                    plot_ui.line(egui_plot::Line::new(pts).name(format!("{} (Special)", name)).color(base_color.gamma_multiply(0.4)));
                                }
                            }
                        }
                    });
                });
            }
 else {
                let mut plot = egui_plot::Plot::new("series_plot")
                    .view_aspect(1.5)
                    .legend(egui_plot::Legend::default().position(egui_plot::Corner::RightTop))
                    .x_axis_label("n")
                    .y_axis_label("Value");

                if self.reset_plot {
                    plot = plot.reset();
                    self.reset_plot = false;
                }

                if self.enable_aspect_ratio {
                    if let (Ok(x), Ok(y)) = (self.aspect_x.parse::<f64>(), self.aspect_y.parse::<f64>()) {
                        if x > 0.0 && y > 0.0 {
                            plot = plot.data_aspect((x / y) as f32);
                        }
                    }
                }
                
                if use_symlog {
                    plot = plot.y_axis_formatter(|mark, _range| {
                        crate::plot::symlog_formatter(mark.value)
                    })
                    .label_formatter(move |name, value| {
                        format!("{}\nx={}\ny={}", name, value.x, crate::plot::symlog_formatter(value.y))
                    });
                }
                
                plot.show(ui, |plot_ui| {
                    if self.selected_tab == PlotTab::Main {
                        // Plot partial sums if available
                    if self.show_partial_sums {
                        let colors = [
                            egui::Color32::BLUE,
                            egui::Color32::LIGHT_BLUE,
                            egui::Color32::DARK_BLUE,
                            egui::Color32::from_rgb(100, 100, 255),
                        ];
                        for (i, (series_name, result)) in self.current_results.iter().enumerate() {
                            let points: Vec<[f64; 2]> = result.sn
                                .iter()
                                .enumerate()
                                .filter_map(|(j, p)| {
                                    self.point_to_f64(p, use_symlog)
                                        .map(|v| [j as f64, v])
                                })
                                .collect();
                            
                            log::debug!("Plotting {} Sn points", points.len());
                            
                            let color = colors[i % colors.len()];
                            let line = egui_plot::Line::new(points)
                                .color(color)
                                .name(format!("Sn - {}", series_name));
                            plot_ui.line(line);
                        }
                    }
                    
                    // Plot accelerated values if available
                    if self.show_accel_values {
                        let colors = [
                            egui::Color32::RED,
                            egui::Color32::GREEN,
                            egui::Color32::YELLOW,
                            egui::Color32::from_rgb(0, 255, 255), // Cyan
                            egui::Color32::from_rgb(255, 0, 255), // Magenta
                        ];
                        
                        for (i, (_name, result)) in self.current_accel_results.iter().enumerate() {
                            let points: Vec<[f64; 2]> = result.values
                                .iter()
                                .enumerate()
                                .filter_map(|(j, opt_p)| {
                                    opt_p.as_ref()
                                         .and_then(|p| self.point_to_f64(p, use_symlog))
                                         .map(|v| [j as f64, v])
                                })
                                .collect();
                            
                            log::info!("GUI Render: plotting {} points for {}", points.len(), _name);
                            
                            let color = colors[i % colors.len()];
                            let line = egui_plot::Line::new(points)
                                .color(color)
                                .name(_name);
                            plot_ui.line(line);
                        }
                    }
                    } else if self.selected_tab == PlotTab::Deviation {
                        let colors = [
                            egui::Color32::RED,
                            egui::Color32::GREEN,
                            egui::Color32::YELLOW,
                            egui::Color32::from_rgb(0, 255, 255), // Cyan
                            egui::Color32::from_rgb(255, 0, 255), // Magenta
                        ];

                        for (i, (_name, result)) in self.current_accel_results.iter().enumerate() {
                            let points: Vec<[f64; 2]> = result.deviations
                                .iter()
                                .enumerate()
                                .filter_map(|(j, d)| {
                                    let mut val = d.to_f64();
                                    if use_symlog {
                                        val = crate::plot::Scientific(d.mantissa, d.exponent as i32).symlog();
                                    }
                                    if val.is_finite() {
                                        Some([j as f64 + 1.0, val]) // Accelerations output n from 1..=n
                                    } else {
                                        None
                                    }
                                })
                                .collect();

                            let color = colors[i % colors.len()];
                            let line = egui_plot::Line::new(points)
                                .color(color)
                                .name(format!("{} Dev", _name));
                            plot_ui.line(line);
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
