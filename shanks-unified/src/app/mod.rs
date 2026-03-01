//! Application state and UI implementation.

mod selection;
mod tree_ui;
mod ui;

use crate::cache::Cache;
use crate::compute::{
    self, AccelData, AccelDesc, ComputeEvent, ComputeTask, SeriesData, SeriesDesc,
};
use crate::experiment::{
    AccelInstance, ExperimentConfig, FilterInstance, NoiseInstance, SeriesInstance,
};
use crate::ffi::{Value, Arr, RealValue, ArrF64, ComplexOf, IntervalOf, ValueOf};
use egui_plot::{Line, LineStyle, PlotPoint, PlotPoints};
use std::collections::{BTreeMap, HashMap, HashSet};
use std::sync::{Arc, Mutex};
use tokio::task::JoinHandle;

pub use selection::{SelectedCombination, SelectionNode, SelectionState};

/// Stable key for identifying a computation result.
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
struct ResultKey {
    series: SeriesInstance,
    precision: String,
    noise: Option<NoiseInstance>,
    accel: Option<AccelInstance>,
}

impl ResultKey {
    fn from_combo(app: &ShanksApp, combo: &SelectedCombination) -> Option<Self> {
        let exp = app.state.experiment.as_ref()?;

        // Use the build_task logic to resolve instances
        let series_def = exp.series.iter().find(|s| s.name == combo.series_name)?;
        let series = series_def.expand().find(|inst| {
            combo.series_params.iter().all(|(k, v)| {
                inst.args
                    .get(k)
                    .map(|sv| sv.to_string() == *v)
                    .unwrap_or(false)
            })
        })?;

        let accel_def = exp.accels.iter().find(|a| a.name == combo.method_name)?;
        let accel = accel_def.expand().find(|inst| {
            inst.m == combo.method_m
                && combo.method_args.iter().all(|(k, v)| {
                    inst.args
                        .get(k)
                        .map(|sv| sv.to_string() == *v)
                        .unwrap_or(false)
                })
        })?;

        let noise = combo
            .noise_idx
            .and_then(|i| exp.noises.get(i).and_then(|d| d.expand().next()));

        Some(Self {
            series,
            precision: combo.precision.clone(),
            noise,
            accel: Some(accel),
        })
    }
}

struct BakedLine {
    name: String,
    data: ArrF64,
    color: egui::Color32,
    width: f32,
    style: LineStyle,
    visible: bool,
}

#[derive(Default)]
struct PlotCache {
    lines_main: Vec<BakedLine>,
    dirty: bool,
}

pub struct AppState {
    pub cache: Arc<Mutex<Cache>>,
    pub experiment: Option<ExperimentConfig>,
    pub n_points: u64,
}

impl AppState {
    pub fn new(experiment: Option<ExperimentConfig>, cache: Cache) -> Self {
        let n_points = experiment.as_ref().and_then(|e| e.n_points).unwrap_or(33);
        Self {
            cache: Arc::new(Mutex::new(cache)),
            experiment,
            n_points,
        }
    }
}

pub struct ShanksApp {
    state: AppState,
    results: HashMap<ResultKey, (SeriesData, Option<AccelData>)>,
    active_tasks: HashMap<ResultKey, JoinHandle<()>>,
    event_rx: tokio::sync::mpsc::Receiver<ComputeEvent<ResultKey>>,
    event_tx: tokio::sync::mpsc::Sender<ComputeEvent<ResultKey>>,

    pub series_tree: Option<SelectionNode>,
    pub accel_tree: Option<SelectionNode>,
    pub noise_tree: Option<SelectionNode>,
    pub precision_tree: Option<SelectionNode>,

    plot_cache: PlotCache,
    pub symlog: bool,
    pub log_linthresh: f64,
    pub show_sn: bool,
    pub show_accel: bool,

    pub status: String,
}

impl ShanksApp {
    pub fn new(state: AppState) -> Self {
        let (event_tx, event_rx) = tokio::sync::mpsc::channel(256);

        let mut app = Self {
            state,
            results: HashMap::new(),
            active_tasks: HashMap::new(),
            event_rx,
            event_tx,
            series_tree: None,
            accel_tree: None,
            noise_tree: None,
            precision_tree: None,
            plot_cache: PlotCache::default(),
            symlog: false,
            log_linthresh: -50.0,
            show_sn: true,
            show_accel: true,
            status: "Ready".to_string(),
        };

        app.rebuild_trees();
        app
    }

    fn rebuild_trees(&mut self) {
        if let Some(ref exp) = self.state.experiment {
            let series_instances: Vec<_> = exp.series.iter().flat_map(|s| s.expand()).collect();
            let method_instances: Vec<_> = exp.accels.iter().flat_map(|a| a.expand()).collect();
            let precisions = exp.precisions.clone().unwrap_or_default();

            self.series_tree = Some(selection::build_series_tree(&series_instances));
            self.accel_tree = Some(selection::build_accel_tree(&method_instances));
            self.noise_tree = Some(selection::build_noise_tree(&exp.noises));
            self.precision_tree = Some(selection::build_precision_tree(&precisions));
        }
    }

    pub fn sync_with_compute(&mut self) {
        let (s_tree, a_tree, n_tree, p_tree) = match (
            &self.series_tree,
            &self.accel_tree,
            &self.noise_tree,
            &self.precision_tree,
        ) {
            (Some(s), Some(a), Some(n), Some(p)) => (s, a, n, p),
            _ => return,
        };

        let combinations = selection::generate_combinations(s_tree, a_tree, n_tree, p_tree);
        let mut requested_keys = HashSet::new();

        for combo in combinations {
            if let Some(key) = ResultKey::from_combo(self, &combo) {
                requested_keys.insert(key.clone());

                if !self.results.contains_key(&key) && !self.active_tasks.contains_key(&key) {
                    let task = ComputeTask {
                        id: key.clone(),
                        precision: key.precision.clone(),
                        series: key.series.clone(),
                        n_points: self.state.n_points,
                        noise: key.noise.clone(),
                        algorithms: vec![key.accel.clone().unwrap()], // Safe due to ResultKey definition
                        filters: vec![],
                    };

                    let tx = self.event_tx.clone();
                    let cache = self.state.cache.lock().unwrap().clone();
                    let handle = compute::spawn_task(task, cache, tx);
                    self.active_tasks.insert(key, handle);
                    self.status = "Computing...".to_string();
                }
            }
        }

        self.active_tasks.retain(|k, handle| {
            if !requested_keys.contains(k) {
                handle.abort();
                false
            } else {
                true
            }
        });
    }

    pub fn process_events(&mut self) {
        while let Ok(event) = self.event_rx.try_recv() {
            match event {
                ComputeEvent::SeriesDone {
                    id,
                    series_data,
                    accel,
                    ..
                } => {
                    let adata = accel.map(|(_, data)| data);
                    self.results.insert(id, (series_data, adata));
                    self.plot_cache.dirty = true;
                }
                ComputeEvent::Complete(id) => {
                    self.active_tasks.remove(&id);
                    if self.active_tasks.is_empty() {
                        self.status = "Complete".to_string();
                    }
                }
                ComputeEvent::Error { id, error } => {
                    self.active_tasks.remove(&id);
                    self.status = format!("Error: {}", error);
                }
                _ => {}
            }
        }
    }

    fn to_plot_point(&self, val: &crate::ffi::RealValue) -> f64 {
        if self.symlog {
            crate::plot::Scientific(val.mantissa, val.exponent as i32).symlog(self.log_linthresh)
        } else {
            val.to_f64()
        }
    }

    fn arr_to_f64(&self, arr: &Arr) -> ArrF64 {
        match arr {
            Arr::Real(v) => ArrF64::Real(v.iter().map(|val| self.to_plot_point(val)).collect()),
            Arr::Complex(c) => ArrF64::Complex(ComplexOf {
                real: c.real.iter().map(|val| self.to_plot_point(val)).collect(),
                imag: c.imag.iter().map(|val| self.to_plot_point(val)).collect(),
            }),
            Arr::Interval(iv) => ArrF64::Interval(IntervalOf {
                inf: iv.inf.iter().map(|val| self.to_plot_point(val)).collect(),
                sup: iv.sup.iter().map(|val| self.to_plot_point(val)).collect(),
            }),
            Arr::CInterval(ci) => ArrF64::CInterval(ComplexOf {
                real: IntervalOf {
                    inf: ci.real.inf.iter().map(|val| self.to_plot_point(val)).collect(),
                    sup: ci.real.sup.iter().map(|val| self.to_plot_point(val)).collect(),
                },
                imag: IntervalOf {
                    inf: ci.imag.inf.iter().map(|val| self.to_plot_point(val)).collect(),
                    sup: ci.imag.sup.iter().map(|val| self.to_plot_point(val)).collect(),
                },
            }),
        }
    }

    pub fn bake_plot_cache(&mut self) {
        if !self.plot_cache.dirty {
            return;
        }

        self.plot_cache.lines_main.clear();

        for (key, (sdata, adata)) in &self.results {
            if self.show_sn {
                let data = self.arr_to_f64(&sdata.sn);
                self.plot_cache.lines_main.push(BakedLine {
                    name: format!("{} Sn", key.precision),
                    data,
                    color: egui::Color32::DARK_GRAY,
                    width: 1.0,
                    style: LineStyle::Dashed { length: 4.0 },
                    visible: true,
                });
            }

            if let Some(data) = adata {
                if self.show_accel {
                    let data = self.arr_to_f64(&data.values);
                    self.plot_cache.lines_main.push(BakedLine {
                        name: format!("{} Accel", key.precision),
                        data,
                        color: egui::Color32::LIGHT_BLUE,
                        width: 2.0,
                        style: LineStyle::Solid,
                        visible: true,
                    });
                }
            }
        }

        self.plot_cache.dirty = false;
    }
}

impl eframe::App for ShanksApp {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        self.process_events();
        self.sync_with_compute();
        self.bake_plot_cache();

        egui::TopBottomPanel::top("top_panel").show(ctx, |ui| {
            ui.horizontal(|ui| {
                ui.heading("Shanks Unified");
                ui.separator();
                ui.label(format!("Status: {}", self.status));
            });
        });

        egui::SidePanel::left("left_panel")
            .resizable(true)
            .show(ctx, |ui| {
                egui::ScrollArea::vertical().show(ui, |ui| {
                    if let Some(ref mut t) = self.series_tree {
                        tree_ui::draw_tree_with_header(ui, "Series", t, true);
                    }
                    if let Some(ref mut t) = self.accel_tree {
                        tree_ui::draw_tree_with_header(ui, "Accelerations", t, true);
                    }
                    if let Some(ref mut t) = self.noise_tree {
                        tree_ui::draw_tree_with_header(ui, "Noises", t, false);
                    }
                    if let Some(ref mut t) = self.precision_tree {
                        tree_ui::draw_tree_with_header(ui, "Precisions", t, true);
                    }
                });
            });

        egui::CentralPanel::default().show(ctx, |ui| {
            ui.horizontal(|ui| {
                if ui.checkbox(&mut self.symlog, "Symlog").changed() {
                    self.plot_cache.dirty = true;
                }
                if self.symlog {
                    ui.add(
                        egui::Slider::new(&mut self.log_linthresh, -100.0..=0.0).text("Threshold"),
                    );
                }
                ui.separator();
                if ui.checkbox(&mut self.show_sn, "Show Sn").changed() {
                    self.plot_cache.dirty = true;
                }
                if ui.checkbox(&mut self.show_accel, "Show Accel").changed() {
                    self.plot_cache.dirty = true;
                }
            });

            egui_plot::Plot::new("main_plot")
                .legend(egui_plot::Legend::default())
                .show(ui, |plot_ui| {
                    for baked in &self.plot_cache.lines_main {
                        if baked.visible {
                            let points: Vec<PlotPoint> = match &baked.data {
                                ArrF64::Real(v) => v.iter().enumerate().map(|(i, &y)| PlotPoint::new(i as f64, y)).collect(),
                                ArrF64::Complex(c) => c.real.iter().enumerate().map(|(i, &y)| PlotPoint::new(i as f64, y)).collect(),
                                ArrF64::Interval(iv) => iv.inf.iter().zip(&iv.sup).enumerate()
                                    .map(|(i, (&inf, &sup))| PlotPoint::new(i as f64, (inf + sup) / 2.0)).collect(),
                                ArrF64::CInterval(ci) => ci.real.inf.iter().zip(&ci.real.sup).enumerate()
                                    .map(|(i, (&inf, &sup))| PlotPoint::new(i as f64, (inf + sup) / 2.0)).collect(),
                            };

                            plot_ui.line(
                                Line::new(PlotPoints::new(points.into_iter().map(|p| [p.x, p.y]).collect::<Vec<_>>()))
                                    .name(&baked.name)
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                            );
                        }
                    }
                });
        });

        if !self.active_tasks.is_empty() || self.plot_cache.dirty {
            ctx.request_repaint();
        }
    }
}
