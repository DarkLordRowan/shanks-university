//! Application state and UI implementation.

pub mod coordinator;
pub mod data_tab;
pub mod export;
mod selection;

use crate::cache::Cache;
use crate::compute::{self, AccelData, IsCancelled, SeriesData, SeriesEventKind};
use crate::experiment::ExperimentConfig;
use crate::ffi::{Arr, ArrF64, ArrLine, ComplexOf, IntervalOf, Value};
use arc_swap::ArcSwap;
use egui::Id;
use egui_plot::{Line, LineStyle, PlotBounds, PlotPoint, PlotPoints};
use std::{
    collections::{BTreeMap, HashMap, HashSet},
    sync::Arc,
};
use tokio::sync::watch;

pub use selection::{AppSelect, AppSelection};

/// Stable key for identifying a computation result (series + optional accel).
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct ResultKey {
    pub series: compute::SeriesDesc,
    pub accel: Option<compute::AccelDesc>,
}

#[derive(Debug, PartialEq, Eq, Hash, Copy, Clone, serde::Serialize, serde::Deserialize)]
pub enum PlotTab {
    Main,
    Deviation,
    Data,
}

#[derive(
    Debug, PartialEq, Eq, Hash, Copy, Clone, Default, serde::Serialize, serde::Deserialize,
)]
pub enum DeviationMode {
    #[default]
    Magnitude,
    Components,
}

#[derive(Debug, Clone, PartialEq)]
pub struct TabState {
    pub desired_symlog: bool,
    pub desired_log_linthresh: f64,
    pub aspect_ratio: f32,
    pub reset_view: bool,
    pub last_bounds: PlotBounds,
}

impl Default for TabState {
    fn default() -> Self {
        Self {
            desired_symlog: false,
            desired_log_linthresh: -50.0,
            aspect_ratio: 10.0,
            reset_view: false,
            last_bounds: PlotBounds::NOTHING,
        }
    }
}

impl ResultKey {
    fn color(&self) -> egui::Color32 {
        use std::hash::{Hash, Hasher};
        let mut hasher = std::collections::hash_map::DefaultHasher::new();
        self.hash(&mut hasher);
        let hash = hasher.finish();

        let hue = (hash % 360) as f32 / 360.0;
        egui::ecolor::Hsva::new(hue, 1.0, 1.0, 1.0).into()
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum LineKind {
    Sn,
    An,
    Limit,
}

#[derive(Clone)]
struct BakedEventGroup {
    kind: SeriesEventKind,
    text: String,
    points: Vec<PlotPoint>,
    shape: egui_plot::MarkerShape,
    color: egui::Color32,
}

#[derive(Clone)]
pub struct BakedLine {
    pub data: ArrLine,
    color: egui::Color32,
    width: f32,
    style: LineStyle,
    shading_polygons: Vec<Vec<PlotPoint>>,
    events: Vec<BakedEventGroup>,
    kind: LineKind,
}

#[derive(Clone, Copy, PartialEq)]
pub enum Grid {
    Normal,
    Symlog { log_linthresh: f64 },
}

impl Default for Grid {
    fn default() -> Self {
        Grid::Normal
    }
}

#[derive(Default)]
pub struct PlotCache {
    lines_main: Vec<BakedLine>,
    grid_main: Grid,
    lines_deviation: Vec<BakedLine>,
    grid_deviation: Grid,
}

pub struct ShanksApp {
    cfg: Option<ExperimentConfig>,
    cache: Cache,
    config_tx: watch::Sender<Config>,
    combos_tx: watch::Sender<AppSelection>,
    status_rx: watch::Receiver<String>,
    last_err_rx: watch::Receiver<String>,

    app_select: Option<selection::AppSelect>,

    data_cache: Arc<ArcSwap<data_tab::DataCache>>,

    plot_cache: Arc<ArcSwap<PlotCache>>,
    selected_tab: PlotTab,
    main_tab_state: TabState,
    dev_tab_state: TabState,

    deviation_mode: DeviationMode,
    show_sn: bool,
    show_an: bool,
    show_events: bool,
    show_limit_lines: bool,
    show_interval_shading: bool,
    show_legend: bool,

    n_points: u64,
    light_theme: bool,

    show_cache_dialog: bool,
}

#[derive(Debug, Clone, PartialEq)]
pub struct Config {
    pub n_points: u64,
    pub main_tab_state: TabState,
    pub dev_tab_state: TabState,
    pub deviation_mode: DeviationMode,
    pub show_interval_shading: bool,
    pub upd_data: bool,
}

impl ShanksApp {
    pub fn new(experiment: Option<ExperimentConfig>, cache: Cache) -> Self {
        let n_points = experiment.as_ref().and_then(|e| e.n_points).unwrap_or(100);
        let plot_cache = Arc::new(ArcSwap::from_pointee(PlotCache::default()));
        let data_cache = Arc::new(ArcSwap::from_pointee(data_tab::DataCache::default()));
        let (status_tx, status_rx) = watch::channel("Ready".to_string());
        let (last_err_tx, last_err_rx) = watch::channel("".to_string());
        let (config_tx, config_rx) = watch::channel(Config {
            n_points,
            main_tab_state: TabState::default(),
            dev_tab_state: TabState::default(),
            deviation_mode: DeviationMode::Magnitude,
            show_interval_shading: true,
            upd_data: true,
        });
        let (combos_tx, combos_rx) = watch::channel(AppSelection::default());

        coordinator::Coordinator::spawn(
            experiment.clone(),
            cache.clone(),
            plot_cache.clone(),
            data_cache.clone(),
            status_tx,
            last_err_tx,
            config_rx,
            combos_rx,
        );

        let mut app = Self {
            cfg: experiment,
            cache,
            config_tx,
            combos_tx,
            status_rx,
            last_err_rx,
            app_select: None,
            data_cache,
            plot_cache,
            selected_tab: PlotTab::Main,
            main_tab_state: TabState::default(),
            dev_tab_state: TabState::default(),
            deviation_mode: DeviationMode::Magnitude,
            show_sn: true,
            show_an: false,
            show_events: true,
            show_limit_lines: true,
            show_interval_shading: true,
            show_legend: true,
            n_points,
            light_theme: true,

            show_cache_dialog: false,
        };

        app.rebuild_trees();
        app
    }

    fn trigger_config_update(&self) {
        let _ = self.config_tx.send(Config {
            n_points: self.n_points,
            main_tab_state: self.main_tab_state.clone(),
            dev_tab_state: self.dev_tab_state.clone(),
            deviation_mode: self.deviation_mode,
            show_interval_shading: self.show_interval_shading,
            upd_data: true,
        });
    }

    fn rebuild_trees(&mut self) {
        if let Some(ref exp) = self.cfg {
            self.app_select = Some(selection::AppSelect::from_config(exp));
        }
        self.trigger_combinations();
    }

    fn trigger_combinations(&self) {
        let selection = self
            .app_select
            .as_ref()
            .map(|a| a.extract())
            .unwrap_or_default();
        let _ = self.combos_tx.send(selection);
    }

    fn to_plot_point(val: &crate::ffi::RealValue, symlog: bool, log_linthresh: f64) -> f64 {
        if symlog {
            crate::plot::Scientific(val.mantissa, val.exponent as i32).symlog(log_linthresh)
        } else {
            val.to_f64()
        }
    }

    fn to_mag_point(val: &f64, symlog: bool, log_linthresh: f64) -> f64 {
        if symlog {
            if val.is_finite() {
                crate::plot::Scientific::from_f64(val.abs()).symlog(log_linthresh)
            } else {
                *val
            }
        } else {
            val.abs()
        }
    }

    fn arr_to_f64(arr: &Arr, symlog: bool, log_linthresh: f64) -> ArrF64 {
        match arr {
            Arr::Real(v) => ArrF64::Real(
                v.iter()
                    .map(|val| Self::to_plot_point(val, symlog, log_linthresh))
                    .collect(),
            ),
            Arr::Complex(c) => ArrF64::Complex(ComplexOf {
                real: c
                    .real
                    .iter()
                    .map(|val| Self::to_plot_point(val, symlog, log_linthresh))
                    .collect(),
                imag: c
                    .imag
                    .iter()
                    .map(|val| Self::to_plot_point(val, symlog, log_linthresh))
                    .collect(),
            }),
            Arr::Interval(iv) => ArrF64::Interval(IntervalOf {
                inf: iv
                    .inf
                    .iter()
                    .map(|val| Self::to_plot_point(val, symlog, log_linthresh))
                    .collect(),
                sup: iv
                    .sup
                    .iter()
                    .map(|val| Self::to_plot_point(val, symlog, log_linthresh))
                    .collect(),
            }),
            Arr::CInterval(ci) => ArrF64::CInterval(ComplexOf {
                real: IntervalOf {
                    inf: ci
                        .real
                        .inf
                        .iter()
                        .map(|val| Self::to_plot_point(val, symlog, log_linthresh))
                        .collect(),
                    sup: ci
                        .real
                        .sup
                        .iter()
                        .map(|val| Self::to_plot_point(val, symlog, log_linthresh))
                        .collect(),
                },
                imag: IntervalOf {
                    inf: ci
                        .imag
                        .inf
                        .iter()
                        .map(|val| Self::to_plot_point(val, symlog, log_linthresh))
                        .collect(),
                    sup: ci
                        .imag
                        .sup
                        .iter()
                        .map(|val| Self::to_plot_point(val, symlog, log_linthresh))
                        .collect(),
                },
            }),
        }
    }

    fn collect_deviation_data(
        key: &ResultKey,
        line_type: &str,
        arr: &Arr,
        mode: DeviationMode,
        symlog: bool,
        log_linthresh: f64,
    ) -> Vec<(ResultKey, String, ArrF64)> {
        let mut results = Vec::new();
        if mode == DeviationMode::Magnitude {
            let mags = match arr {
                Arr::Real(v) => v
                    .iter()
                    .map(|val| {
                        if symlog {
                            crate::plot::Scientific(val.mantissa.abs(), val.exponent as i32)
                                .symlog(log_linthresh)
                        } else {
                            val.to_f64().abs()
                        }
                    })
                    .collect(),
                Arr::Complex(c) => c
                    .real
                    .iter()
                    .zip(&c.imag)
                    .map(|(re, im)| {
                        let r = re.to_f64();
                        let i = im.to_f64();
                        let mag = r.hypot(i);
                        Self::to_mag_point(&mag, symlog, log_linthresh)
                    })
                    .collect(),
                Arr::Interval(iv) => iv
                    .inf
                    .iter()
                    .zip(&iv.sup)
                    .map(|(inf, sup)| {
                        let mean = (inf.to_f64() + sup.to_f64()) / 2.0;
                        Self::to_mag_point(&mean, symlog, log_linthresh)
                    })
                    .collect(),
                Arr::CInterval(ci) => {
                    let re_means: Vec<f64> = ci
                        .real
                        .inf
                        .iter()
                        .zip(&ci.real.sup)
                        .map(|(inf, sup)| (inf.to_f64() + sup.to_f64()) / 2.0)
                        .collect();
                    let im_means: Vec<f64> = ci
                        .imag
                        .inf
                        .iter()
                        .zip(&ci.imag.sup)
                        .map(|(inf, sup)| (inf.to_f64() + sup.to_f64()) / 2.0)
                        .collect();
                    re_means
                        .into_iter()
                        .zip(im_means)
                        .map(|(re, im)| {
                            let mag = re.hypot(im);
                            Self::to_mag_point(&mag, symlog, log_linthresh)
                        })
                        .collect()
                }
            };
            results.push((key.clone(), line_type.to_string(), ArrF64::Real(mags)));
        } else {
            results.push((
                key.clone(),
                line_type.to_string(),
                Self::arr_to_f64(arr, symlog, log_linthresh),
            ));
        }
        results
    }

    fn process_collected_lines(
        raw_lines: Vec<(
            ResultKey,
            String,
            ArrF64,
            egui::Color32,
            f32,
            LineStyle,
            Vec<compute::SeriesEvent>,
            u64, // start_offset
            LineKind,
        )>,
    ) -> Vec<BakedLine> {
        if raw_lines.is_empty() {
            return Vec::new();
        }

        // 1. Generate all LineInfos for all components
        let mut all_infos = Vec::new();
        for (key, ltype, data, _, _, _, _, _, _) in &raw_lines {
            match data {
                ArrF64::Real(_) => {
                    let component = if ltype.contains("Dev") {
                        Some("Mag")
                    } else {
                        None
                    };
                    all_infos.push(LineInfo::from_key(key, ltype, component));
                }
                ArrF64::Complex(_) => {
                    all_infos.push(LineInfo::from_key(key, ltype, Some("Re")));
                    all_infos.push(LineInfo::from_key(key, ltype, Some("Im")));
                }
                ArrF64::Interval(_) => {
                    all_infos.push(LineInfo::from_key(key, ltype, Some("Inf")));
                    all_infos.push(LineInfo::from_key(key, ltype, Some("Sup")));
                }
                ArrF64::CInterval(_) => {
                    all_infos.push(LineInfo::from_key(key, ltype, Some("Re-Inf")));
                    all_infos.push(LineInfo::from_key(key, ltype, Some("Re-Sup")));
                    all_infos.push(LineInfo::from_key(key, ltype, Some("Im-Inf")));
                    all_infos.push(LineInfo::from_key(key, ltype, Some("Im-Sup")));
                }
            }
        }

        // 2. Shorten all infos into display names
        let shortened_names = shorten_line_infos(&all_infos);

        // 3. Build BakedLines
        let mut baked_lines = Vec::new();
        let mut name_idx = 0;
        for (_key, _ltype, data, color, width, style, events, offset, kind) in raw_lines {
            let baked_data = match data {
                ArrF64::Real(v) => {
                    let name = shortened_names[name_idx].clone();
                    name_idx += 1;
                    let pts = v
                        .iter()
                        .enumerate()
                        .map(|(i, &y)| PlotPoint::new((i as u64 + offset) as f64, y))
                        .collect();
                    ArrLine::Real((name, pts))
                }
                ArrF64::Complex(c) => {
                    let re_name = shortened_names[name_idx].clone();
                    let im_name = shortened_names[name_idx + 1].clone();
                    name_idx += 2;
                    let re_pts = c
                        .real
                        .iter()
                        .enumerate()
                        .map(|(i, &y)| PlotPoint::new((i as u64 + offset) as f64, y))
                        .collect();
                    let im_pts = c
                        .imag
                        .iter()
                        .enumerate()
                        .map(|(i, &y)| PlotPoint::new((i as u64 + offset) as f64, y))
                        .collect();
                    ArrLine::Complex(ComplexOf {
                        real: (re_name, re_pts),
                        imag: (im_name, im_pts),
                    })
                }
                ArrF64::Interval(iv) => {
                    let inf_name = shortened_names[name_idx].clone();
                    let sup_name = shortened_names[name_idx + 1].clone();
                    name_idx += 2;
                    let inf_pts = iv
                        .inf
                        .iter()
                        .enumerate()
                        .map(|(i, &y)| PlotPoint::new((i as u64 + offset) as f64, y))
                        .collect();
                    let sup_pts = iv
                        .sup
                        .iter()
                        .enumerate()
                        .map(|(i, &y)| PlotPoint::new((i as u64 + offset) as f64, y))
                        .collect();
                    ArrLine::Interval(IntervalOf {
                        inf: (inf_name, inf_pts),
                        sup: (sup_name, sup_pts),
                    })
                }
                ArrF64::CInterval(ci) => {
                    let re_inf_name = shortened_names[name_idx].clone();
                    let re_sup_name = shortened_names[name_idx + 1].clone();
                    let im_inf_name = shortened_names[name_idx + 2].clone();
                    let im_sup_name = shortened_names[name_idx + 3].clone();
                    name_idx += 4;
                    let re_inf_pts = ci
                        .real
                        .inf
                        .iter()
                        .enumerate()
                        .map(|(i, &y)| PlotPoint::new((i as u64 + offset) as f64, y))
                        .collect();
                    let re_sup_pts = ci
                        .real
                        .sup
                        .iter()
                        .enumerate()
                        .map(|(i, &y)| PlotPoint::new((i as u64 + offset) as f64, y))
                        .collect();
                    let im_inf_pts = ci
                        .imag
                        .inf
                        .iter()
                        .enumerate()
                        .map(|(i, &y)| PlotPoint::new((i as u64 + offset) as f64, y))
                        .collect();
                    let im_sup_pts = ci
                        .imag
                        .sup
                        .iter()
                        .enumerate()
                        .map(|(i, &y)| PlotPoint::new((i as u64 + offset) as f64, y))
                        .collect();
                    ArrLine::CInterval(ComplexOf {
                        real: IntervalOf {
                            inf: (re_inf_name, re_inf_pts),
                            sup: (re_sup_name, re_sup_pts),
                        },
                        imag: IntervalOf {
                            inf: (im_inf_name, im_inf_pts),
                            sup: (im_sup_name, im_sup_pts),
                        },
                    })
                }
            };

            let mut baked_events: Vec<BakedEventGroup> = Vec::new();
            if !events.is_empty() {
                let mut grouped_events: BTreeMap<
                    (compute::SeriesEventKind, String),
                    Vec<PlotPoint>,
                > = BTreeMap::new();
                for ev in &events {
                    let pts = if let Some(idx) = (ev.n as usize).checked_sub(1) {
                        let extract_pts =
                            |v: &[PlotPoint]| v.get(idx).map(|p| vec![*p]).unwrap_or_default();
                        match &baked_data {
                            ArrLine::Real((_, v)) => extract_pts(v),
                            ArrLine::Complex(c) => vec![c.real.1.get(idx), c.imag.1.get(idx)]
                                .into_iter()
                                .flatten()
                                .cloned()
                                .collect(),
                            ArrLine::Interval(iv) => vec![iv.inf.1.get(idx), iv.sup.1.get(idx)]
                                .into_iter()
                                .flatten()
                                .cloned()
                                .collect(),
                            ArrLine::CInterval(ci) => vec![
                                ci.real.inf.1.get(idx),
                                ci.real.sup.1.get(idx),
                                ci.imag.inf.1.get(idx),
                                ci.imag.sup.1.get(idx),
                            ]
                            .into_iter()
                            .flatten()
                            .cloned()
                            .collect(),
                        }
                    } else {
                        Vec::new()
                    };

                    if !pts.is_empty() {
                        grouped_events
                            .entry((ev.kind, ev.description.clone()))
                            .or_default()
                            .extend(pts);
                    } else {
                        grouped_events
                            .entry((ev.kind, ev.description.clone()))
                            .or_default()
                            .push(PlotPoint::new(ev.n as f64, 0.0));
                    }
                }

                for ((kind, desc), pts) in grouped_events {
                    use egui_plot::MarkerShape;
                    let (shape, color) = match kind {
                        compute::SeriesEventKind::SlowAccel => {
                            (MarkerShape::Circle, egui::Color32::YELLOW)
                        }
                        compute::SeriesEventKind::Monotone => {
                            (MarkerShape::Square, egui::Color32::from_rgb(100, 149, 237))
                        } // CornflowerBlue
                        compute::SeriesEventKind::DivergentAccel => {
                            (MarkerShape::Up, egui::Color32::from_rgb(255, 69, 0))
                        } // OrangeRed
                        compute::SeriesEventKind::SignChanged => {
                            (MarkerShape::Diamond, egui::Color32::from_rgb(147, 112, 219))
                        } // MediumPurple
                        compute::SeriesEventKind::SecondDiff => {
                            (MarkerShape::Down, egui::Color32::from_rgb(218, 112, 214))
                        } // Orchid
                        compute::SeriesEventKind::Trigger => {
                            (MarkerShape::Plus, egui::Color32::from_rgb(255, 215, 0))
                        } // Gold
                        compute::SeriesEventKind::Error => {
                            (MarkerShape::Asterisk, egui::Color32::PURPLE)
                        }
                        compute::SeriesEventKind::DivisionByZero => {
                            (MarkerShape::Cross, egui::Color32::RED)
                        }
                    };
                    baked_events.push(BakedEventGroup {
                        text: format!("{}{}: {}", kind.symbol(), kind, desc),
                        kind,
                        points: pts,
                        shape,
                        color,
                    });
                }
            }

            let mut shading_polygons = Vec::new();
            let gen_shading = |inf: &[PlotPoint], sup: &[PlotPoint]| {
                if inf.len() == sup.len() && inf.len() >= 2 {
                    let mut polys = Vec::with_capacity(inf.len() - 1);
                    for i in 0..inf.len() - 1 {
                        let p_inf0 = inf[i];
                        let p_inf1 = inf[i + 1];
                        let p_sup0 = sup[i];
                        let p_sup1 = sup[i + 1];

                        if p_inf0.y.is_finite()
                            && p_inf1.y.is_finite()
                            && p_sup0.y.is_finite()
                            && p_sup1.y.is_finite()
                        {
                            polys.push(vec![p_inf0, p_inf1, p_sup1, p_sup0]);
                        }
                    }
                    polys
                } else {
                    Vec::new()
                }
            };

            match &baked_data {
                ArrLine::Interval(iv) => {
                    shading_polygons.extend(gen_shading(&iv.inf.1, &iv.sup.1));
                }
                ArrLine::CInterval(ci) => {
                    shading_polygons.extend(gen_shading(&ci.real.inf.1, &ci.real.sup.1));
                    shading_polygons.extend(gen_shading(&ci.imag.inf.1, &ci.imag.sup.1));
                }
                _ => {}
            }

            baked_lines.push(BakedLine {
                data: baked_data,
                color,
                width,
                style,
                shading_polygons,
                events: baked_events, // Use the newly processed events
                kind,
            });
        }
        baked_lines
    }

    pub fn bake_plot_cache_task(
        series_results: &HashMap<compute::SeriesDesc, Option<Arc<SeriesData>>>,
        accel_results: &HashMap<ResultKey, Option<Arc<AccelData>>>,
        main_tab_state: &TabState,
        dev_tab_state: &TabState,
        deviation_mode: DeviationMode,
        is_cancelled: IsCancelled,
    ) -> PlotCache {
        let mut main_raw = Vec::new();
        let mut dev_raw = Vec::new();

        let main_symlog = main_tab_state.desired_symlog;
        let main_thresh = main_tab_state.desired_log_linthresh;
        let dev_symlog = dev_tab_state.desired_symlog;
        let dev_thresh = dev_tab_state.desired_log_linthresh;

        let mut max_n = 0usize;
        for sdata in series_results.values() {
            let Some(sdata) = sdata else { continue };
            max_n = max_n.max(match &sdata.result.sn {
                Arr::Real(v) => v.len(),
                Arr::Complex(c) => c.real.len(),
                Arr::Interval(iv) => iv.inf.len(),
                Arr::CInterval(ci) => ci.real.inf.len(),
            });
        }
        for (_key, adata) in accel_results {
            if is_cancelled.cancelled() {
                return PlotCache::default();
            }
            let Some(adata) = adata else { continue };
            max_n = max_n.max(
                adata.start_offset as usize
                    + match &adata.result.sn {
                        Arr::Real(v) => v.len(),
                        Arr::Complex(c) => c.real.len(),
                        Arr::Interval(iv) => iv.inf.len(),
                        Arr::CInterval(ci) => ci.real.inf.len(),
                    },
            );
        }

        // 1. Process Base Series Results
        for (s_desc, sdata) in series_results {
            if is_cancelled.cancelled() {
                return PlotCache::default();
            }
            let Some(sdata) = sdata else { continue };
            let key = ResultKey {
                series: s_desc.clone(),
                accel: None,
            };
            let base_color = key.color();

            // Sn for base series
            main_raw.push((
                key.clone(),
                "Sn".to_string(),
                Self::arr_to_f64(&sdata.result.sn, main_symlog, main_thresh),
                base_color.gamma_multiply(0.4),
                2.0,
                LineStyle::Dashed { length: 4.0 },
                Vec::new(),
                0,
                LineKind::Sn,
            ));

            // Limit line
            if let Some(ref val) = sdata.sum {
                let points = match val {
                    Value::Real(rv) => {
                        ArrF64::Real(vec![
                            Self::to_plot_point(rv, main_symlog, main_thresh);
                            max_n + 1
                        ])
                    }
                    Value::Complex(cv) => ArrF64::Complex(ComplexOf {
                        real: vec![
                            Self::to_plot_point(&cv.real, main_symlog, main_thresh);
                            max_n + 1
                        ],
                        imag: vec![
                            Self::to_plot_point(&cv.imag, main_symlog, main_thresh);
                            max_n + 1
                        ],
                    }),
                    Value::Interval(iv) => ArrF64::Interval(IntervalOf {
                        inf: vec![
                            Self::to_plot_point(&iv.inf, main_symlog, main_thresh);
                            max_n + 1
                        ],
                        sup: vec![
                            Self::to_plot_point(&iv.sup, main_symlog, main_thresh);
                            max_n + 1
                        ],
                    }),
                    Value::CInterval(ci) => ArrF64::CInterval(ComplexOf {
                        real: IntervalOf {
                            inf: vec![
                                Self::to_plot_point(&ci.real.inf, main_symlog, main_thresh);
                                max_n + 1
                            ],
                            sup: vec![
                                Self::to_plot_point(&ci.real.sup, main_symlog, main_thresh);
                                max_n + 1
                            ],
                        },
                        imag: IntervalOf {
                            inf: vec![
                                Self::to_plot_point(&ci.imag.inf, main_symlog, main_thresh);
                                max_n + 1
                            ],
                            sup: vec![
                                Self::to_plot_point(&ci.imag.sup, main_symlog, main_thresh);
                                max_n + 1
                            ],
                        },
                    }),
                };
                main_raw.push((
                    key.clone(),
                    "Limit".to_string(),
                    points,
                    egui::Color32::RED,
                    1.0,
                    LineStyle::Dotted { spacing: 4.0 },
                    Vec::new(),
                    0,
                    LineKind::Limit,
                ));
            }

            // An for base series (on main plot)
            main_raw.push((
                key.clone(),
                "An".to_string(),
                Self::arr_to_f64(&sdata.result.deviations, main_symlog, main_thresh),
                base_color.gamma_multiply(0.8),
                1.0,
                LineStyle::Dashed { length: 4.0 },
                Vec::new(),
                0,
                LineKind::An,
            ));

            // Deviation of base series
            let collected = Self::collect_deviation_data(
                &key,
                "Sn",
                &sdata.result.deviations,
                deviation_mode,
                dev_symlog,
                dev_thresh,
            );
            for (k, lt, data) in collected {
                dev_raw.push((
                    k,
                    lt,
                    data,
                    base_color.gamma_multiply(0.5),
                    1.0,
                    LineStyle::Solid,
                    Vec::new(),
                    0,
                    LineKind::Sn,
                ));
            }
        }

        // 2. Process Accelerated Results
        for (key, adata) in accel_results {
            let Some(adata) = adata else { continue };
            let base_color = key.color();
            let is_filtered = key
                .accel
                .as_ref()
                .map(|a| a.filter.is_some())
                .unwrap_or(false);
            // Sn for accel results
            main_raw.push((
                key.clone(),
                if is_filtered {
                    "Estimated Sn".to_string()
                } else {
                    "Accel Sn".to_string()
                },
                Self::arr_to_f64(&adata.result.sn, main_symlog, main_thresh),
                if is_filtered {
                    egui::Color32::from_rgb(240, 230, 140)
                } else {
                    base_color
                },
                if is_filtered { 3.0 } else { 2.0 },
                LineStyle::Solid,
                adata.events.clone(),
                adata.start_offset,
                LineKind::Sn,
            ));

            // An for accel results (on main plot)
            main_raw.push((
                key.clone(),
                if is_filtered {
                    "Estimated An".to_string()
                } else {
                    "Accel An".to_string()
                },
                Self::arr_to_f64(&adata.result.deviations, main_symlog, main_thresh),
                if is_filtered {
                    egui::Color32::from_rgb(240, 230, 140).gamma_multiply(0.8)
                } else {
                    base_color.gamma_multiply(0.8)
                },
                if is_filtered { 3.0 } else { 2.0 },
                LineStyle::Solid,
                adata.events.clone(),
                adata.start_offset,
                LineKind::An,
            ));

            // Deviation
            let collected = Self::collect_deviation_data(
                key,
                if is_filtered {
                    "Estimated Sn"
                } else {
                    "Accel Sn"
                },
                &adata.result.deviations,
                deviation_mode,
                dev_symlog,
                dev_thresh,
            );
            for (k, lt, data) in collected {
                dev_raw.push((
                    k,
                    lt,
                    data,
                    base_color,
                    2.0,
                    LineStyle::Solid,
                    adata.events.clone(),
                    adata.start_offset,
                    LineKind::Sn,
                ));
            }
        }
        PlotCache {
            lines_main: Self::process_collected_lines(main_raw),
            grid_main: if main_symlog {
                Grid::Symlog {
                    log_linthresh: main_thresh,
                }
            } else {
                Grid::Normal
            },
            lines_deviation: Self::process_collected_lines(dev_raw),
            grid_deviation: if dev_symlog {
                Grid::Symlog {
                    log_linthresh: dev_thresh,
                }
            } else {
                Grid::Normal
            },
        }
    }
}

impl eframe::App for ShanksApp {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        ctx.set_theme(if self.light_theme {
            egui::Theme::Light
        } else {
            egui::Theme::Dark
        });

        egui::TopBottomPanel::top("top_panel").show(ctx, |ui| {
            egui::MenuBar::new().ui(ui, |ui| {
                ui.menu_button("File", |ui| {
                    if ui.button("Clear Cache").clicked() {
                        self.show_cache_dialog = true;
                    }
                    // CSV export moved to Export tab with filtering and symlog checks
                    if ui.button("Quit").clicked() {
                        ctx.send_viewport_cmd(egui::ViewportCommand::Close);
                    }
                });
                ui.menu_button("View", |ui| {
                    ui.checkbox(&mut self.show_sn, "Show Sn");
                    ui.checkbox(&mut self.show_an, "Show An");
                    ui.checkbox(&mut self.show_events, "Show Events");
                    ui.separator();
                    ui.checkbox(&mut self.show_limit_lines, "Show Limit Line");
                    ui.checkbox(&mut self.show_interval_shading, "Show Interval Shading");
                    ui.separator();
                    ui.checkbox(&mut self.show_legend, "Show Legend");
                    ui.checkbox(&mut self.light_theme, "Light Theme");
                });
                ui.label(format!("Status: {}", *self.status_rx.borrow()));
            });
        });

        if self.show_cache_dialog {
            egui::Window::new("Clear Cache")
                .collapsible(false)
                .resizable(false)
                .show(ctx, |ui| {
                    ui.label("Are you sure you want to clear the cache?");
                    ui.horizontal(|ui| {
                        if ui.button("Yes").clicked() {
                            let cache = self.cache.clone();
                            tokio::spawn(async move {
                                if let Err(e) = cache.clear_all().await {
                                    log::error!("Failed to clear cache: {}", e);
                                }
                            });
                            self.show_cache_dialog = false;
                        }
                        if ui.button("No").clicked() {
                            self.show_cache_dialog = false;
                        }
                    });
                });
        }

        egui::SidePanel::left("left_panel")
            .resizable(true)
            .show(ctx, |ui| {
                egui::ScrollArea::vertical().show(ui, |ui| {
                    ui.label("N:");
                    let n = egui::Slider::new(&mut self.n_points, 0..=1000)
                        .clamping(egui::SliderClamping::Never);
                    if ui.add(n).changed() {
                        self.trigger_config_update();
                    }
                    ui.separator();
                    let mut trees_changed = false;
                    if let Some(app_select) = &mut self.app_select {
                        trees_changed = app_select.draw(ui);
                    }
                    if trees_changed {
                        self.trigger_combinations();
                    }
                    ui.separator();

                    ui.heading("📄 Export Data");
                    ui.horizontal(|ui| {
                        let cache_lock = self.plot_cache.load();
                        let (live_lines, grid, last_bounds) = match self.selected_tab {
                            PlotTab::Main | PlotTab::Data => (
                                &cache_lock.lines_main,
                                cache_lock.grid_main,
                                self.main_tab_state.last_bounds,
                            ),
                            PlotTab::Deviation => (
                                &cache_lock.lines_deviation,
                                cache_lock.grid_deviation,
                                self.main_tab_state.last_bounds,
                            ),
                        };

                        let exportable = grid == Grid::Normal;
                        if ui
                            .button(if exportable {
                                "CSV"
                            } else {
                                "CSV (disable symlog)"
                            })
                            .clicked()
                        {
                            if exportable {
                                if let Err(e) = crate::app::export::perform_export(live_lines) {
                                    log::error!("CSV export failed: {}", e);
                                } else {
                                    log::info!("CSV exported successfully");
                                }
                            } else {
                                log::error!("Cannot export CSV when Symlog mode is enabled.");
                            }
                        }

                        if ui.button("JSON").clicked() {
                            if let Err(e) = crate::app::export::perform_export_json(
                                live_lines,
                                grid,
                                last_bounds,
                            ) {
                                log::error!("JSON export failed: {}", e);
                            } else {
                                log::info!("JSON exported successfully");
                            }
                        }
                    });

                    ui.separator();
                    ui.label(self.last_err_rx.borrow().as_str());
                });
            });

        egui::CentralPanel::default().show(ctx, |ui| {
            ui.horizontal(|ui| {
                let mut changed_tab = false;
                changed_tab |= ui
                    .selectable_value(&mut self.selected_tab, PlotTab::Main, "Main Plot")
                    .changed();
                changed_tab |= ui
                    .selectable_value(&mut self.selected_tab, PlotTab::Deviation, "Deviations")
                    .changed();
                changed_tab |= ui
                    .selectable_value(&mut self.selected_tab, PlotTab::Data, "Data View")
                    .changed();

                if changed_tab {
                    self.trigger_config_update();
                };

                ui.separator();

                if self.selected_tab != PlotTab::Data {
                    let mut config_changed = false;
                    {
                        let tab_state = match self.selected_tab {
                            PlotTab::Main => &mut self.main_tab_state,
                            PlotTab::Deviation => &mut self.dev_tab_state,
                            PlotTab::Data => unreachable!(),
                        };

                        if ui
                            .checkbox(&mut tab_state.desired_symlog, "Symlog")
                            .changed()
                        {
                            config_changed = true;
                        }
                        if tab_state.desired_symlog {
                            ui.label("Log Linthresh: e^");
                            let slider = egui::Slider::new(
                                &mut tab_state.desired_log_linthresh,
                                -100.0..=100.0,
                            )
                            .clamping(egui::SliderClamping::Never);
                            if ui.add(slider).changed() {
                                config_changed = true;
                            }
                        }

                        if ui.button("Home").clicked() {
                            tab_state.reset_view = true;
                        }

                        ui.separator();
                        ui.label("Aspect Ratio");
                        ui.add(
                            egui::Slider::new(&mut tab_state.aspect_ratio, 0.1..=100.0)
                                .logarithmic(true),
                        );
                    }
                    if config_changed {
                        self.trigger_config_update();
                    }

                    if self.selected_tab == PlotTab::Deviation {
                        ui.separator();
                        ui.label("Dev Mode:");
                        if ui
                            .radio_value(&mut self.deviation_mode, DeviationMode::Magnitude, "Mag")
                            .changed()
                        {
                            self.trigger_config_update();
                        }
                        if ui
                            .radio_value(
                                &mut self.deviation_mode,
                                DeviationMode::Components,
                                "Comp",
                            )
                            .changed()
                        {
                            self.trigger_config_update();
                        }
                    }
                }
            });

            if self.selected_tab == PlotTab::Data {
                let dlock = self.data_cache.load();
                data_tab::show(ui, &*dlock);
                return;
            }

            let cache_lock = self.plot_cache.load();
            let (plot_lines, current_tab_state, grid) = match self.selected_tab {
                PlotTab::Main => (
                    &cache_lock.lines_main,
                    &mut self.main_tab_state,
                    cache_lock.grid_main,
                ),
                PlotTab::Deviation => (
                    &cache_lock.lines_deviation,
                    &mut self.dev_tab_state,
                    cache_lock.grid_deviation,
                ),
                PlotTab::Data => unreachable!(),
            };

            ui.scope(|ui| {
                let mut plot =
                    egui_plot::Plot::new("main_plot").data_aspect(current_tab_state.aspect_ratio);

                if self.show_legend {
                    plot = plot.legend(
                        egui_plot::Legend::default()
                            .follow_insertion_order(true)
                            .grouping(egui_plot::LegendGrouping::ById),
                    );
                }

                match grid {
                    Grid::Symlog { log_linthresh } => {
                        plot = plot.y_axis_formatter(move |grid_mark, _range| {
                            crate::plot::symlog_grid_formatter(grid_mark.value, log_linthresh)
                        })
                    }
                    Grid::Normal => {
                        plot = plot.y_axis_formatter(|grid_mark, _range| {
                            crate::plot::format_grid_value(grid_mark.value)
                        })
                    }
                }

                if current_tab_state.reset_view {
                    plot = plot.reset();
                    current_tab_state.reset_view = false;
                }

                plot = plot.label_formatter(move |name, value| {
                    if name.is_empty() {
                        return format!(
                            "n = {:.0}\ny = {}",
                            value.x,
                            crate::plot::format_value(value.y)
                        );
                    }
                    let y_str = match grid {
                        Grid::Normal => crate::plot::format_value(value.y),
                        Grid::Symlog { log_linthresh } => {
                            crate::plot::symlog_formatter(value.y, log_linthresh)
                        }
                    };
                    format!("{}\nn = {:.0}\ny = {}", name, value.x, y_str)
                });

                fn limit_pts_slice<'a>(pts: &'a [PlotPoint], max_n: u64) -> &'a [PlotPoint] {
                    &pts[0..(max_n as usize).min(pts.len())]
                }

                let plot_response = plot.show(ui, |plot_ui| {
                    for baked in plot_lines {
                        // Filter by View toggles — no cache rebuild needed
                        match baked.kind {
                            LineKind::Sn if !self.show_sn => continue,
                            LineKind::An if !self.show_an => continue,
                            LineKind::Limit if !self.show_limit_lines => continue,
                            _ => {}
                        }

                        if self.show_events {
                            for ev_group in &baked.events {
                                let points = egui_plot::Points::new(
                                    &ev_group.text,
                                    PlotPoints::Borrowed(&ev_group.points),
                                )
                                .shape(ev_group.shape)
                                .color(ev_group.color)
                                .radius(5.0)
                                .id(Id::new(ev_group.kind));

                                plot_ui.points(points);
                            }
                        }

                        if self.show_interval_shading {
                            for poly_pts in &baked.shading_polygons {
                                plot_ui.polygon(
                                    egui_plot::Polygon::new(
                                        "",
                                        PlotPoints::Borrowed(limit_pts_slice(
                                            poly_pts,
                                            self.n_points,
                                        )),
                                    )
                                    .fill_color(baked.color.gamma_multiply(0.2))
                                    .stroke(egui::Stroke::NONE),
                                );
                            }
                        }

                        match &baked.data {
                            ArrLine::Real((name, v)) => {
                                plot_ui.line(
                                    Line::new(
                                        name,
                                        PlotPoints::Borrowed(limit_pts_slice(v, self.n_points)),
                                    )
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                                );
                            }
                            ArrLine::Complex(c) => {
                                plot_ui.line(
                                    Line::new(
                                        &c.real.0,
                                        PlotPoints::Borrowed(limit_pts_slice(
                                            &c.real.1,
                                            self.n_points,
                                        )),
                                    )
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                                );
                                plot_ui.line(
                                    Line::new(
                                        &c.imag.0,
                                        PlotPoints::Borrowed(limit_pts_slice(
                                            &c.imag.1,
                                            self.n_points,
                                        )),
                                    )
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                                );
                            }
                            ArrLine::Interval(iv) => {
                                plot_ui.line(
                                    Line::new(
                                        &iv.inf.0,
                                        PlotPoints::Borrowed(limit_pts_slice(
                                            &iv.inf.1,
                                            self.n_points,
                                        )),
                                    )
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                                );
                                plot_ui.line(
                                    Line::new(
                                        &iv.sup.0,
                                        PlotPoints::Borrowed(limit_pts_slice(
                                            &iv.sup.1,
                                            self.n_points,
                                        )),
                                    )
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                                );
                            }
                            ArrLine::CInterval(ci) => {
                                plot_ui.line(
                                    Line::new(
                                        &ci.real.inf.0,
                                        PlotPoints::Borrowed(limit_pts_slice(
                                            &ci.real.inf.1,
                                            self.n_points,
                                        )),
                                    )
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                                );
                                plot_ui.line(
                                    Line::new(
                                        &ci.real.sup.0,
                                        PlotPoints::Borrowed(limit_pts_slice(
                                            &ci.real.sup.1,
                                            self.n_points,
                                        )),
                                    )
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                                );
                                plot_ui.line(
                                    Line::new(
                                        &ci.imag.inf.0,
                                        PlotPoints::Borrowed(limit_pts_slice(
                                            &ci.imag.inf.1,
                                            self.n_points,
                                        )),
                                    )
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                                );
                                plot_ui.line(
                                    Line::new(
                                        &ci.imag.sup.0,
                                        PlotPoints::Borrowed(limit_pts_slice(
                                            &ci.imag.sup.1,
                                            self.n_points,
                                        )),
                                    )
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                                );
                            }
                        }
                    }
                });
                current_tab_state.last_bounds = *plot_response.transform.bounds();
            });
        });

        ctx.request_repaint();
    }
}

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
struct LineInfo {
    precision: String,
    series_name: String,
    series_x: String,
    series_args: BTreeMap<String, String>,
    noise: Option<(String, String, BTreeMap<String, String>, i64)>, // Type, Method, Args, Seed
    accel_name: Option<String>,
    accel_m: Option<i64>,
    accel_args: BTreeMap<String, String>,
    filter: Option<(String, BTreeMap<String, String>)>,
    line_type: String,         // "Sn", "Accel", "Sn Dev", "Accel Dev"
    component: Option<String>, // "Mag", "Re", "Im", "Inf", "Sup"
}

impl LineInfo {
    fn from_key(key: &ResultKey, line_type: &str, component: Option<&str>) -> Self {
        let series_x = key.series.series.x.to_string();
        let series_args = key
            .series
            .series
            .args
            .iter()
            .map(|(k, v)| (k.clone(), v.to_string()))
            .collect();
        let noise = key.series.noise.as_ref().map(|n| {
            (
                n.noise_type.clone(),
                n.method.clone(),
                n.args
                    .iter()
                    .map(|(k, v)| (k.clone(), v.to_string()))
                    .collect(),
                n.seed,
            )
        });
        let accel_name = key.accel.as_ref().map(|a| a.accel.name.clone());
        let accel_m = key.accel.as_ref().map(|a| a.accel.m);
        let accel_args = key
            .accel
            .as_ref()
            .map(|a| {
                a.accel
                    .args
                    .iter()
                    .map(|(k, v)| (k.clone(), v.to_string()))
                    .collect()
            })
            .unwrap_or_default();
        let filter = key.accel.as_ref().and_then(|a| {
            a.filter.as_ref().map(|f| {
                (
                    f.filter_type.clone(),
                    f.args
                        .iter()
                        .map(|(k, v)| (k.clone(), v.to_string()))
                        .collect(),
                )
            })
        });

        Self {
            precision: key.series.precision.clone(),
            series_name: key.series.series.name.clone(),
            series_x,
            series_args,
            noise,
            accel_name,
            accel_m,
            accel_args,
            filter,
            line_type: line_type.to_string(),
            component: component.map(|s| s.to_string()),
        }
    }
}

fn shorten_line_infos(infos: &[LineInfo]) -> Vec<String> {
    if infos.is_empty() {
        return Vec::new();
    }

    fn varies<T: PartialEq>(infos: &[LineInfo], f: impl Fn(&LineInfo) -> T) -> bool {
        let first = f(&infos[0]);
        infos[1..].iter().any(|i| f(i) != first)
    }

    fn diff_keys<'a>(
        infos: &'a [LineInfo],
        f: impl Fn(&'a LineInfo) -> Option<&'a BTreeMap<String, String>>,
    ) -> HashSet<String> {
        infos
            .iter()
            .flat_map(|i| f(i).into_iter().flat_map(|m| m.keys().cloned()))
            .collect::<HashSet<_>>()
            .into_iter()
            .filter(|k| {
                let first = f(&infos[0]).and_then(|m| m.get(k.as_str()));
                infos[1..]
                    .iter()
                    .any(|i| f(i).and_then(|m| m.get(k.as_str())) != first)
            })
            .collect()
    }

    let fmt_args = |map: &BTreeMap<String, String>, diff: &HashSet<String>| -> Option<String> {
        let args: Vec<_> = map
            .keys()
            .filter(|k| diff.contains(*k))
            .map(|k| format!("{}={}", k, map[k]))
            .collect();
        if args.is_empty() {
            None
        } else {
            Some(format!("({})", args.join(", ")))
        }
    };

    let show_precision = varies(infos, |i| i.precision.clone());
    let show_series_name = varies(infos, |i| i.series_name.clone());
    let show_series_x = varies(infos, |i| i.series_x.clone());
    let show_noise_type = varies(infos, |i| i.noise.as_ref().map(|(t, _, _, _)| t.clone()));
    let show_noise_meth = varies(infos, |i| i.noise.as_ref().map(|(_, m, _, _)| m.clone()));
    let show_noise_seed = varies(infos, |i| i.noise.as_ref().map(|(_, _, _, s)| *s));
    let show_accel_name = varies(infos, |i| i.accel_name.clone());
    let show_accel_m = varies(infos, |i| i.accel_m);
    let show_filter_type = varies(infos, |i| i.filter.as_ref().map(|(t, _)| t.clone()));
    let mut show_line_type = varies(infos, |i| i.line_type.clone());
    let mut show_component = varies(infos, |i| i.component.clone());

    let diff_series_args = diff_keys(infos, |i| Some(&i.series_args));
    let diff_noise_args = diff_keys(infos, |i| i.noise.as_ref().map(|(_, _, m, _)| m));
    let diff_accel_args = diff_keys(infos, |i| Some(&i.accel_args));
    let diff_filter_args = diff_keys(infos, |i| i.filter.as_ref().map(|(_, m)| m));

    if !show_precision
        && !show_series_name
        && !show_series_x
        && diff_series_args.is_empty()
        && !show_noise_type
        && !show_noise_meth
        && !show_noise_seed
        && diff_noise_args.is_empty()
        && !show_accel_name
        && !show_accel_m
        && diff_accel_args.is_empty()
        && !show_filter_type
        && diff_filter_args.is_empty()
        && !show_line_type
        && !show_component
    {
        show_line_type = true;
        show_component = true;
    }

    infos
        .iter()
        .map(|info| {
            let mut parts: Vec<String> = Vec::new();

            if show_precision {
                parts.push(info.precision.clone());
            }

            // Series: name and args grouped together (bug fix: args were floating solo before)
            let mut s_args = fmt_args(&info.series_args, &diff_series_args);
            if show_series_x {
                let x_str = format!("x={}", info.series_x);
                s_args = match s_args {
                    Some(mut args) => {
                        args.insert_str(1, &format!("{}, ", x_str));
                        Some(args)
                    }
                    None => Some(format!("({})", x_str)),
                };
            }
            if show_series_name || s_args.is_some() {
                let mut s = info.series_name.clone();
                if let Some(a) = s_args {
                    s += &format!(" {}", a);
                }
                parts.push(s);
            }

            // Noise
            if let Some((nt, nm, na, ns)) = &info.noise {
                let a = fmt_args(na, &diff_noise_args);
                if show_noise_type || show_noise_meth || show_noise_seed || a.is_some() {
                    let mut s = "Noise".to_string();
                    if show_noise_type {
                        s += &format!(": {}", nt);
                    }
                    if show_noise_meth {
                        s += &format!(" ({})", nm);
                    }
                    if show_noise_seed {
                        s += &format!(" seed={}", ns);
                    }
                    if let Some(a) = a {
                        s += &format!(" {}", a);
                    }
                    parts.push(s);
                }
            } else if show_noise_type {
                parts.push("No Noise".to_string());
            }

            // Accel
            if let Some(an) = &info.accel_name {
                let a = fmt_args(&info.accel_args, &diff_accel_args);
                if show_accel_name || show_accel_m || a.is_some() {
                    let mut s = if show_accel_name {
                        an.clone()
                    } else {
                        "Accel".to_string()
                    };
                    if show_accel_m {
                        s += &format!(" m={}", info.accel_m.unwrap_or(0));
                    }
                    if let Some(a) = a {
                        s += &format!(" {}", a);
                    }
                    parts.push(s);
                }
            } else if show_accel_name {
                parts.push("No Accel".to_string());
            }

            // Filter
            if let Some((ft, fa)) = &info.filter {
                let a = fmt_args(fa, &diff_filter_args);
                if show_filter_type || a.is_some() {
                    let mut s = "Filter".to_string();
                    if show_filter_type {
                        s += &format!(": {}", ft);
                    }
                    if let Some(a) = a {
                        s += &format!(" {}", a);
                    }
                    parts.push(s);
                }
            } else if show_filter_type {
                parts.push("No Filter".to_string());
            }

            if show_line_type {
                parts.push(info.line_type.clone());
            }
            let mut s = parts.join(" | ");
            if show_component {
                if let Some(c) = &info.component {
                    s += &format!(" [{}]", c);
                }
            }
            s
        })
        .collect()
}
