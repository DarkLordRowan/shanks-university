//! Application state and UI implementation.

mod selection;
mod tree_ui;
mod ui;

use crate::cache::Cache;
use crate::config::{AppConfig, ExperimentConfig, SeriesInstance, MethodInstance, NoiseDef};
use crate::ffi::{ShanksLibrary, SeriesResult, AccelResult, SeriesPoint, ComputeEvent, ComputeEventBody};
use crate::compute::{ComputeEngine, ComputeTask};
use std::sync::{Arc, RwLock, mpsc};

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
    pub cache: Arc<RwLock<Cache>>,
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
            cache: Arc::new(RwLock::new(cache)),
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
        self.cache.read().unwrap().stats().unwrap_or(crate::cache::CacheStats {
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
    current_result: Option<SeriesResult>,
    current_accel_results: Vec<(String, AccelResult)>,
    
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
    auto_compute: bool,

    // Plot controls
    reset_plot: bool,
    enable_aspect_ratio: bool,
    aspect_x: String,
    aspect_y: String,
}

#[derive(PartialEq)]
enum PlotTab {
    Main,
    Deviation,
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
            current_result: None,
            current_accel_results: Vec::new(),
            show_partial_sums,
            show_accel_values: true,
            symlog_main: use_symlog,
            symlog_dev: use_symlog,
            selected_tab: PlotTab::Main,
            status_message: String::new(),
            is_computing: false,
            auto_compute: false,
            reset_plot: false,
            enable_aspect_ratio: true,
            aspect_x: "10.0".to_string(),
            aspect_y: "1.0".to_string(),
        }
    }
    
    /// Start a computation from tree selection.
    fn start_tree_computation(&mut self) {
        if self.state.is_offline() {
            self.status_message = "Cannot compute in offline mode".to_string();
            return;
        }
        
        let series_tree = match &self.series_tree {
            Some(t) => t,
            None => {
                self.status_message = "No series tree available".to_string();
                return;
            }
        };
        
        let accel_tree = match &self.accel_tree {
            Some(t) => t,
            None => {
                self.status_message = "No acceleration tree available".to_string();
                return;
            }
        };
        
        let noise_tree = self.noise_tree.as_ref();
        let precision_tree = self.precision_tree.as_ref();
        
        // Generate combinations
        let combinations = selection::generate_combinations(
            series_tree,
            accel_tree,
            noise_tree.unwrap_or(&SelectionNode::new("empty", "Empty")),
            precision_tree.unwrap_or(&SelectionNode::new("empty", "Empty")),
        );
        
        if combinations.is_empty() {
            self.status_message = "No combinations selected".to_string();
            return;
        }
        
        // Group all accelerations for the first base series/precision combo
        // TODO: Queue all combinations correctly for multiple series selections
        let base_combo = &combinations[0];
        
        let series_n_points = 100;
        
        let mut task = ComputeTask::new(&base_combo.series_name, series_n_points)
            .with_precision(&base_combo.precision)
            .with_x("1.0"); // TODO: Get x from series_params
        
        // Add series parameters
        for (name, value) in &base_combo.series_params {
            let param_value = value.parse::<f64>()
                .map(crate::ffi::ParamValue::Float)
                .unwrap_or_else(|_| crate::ffi::ParamValue::String(value.clone()));
            task = task.with_param(name.clone(), param_value);
        }
        
        // Add all selected algorithms that map to this base combo
        let mut added_algorithms = std::collections::HashSet::new();
        for combo in combinations.iter().filter(|c| c.series_name == base_combo.series_name && c.precision == base_combo.precision) {
            // Deduplicate same algorithms for the same base configuration
            let signature = format!("{}-{}-{}", combo.method_name, combo.method_n, combo.method_m);
            if !added_algorithms.insert(signature) { continue; }
            
            let mut params = std::collections::HashMap::new();
            params.insert("n".to_string(), crate::ffi::ParamValue::Int(combo.method_n));
            params.insert("m".to_string(), crate::ffi::ParamValue::Int(combo.method_m));
            for (k, v) in &combo.method_args {
                let pv = v.parse::<f64>()
                    .map(crate::ffi::ParamValue::Float)
                    .unwrap_or_else(|_| crate::ffi::ParamValue::String(v.clone()));
                params.insert(k.clone(), pv);
            }
            task = task.with_algorithm_params(combo.method_name.clone(), params);
        }
        
        // Start task
        if let Some(ref mut engine) = self.compute_engine {
            match engine.start_task(task) {
                Ok(id) => {
                    self.current_task_id = Some(id);
                    self.is_computing = true;
                    self.status_message = format!("Computing {} combinations...", combinations.len());
                    self.current_result = None;
                    self.current_accel_results.clear();
                }
                Err(e) => {
                    self.status_message = format!("Error: {}", e);
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
                    ComputeEventBody::SeriesComplete { result } => {
                        log::info!("SeriesComplete: {} Sn points", result.sn.len());
                        self.current_result = Some(result);
                        self.status_message = "Series complete, applying algorithms...".to_string();
                    }
                    ComputeEventBody::AccelComplete { name, result } => {
                        log::info!("AccelComplete: {} values", result.values.len());
                        self.current_accel_results.push((name, result));
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
        // Capture initial state for auto-compute
        let initial_state = self.get_compute_state();

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
                        if let Err(e) = self.state.cache.write().unwrap().clear_all() {
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
                let series_count = self.series_tree.as_ref().map(|t| t.count_selected()).unwrap_or(0);
                let accel_count = self.accel_tree.as_ref().map(|t| t.count_selected()).unwrap_or(0);
                ui.label(format!("Selected: {} series × {} accels", series_count, accel_count));
                
                // Compute button
                ui.horizontal(|ui| {
                    if ui.add_enabled(!self.is_computing, egui::Button::new("Compute Selected")).clicked() {
                        self.start_tree_computation();
                    }
                    
                    if self.is_computing && ui.button("Cancel").clicked() {
                        if let (Some(ref mut engine), Some(id)) = (&mut self.compute_engine, self.current_task_id) {
                            engine.cancel_task(id);
                        }
                    }
                    ui.checkbox(&mut self.auto_compute, "Auto");
                });
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
            if self.current_result.is_none() && self.current_accel_results.is_empty() {
                ui.centered_and_justified(|ui| {
                    ui.heading("Select a series and click Compute");
                });
                return;
            }
            
            ui.horizontal(|ui| {
                ui.selectable_value(&mut self.selected_tab, PlotTab::Main, "Основной график");
                ui.selectable_value(&mut self.selected_tab, PlotTab::Deviation, "Отклонения");
                
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
                    if let Some(ref result) = self.current_result {
                        let points: Vec<[f64; 2]> = result.sn
                            .iter()
                            .enumerate()
                            .filter_map(|(i, p)| {
                                self.point_to_f64(p, use_symlog)
                                    .map(|v| [i as f64, v])
                            })
                            .collect();
                        
                        log::debug!("Plotting {} Sn points", points.len());
                        if !points.is_empty() {
                            log::debug!("First point: {:?}, Last point: {:?}", points.first(), points.last());
                        }
                        
                        let line = egui_plot::Line::new(points)
                            .color(egui::Color32::BLUE)
                            .name("Sn");
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
            
        });

        // Check for state changes to trigger auto-compute
        if self.auto_compute {
            let final_state = self.get_compute_state();
            if initial_state != final_state {
                // Cancel existing computation if any
                if let (Some(ref mut engine), Some(id)) = (&mut self.compute_engine, self.current_task_id) {
                    if self.is_computing {
                        engine.cancel_task(id);
                    }
                }
                
                self.start_tree_computation();
            }
        }
    }
}
