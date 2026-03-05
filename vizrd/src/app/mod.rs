//! Application state and UI implementation.

pub mod coordinator;
pub mod data_tab;
mod selection;
mod ui;

use crate::cache::Cache;
use crate::compute::{self, AccelData, IsCancelled, SeriesData};
use crate::experiment::ExperimentConfig;
use crate::ffi::{Arr, ArrF64, ArrLine, ComplexOf, IntervalOf, Value};
use arc_swap::ArcSwap;
use egui_plot::{Line, LineStyle, PlotPoint, PlotPoints};
use std::collections::{BTreeMap, HashMap, HashSet};
use std::sync::Arc;
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

#[derive(Debug, Clone, serde::Serialize, serde::Deserialize, PartialEq)]
pub struct TabState {
    pub symlog: bool,
    pub log_linthresh: f64,
    pub aspect_ratio: f32,
    #[serde(skip)]
    pub reset_view: bool,
}

impl Default for TabState {
    fn default() -> Self {
        Self {
            symlog: false,
            log_linthresh: -50.0,
            aspect_ratio: 10.0,
            reset_view: false,
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
struct BakedLine {
    data: ArrLine,
    full_name: String, // Full descriptive name for tooltips
    color: egui::Color32,
    width: f32,
    style: LineStyle,
    shading_polygons: Vec<Vec<PlotPoint>>,
    events: Vec<compute::SeriesEvent>,
    kind: LineKind,
}

#[derive(Default, Clone)]
pub struct PlotCache {
    lines_main: Vec<BakedLine>,
    lines_deviation: Vec<BakedLine>,
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

    n_points: u64,
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
            upd_data: false,
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
            n_points,
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
            upd_data: self.selected_tab == PlotTab::Data,
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

        // 2. Shorten all infos and prepare full names
        let shortened_names = shorten_line_infos(&all_infos);
        let full_names: Vec<String> = all_infos
            .iter()
            .map(|info| {
                let mut parts = Vec::new();
                parts.push(info.precision.clone());
                parts.push(info.series_name.clone());

                let s_args: Vec<_> = info
                    .series_args
                    .iter()
                    .map(|(k, v)| format!("{}={}", k, v))
                    .collect();
                if !s_args.is_empty() {
                    parts.push(format!("({})", s_args.join(", ")));
                }

                if let Some((ref nt, ref nm, ref na, ref ns)) = info.noise {
                    let n_args: Vec<_> = na.iter().map(|(k, v)| format!("{}={}", k, v)).collect();
                    let mut s = format!("Noise: {} ({})", nt, nm);
                    if !n_args.is_empty() {
                        s.push_str(&format!(" ({})", n_args.join(", ")));
                    }
                    s.push_str(&format!(" seed={}", ns));
                    parts.push(s);
                }

                if let Some(ref an) = info.accel_name {
                    let a_args: Vec<_> = info
                        .accel_args
                        .iter()
                        .map(|(k, v)| format!("{}={}", k, v))
                        .collect();
                    let mut s = format!("Accel: {}", an);
                    if let Some(m) = info.accel_m {
                        s.push_str(&format!(" m={}", m));
                    }
                    if !a_args.is_empty() {
                        s.push_str(&format!(" ({})", a_args.join(", ")));
                    }
                    parts.push(s);
                }

                if let Some((ref ft, ref fa)) = info.filter {
                    let f_args: Vec<_> = fa.iter().map(|(k, v)| format!("{}={}", k, v)).collect();
                    let mut s = format!("Filter: {}", ft);
                    if !f_args.is_empty() {
                        s.push_str(&format!(" ({})", f_args.join(", ")));
                    }
                    parts.push(s);
                }

                parts.push(info.line_type.clone());
                let mut s = parts.join(" | ");
                if let Some(ref c) = info.component {
                    s.push_str(&format!(" [{}]", c));
                }
                s
            })
            .collect();

        // 3. Build BakedLines
        let mut baked_lines = Vec::new();
        let mut name_idx = 0;
        for (_key, _ltype, data, color, width, style, events, offset, kind) in raw_lines {
            // Determine full_name (take from the first component of this data)
            let full_name = full_names[name_idx].clone();

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
                full_name,
                color,
                width,
                style,
                shading_polygons,
                events,
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

        let main_symlog = main_tab_state.symlog;
        let main_thresh = main_tab_state.log_linthresh;
        let dev_symlog = dev_tab_state.symlog;
        let dev_thresh = dev_tab_state.log_linthresh;

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
                1.0,
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
            lines_deviation: Self::process_collected_lines(dev_raw),
        }
    }
}

impl eframe::App for ShanksApp {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        egui::TopBottomPanel::top("top_panel").show(ctx, |ui| {
            egui::menu::bar(ui, |ui| {
                ui.menu_button("File", |ui| {
                    if ui.button("Clear Cache").clicked() {
                        let cache = self.cache.clone();
                        tokio::spawn(async move {
                            if let Err(e) = cache.clear_all().await {
                                log::error!("Failed to clear cache: {}", e);
                            }
                        });
                    }
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
                });
                ui.label(format!("Status: {}", *self.status_rx.borrow()));
            });
        });

        egui::SidePanel::left("left_panel")
            .resizable(true)
            .show(ctx, |ui| {
                egui::ScrollArea::vertical().show(ui, |ui| {
                    let mut trees_changed = false;
                    if let Some(app_select) = &mut self.app_select {
                        trees_changed = app_select.draw(ui);
                    }
                    if trees_changed {
                        self.trigger_combinations();
                    }
                    ui.separator();
                    ui.label("N:");
                    let n = egui::Slider::new(&mut self.n_points, 0..=1000)
                        .clamping(egui::SliderClamping::Never);
                    if ui.add(n).changed() {
                        self.trigger_config_update();
                    }
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

                        if ui.checkbox(&mut tab_state.symlog, "Symlog").changed() {
                            config_changed = true;
                        }
                        if tab_state.symlog {
                            ui.label("Log Linthresh: e^");
                            let slider =
                                egui::Slider::new(&mut tab_state.log_linthresh, -100.0..=100.0)
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
            let (plot_lines, current_tab_state) = match self.selected_tab {
                PlotTab::Main => (&cache_lock.lines_main, &mut self.main_tab_state),
                PlotTab::Deviation => (&cache_lock.lines_deviation, &mut self.dev_tab_state),
                PlotTab::Data => unreachable!(),
            };

            let mut plot = egui_plot::Plot::new("main_plot")
                .legend(egui_plot::Legend::default())
                .data_aspect(current_tab_state.aspect_ratio);

            if current_tab_state.symlog {
                let thresh = current_tab_state.log_linthresh;
                plot = plot.y_axis_formatter(move |grid_mark, _range| {
                    crate::plot::symlog_formatter(grid_mark.value, thresh)
                });
            }

            if current_tab_state.reset_view {
                plot = plot.reset();
                current_tab_state.reset_view = false;
            }

            let name_to_full: HashMap<String, String> = plot_lines
                .iter()
                .flat_map(|baked| match &baked.data {
                    ArrLine::Real((n, _)) => vec![(n.clone(), baked.full_name.clone())],
                    ArrLine::Complex(c) => vec![
                        (c.real.0.clone(), baked.full_name.clone()),
                        (c.imag.0.clone(), baked.full_name.clone()),
                    ],
                    ArrLine::Interval(iv) => vec![
                        (iv.inf.0.clone(), baked.full_name.clone()),
                        (iv.sup.0.clone(), baked.full_name.clone()),
                    ],
                    ArrLine::CInterval(ci) => vec![
                        (ci.real.inf.0.clone(), baked.full_name.clone()),
                        (ci.real.sup.0.clone(), baked.full_name.clone()),
                        (ci.imag.inf.0.clone(), baked.full_name.clone()),
                        (ci.imag.sup.0.clone(), baked.full_name.clone()),
                    ],
                })
                .collect();

            let symlog = current_tab_state.symlog;
            let thresh = current_tab_state.log_linthresh;

            plot = plot.label_formatter(move |name, value| {
                if name.is_empty() {
                    return format!("n = {:.0}\ny = {:.4e}", value.x, value.y);
                }
                let full = name_to_full
                    .get(name)
                    .cloned()
                    .unwrap_or_else(|| name.to_string());
                let y_str = if symlog {
                    crate::plot::symlog_formatter(value.y, thresh)
                } else {
                    format!("{:.10e}", value.y)
                };
                format!("{}\nn = {:.0}\ny = {}", full, value.x, y_str)
            });

            fn limit_pts_slice<'a>(pts: &'a [PlotPoint], max_n: u64) -> &'a [PlotPoint] {
                &pts[0..(max_n as usize).min(pts.len())]
            }

            plot.show(ui, |plot_ui| {
                for baked in plot_lines {
                    // Filter by View toggles — no cache rebuild needed
                    match baked.kind {
                        LineKind::Sn if !self.show_sn => continue,
                        LineKind::An if !self.show_an => continue,
                        LineKind::Limit if !self.show_limit_lines => continue,
                        _ => {}
                    }

                    if self.show_interval_shading {
                        for poly_pts in &baked.shading_polygons {
                            plot_ui.polygon(
                                egui_plot::Polygon::new(PlotPoints::Borrowed(limit_pts_slice(
                                    poly_pts,
                                    self.n_points,
                                )))
                                .fill_color(baked.color.gamma_multiply(0.2))
                                .stroke(egui::Stroke::NONE),
                            );
                        }
                    }

                    match &baked.data {
                        ArrLine::Real((name, v)) => {
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(limit_pts_slice(v, self.n_points)))
                                    .name(name)
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                            );
                        }
                        ArrLine::Complex(c) => {
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(limit_pts_slice(
                                    &c.real.1,
                                    self.n_points,
                                )))
                                .name(&c.real.0)
                                .color(baked.color)
                                .width(baked.width)
                                .style(baked.style),
                            );
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(limit_pts_slice(
                                    &c.imag.1,
                                    self.n_points,
                                )))
                                .name(&c.imag.0)
                                .color(baked.color)
                                .width(baked.width)
                                .style(baked.style),
                            );
                        }
                        ArrLine::Interval(iv) => {
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(limit_pts_slice(
                                    &iv.inf.1,
                                    self.n_points,
                                )))
                                .name(&iv.inf.0)
                                .color(baked.color)
                                .width(baked.width)
                                .style(baked.style),
                            );
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(limit_pts_slice(
                                    &iv.sup.1,
                                    self.n_points,
                                )))
                                .name(&iv.sup.0)
                                .color(baked.color)
                                .width(baked.width)
                                .style(baked.style),
                            );
                        }
                        ArrLine::CInterval(ci) => {
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(limit_pts_slice(
                                    &ci.real.inf.1,
                                    self.n_points,
                                )))
                                .name(&ci.real.inf.0)
                                .color(baked.color)
                                .width(baked.width)
                                .style(baked.style),
                            );
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(limit_pts_slice(
                                    &ci.real.sup.1,
                                    self.n_points,
                                )))
                                .name(&ci.real.sup.0)
                                .color(baked.color)
                                .width(baked.width)
                                .style(baked.style),
                            );
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(limit_pts_slice(
                                    &ci.imag.inf.1,
                                    self.n_points,
                                )))
                                .name(&ci.imag.inf.0)
                                .color(baked.color)
                                .width(baked.width)
                                .style(baked.style),
                            );
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(limit_pts_slice(
                                    &ci.imag.sup.1,
                                    self.n_points,
                                )))
                                .name(&ci.imag.sup.0)
                                .color(baked.color)
                                .width(baked.width)
                                .style(baked.style),
                            );
                        }
                    }

                    // TODO: Cache.
                    if self.show_events {
                        // Draw events as markers
                        // Group events by name + description to minimize drawing calls and legend clutter
                        let mut grouped_events: BTreeMap<(String, String), Vec<PlotPoint>> =
                            BTreeMap::new();
                        for ev in &baked.events {
                            if ev.n == 0 {
                                continue;
                            }
                            let idx = ev.n as usize - 1;
                            let pts = match &baked.data {
                                ArrLine::Real((_, v)) => {
                                    v.get(idx).map(|p| vec![*p]).unwrap_or_default()
                                }
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
                            };
                            if !pts.is_empty() {
                                grouped_events
                                    .entry((ev.name.clone(), ev.description.clone()))
                                    .or_default()
                                    .extend(pts);
                            }
                        }

                        for ((name, desc), pts) in grouped_events {
                            use egui_plot::MarkerShape;
                            let (shape, color) = match name.as_str() {
                                "stop" => (MarkerShape::Square, egui::Color32::RED),
                                "algo_error" => {
                                    (MarkerShape::Cross, egui::Color32::from_rgb(255, 140, 0))
                                } // Orange
                                _ => (MarkerShape::Circle, egui::Color32::YELLOW),
                            };

                            let points = egui_plot::Points::new(PlotPoints::Owned(pts))
                                .name(format!("{}: {}", name, desc))
                                .shape(shape)
                                .color(color)
                                .radius(5.0);

                            plot_ui.points(points);
                        }
                    }
                }
            });
        });

        ctx.request_repaint();
    }
}

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
struct LineInfo {
    precision: String,
    series_name: String,
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

    let mut show_precision = false;
    let mut show_series_name = false;
    let mut show_noise_type = false;
    let mut show_noise_method = false;
    let mut show_noise_seed = false;
    let mut show_accel_name = false;
    let mut show_accel_m = false;
    let mut show_filter_type = false;
    let mut show_line_type = false;
    let mut show_component = false;

    // Helper to find differing keys in maps
    let find_diff_keys =
        |infos: &[LineInfo], get_map: fn(&LineInfo) -> &BTreeMap<String, String>| {
            let mut all_keys = HashSet::new();
            for info in infos {
                for key in get_map(info).keys() {
                    all_keys.insert(key);
                }
            }
            let mut diff_keys = HashSet::new();
            for key in all_keys {
                let first_val = get_map(&infos[0]).get(key);
                for i in 1..infos.len() {
                    if get_map(&infos[i]).get(key) != first_val {
                        diff_keys.insert(key.clone());
                        break;
                    }
                }
            }
            diff_keys
        };
    // Helper for noise (4-tuple)
    let find_diff_keys_opt_noise = |infos: &[LineInfo]| {
        let mut all_keys = HashSet::new();
        for info in infos {
            if let Some((_, _, map, _)) = &info.noise {
                for key in map.keys() {
                    all_keys.insert(key);
                }
            }
        }
        let mut diff_keys = HashSet::new();
        for key in all_keys {
            let first_val = infos[0].noise.as_ref().and_then(|(_, _, m, _)| m.get(key));
            for i in 1..infos.len() {
                let current_val = infos[i].noise.as_ref().and_then(|(_, _, m, _)| m.get(key));
                if current_val != first_val {
                    diff_keys.insert(key.clone());
                    break;
                }
            }
        }
        diff_keys
    };

    // Helper for optional maps (noise, filter, etc.)
    let find_diff_keys_opt =
        |infos: &[LineInfo],
         get_opt: fn(&LineInfo) -> &Option<(String, BTreeMap<String, String>)>| {
            let mut all_keys = HashSet::new();
            for info in infos {
                if let Some((_, map)) = get_opt(info) {
                    for key in map.keys() {
                        all_keys.insert(key);
                    }
                }
            }
            let mut diff_keys = HashSet::new();
            for key in all_keys {
                let first_val = get_opt(&infos[0]).as_ref().and_then(|(_, m)| m.get(key));
                for i in 1..infos.len() {
                    let current_val = get_opt(&infos[i]).as_ref().and_then(|(_, m)| m.get(key));
                    if current_val != first_val {
                        diff_keys.insert(key.clone());
                        break;
                    }
                }
            }
            diff_keys
        };

    for i in 1..infos.len() {
        show_precision |= infos[i].precision != infos[0].precision;
        show_series_name |= infos[i].series_name != infos[0].series_name;
        show_noise_type |= infos[i].noise.as_ref().map(|(t, _, _, _)| t)
            != infos[0].noise.as_ref().map(|(t, _, _, _)| t);
        show_noise_method |= infos[i].noise.as_ref().map(|(_, m, _, _)| m)
            != infos[0].noise.as_ref().map(|(_, m, _, _)| m);
        show_noise_seed |= infos[i].noise.as_ref().map(|(_, _, _, s)| s)
            != infos[0].noise.as_ref().map(|(_, _, _, s)| s);
        show_accel_name |= infos[i].accel_name != infos[0].accel_name;
        show_accel_m |= infos[i].accel_m != infos[0].accel_m;
        show_filter_type |=
            infos[i].filter.as_ref().map(|(t, _)| t) != infos[0].filter.as_ref().map(|(t, _)| t);
        show_line_type |= infos[i].line_type != infos[0].line_type;
        show_component |= infos[i].component != infos[0].component;
    }

    let diff_series_args = find_diff_keys(infos, |inf| &inf.series_args);
    let diff_noise_args = find_diff_keys_opt_noise(infos);
    let diff_accel_args = find_diff_keys(infos, |inf| &inf.accel_args);
    let diff_filter_args = find_diff_keys_opt(infos, |inf| &inf.filter);

    // If everything is perfectly identical, show at least names/types to distinguish
    if !show_precision
        && !show_series_name
        && diff_series_args.is_empty()
        && !show_noise_type
        && !show_noise_method
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
            let mut parts = Vec::new();
            if show_precision {
                parts.push(info.precision.clone());
            }
            if show_series_name {
                parts.push(info.series_name.clone());
            }

            // Series Args
            let mut s_args = Vec::new();
            for key in info.series_args.keys() {
                if diff_series_args.contains(key) {
                    s_args.push(format!("{}={}", key, info.series_args[key]));
                }
            }
            if !s_args.is_empty() {
                parts.push(format!("({})", s_args.join(", ")));
            }

            // Noise
            if let Some((ref nt, ref nm, ref na, ref ns)) = info.noise {
                let mut n_args = Vec::new();
                for key in na.keys() {
                    if diff_noise_args.contains(key) {
                        n_args.push(format!("{}={}", key, na[key]));
                    }
                }
                if show_noise_type || show_noise_method || show_noise_seed || !n_args.is_empty() {
                    let mut s = "Noise".to_string();
                    if show_noise_type {
                        s.push_str(&format!(": {}", nt));
                    }
                    if show_noise_method {
                        s.push_str(&format!(" ({})", nm));
                    }
                    if show_noise_seed {
                        s.push_str(&format!(" seed={}", ns));
                    }
                    if !n_args.is_empty() {
                        s.push_str(&format!(" ({})", n_args.join(", ")));
                    }
                    parts.push(s);
                }
            } else if show_noise_type {
                parts.push("No Noise".to_string());
            }

            // Accel
            if let Some(ref an) = info.accel_name {
                let mut a_args = Vec::new();
                for key in info.accel_args.keys() {
                    if diff_accel_args.contains(key) {
                        a_args.push(format!("{}={}", key, info.accel_args[key]));
                    }
                }
                if show_accel_name || show_accel_m || !a_args.is_empty() {
                    let mut s = String::new();
                    if show_accel_name {
                        s.push_str(an);
                    } else {
                        s.push_str("Accel");
                    }
                    if show_accel_m {
                        s.push_str(&format!(" m={}", info.accel_m.unwrap_or(0)));
                    }
                    if !a_args.is_empty() {
                        s.push_str(&format!(" ({})", a_args.join(", ")));
                    }
                    parts.push(s);
                }
            } else if show_accel_name {
                parts.push("No Accel".to_string());
            }

            // Filter
            if let Some((ref ft, ref fa)) = info.filter {
                let mut f_args = Vec::new();
                for key in fa.keys() {
                    if diff_filter_args.contains(key) {
                        f_args.push(format!("{}={}", key, fa[key]));
                    }
                }
                if show_filter_type || !f_args.is_empty() {
                    let mut s = "Filter".to_string();
                    if show_filter_type {
                        s.push_str(&format!(": {}", ft));
                    }
                    if !f_args.is_empty() {
                        s.push_str(&format!(" ({})", f_args.join(", ")));
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
                if let Some(ref c) = info.component {
                    s.push_str(&format!(" [{}]", c));
                }
            }
            s
        })
        .collect()
}
