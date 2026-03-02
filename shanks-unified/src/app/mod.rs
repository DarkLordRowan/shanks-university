//! Application state and UI implementation.

mod selection;
mod tree_ui;
mod ui;

use crate::cache::Cache;
use crate::compute::{self, AccelData, ComputeEvent, ComputeTask, SeriesData};
use crate::experiment::{
    AccelInstance, ExperimentConfig, FilterInstance, NoiseInstance, SeriesInstance,
};
use crate::ffi::{Arr, ArrF64, ArrLine, ComplexOf, IntervalOf, Value};
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
    filter: Option<FilterInstance>,
}

/// Key for grouping multiple (accel, filter) combinations under one series compute task.
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
struct GroupKey {
    series: SeriesInstance,
    precision: String,
    noise: Option<NoiseInstance>,
}

#[derive(Debug, PartialEq, Eq, Hash, Copy, Clone, serde::Serialize, serde::Deserialize)]
pub enum PlotTab {
    Main,
    Deviation,
}

#[derive(
    Debug, PartialEq, Eq, Hash, Copy, Clone, Default, serde::Serialize, serde::Deserialize,
)]
pub enum DeviationMode {
    #[default]
    Magnitude,
    Components,
}

#[derive(Debug, Clone, serde::Serialize, serde::Deserialize)]
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

        let filter = if let Some(ref name) = combo.filter_name {
            exp.filters
                .iter()
                .find(|f| f.filter_type == *name)
                .and_then(|f| {
                    f.expand().find(|inst| {
                        combo.filter_args.iter().all(|(k, v)| {
                            inst.args
                                .get(k)
                                .map(|sv| sv.to_string() == *v)
                                .unwrap_or(false)
                        })
                    })
                })
        } else {
            None
        };

        Some(Self {
            series,
            precision: combo.precision.clone(),
            noise,
            accel: Some(accel),
            filter,
        })
    }

    fn color(&self) -> egui::Color32 {
        use std::hash::{Hash, Hasher};
        let mut hasher = std::collections::hash_map::DefaultHasher::new();
        self.hash(&mut hasher);
        let hash = hasher.finish();

        let hue = (hash % 360) as f32 / 360.0;
        egui::ecolor::Hsva::new(hue, 1.0, 1.0, 1.0).into()
    }
}

struct BakedLine {
    data: ArrLine,
    full_name: String, // Full descriptive name for tooltips
    color: egui::Color32,
    width: f32,
    style: LineStyle,
    shading_polygons: Vec<Vec<PlotPoint>>,
    events: Vec<compute::SeriesEvent>,
}

#[derive(Default)]
struct PlotCache {
    lines_main: Vec<BakedLine>,
    lines_deviation: Vec<BakedLine>,
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
    active_tasks: HashMap<GroupKey, JoinHandle<()>>,
    event_rx: tokio::sync::mpsc::Receiver<ComputeEvent<GroupKey>>,
    event_tx: tokio::sync::mpsc::Sender<ComputeEvent<GroupKey>>,

    pub series_tree: Option<SelectionNode>,
    pub accel_tree: Option<SelectionNode>,
    pub noise_tree: Option<SelectionNode>,
    pub filter_tree: Option<SelectionNode>,
    pub precision_tree: Option<SelectionNode>,

    plot_cache: PlotCache,
    pub selected_tab: PlotTab,
    pub main_tab_state: TabState,
    pub dev_tab_state: TabState,

    pub deviation_mode: DeviationMode,
    pub show_sn: bool,
    pub show_accel: bool,
    pub show_partial_sums: bool,
    pub show_accel_values: bool,
    pub show_smoothed_estimates: bool,
    pub show_limit_lines: bool,
    pub show_interval_shading: bool,

    pub status: String,
}

impl ShanksApp {
    pub fn new(state: AppState) -> Self {
        let (event_tx, event_rx) = tokio::sync::mpsc::channel(512);

        let mut app = Self {
            state,
            results: HashMap::new(),
            active_tasks: HashMap::new(),
            event_rx,
            event_tx,
            series_tree: None,
            accel_tree: None,
            noise_tree: None,
            filter_tree: None,
            precision_tree: None,
            plot_cache: PlotCache::default(),
            selected_tab: PlotTab::Main,
            main_tab_state: TabState::default(),
            dev_tab_state: TabState::default(),
            deviation_mode: DeviationMode::Magnitude,
            show_sn: true,
            show_accel: true,
            show_partial_sums: true,
            show_accel_values: true,
            show_smoothed_estimates: true,
            show_limit_lines: true,
            show_interval_shading: true,
            status: "Ready".to_string(),
        };

        app.rebuild_trees();
        app
    }

    fn rebuild_trees(&mut self) {
        if let Some(ref exp) = self.state.experiment {
            let series_instances: Vec<_> = exp.series.iter().flat_map(|s| s.expand()).collect();
            let method_instances: Vec<_> = exp.accels.iter().flat_map(|a| a.expand()).collect();
            let filter_instances: Vec<_> = exp.filters.iter().flat_map(|f| f.expand()).collect();
            let precisions = exp.precisions.clone().unwrap_or_default();

            self.series_tree = Some(selection::build_series_tree(&series_instances));
            self.accel_tree = Some(selection::build_accel_tree(&method_instances));
            self.noise_tree = Some(selection::build_noise_tree(&exp.noises));
            self.filter_tree = Some(selection::build_filter_tree(&filter_instances));
            self.precision_tree = Some(selection::build_precision_tree(&precisions));
        }
    }

    pub fn sync_with_compute(&mut self) {
        let (s_tree, a_tree, n_tree, f_tree, p_tree) = match (
            &self.series_tree,
            &self.accel_tree,
            &self.noise_tree,
            &self.filter_tree,
            &self.precision_tree,
        ) {
            (Some(s), Some(a), Some(n), Some(f), Some(p)) => (s, a, n, f, p),
            _ => return,
        };

        let combinations = selection::generate_combinations(s_tree, a_tree, n_tree, f_tree, p_tree);
        let mut requested_keys = HashSet::new();
        let mut grouped_requests: HashMap<GroupKey, (Vec<AccelInstance>, Vec<FilterInstance>)> =
            HashMap::new();

        for combo in combinations {
            if let Some(key) = ResultKey::from_combo(self, &combo) {
                requested_keys.insert(key.clone());

                let gkey = GroupKey {
                    series: key.series.clone(),
                    precision: key.precision.clone(),
                    noise: key.noise.clone(),
                };

                let (accels, filters) = grouped_requests.entry(gkey).or_default();
                if let Some(ref a) = key.accel {
                    if !accels.contains(a) {
                        accels.push(a.clone());
                    }
                }
                if let Some(ref f) = key.filter {
                    if !filters.contains(f) {
                        filters.push(f.clone());
                    }
                }
            }
        }

        // Spawn tasks for groups that have missing data
        for (gkey, (algos, filters)) in grouped_requests {
            let mut need_recompute = false;
            for algo in &algos {
                // Check if bare algo is missing
                let rkey_bare = ResultKey {
                    series: gkey.series.clone(),
                    precision: gkey.precision.clone(),
                    noise: gkey.noise.clone(),
                    accel: Some(algo.clone()),
                    filter: None,
                };
                if !self.results.contains_key(&rkey_bare) {
                    need_recompute = true;
                    break;
                }

                // Check if filtered versions are missing
                for filter in &filters {
                    let rkey_filt = ResultKey {
                        series: gkey.series.clone(),
                        precision: gkey.precision.clone(),
                        noise: gkey.noise.clone(),
                        accel: Some(algo.clone()),
                        filter: Some(filter.clone()),
                    };
                    if !self.results.contains_key(&rkey_filt) {
                        need_recompute = true;
                        break;
                    }
                }
                if need_recompute {
                    break;
                }
            }

            // Also check if series itself is missing
            if !need_recompute {
                let rkey_series = ResultKey {
                    series: gkey.series.clone(),
                    precision: gkey.precision.clone(),
                    noise: gkey.noise.clone(),
                    accel: None,
                    filter: None,
                };
                if !self.results.contains_key(&rkey_series) {
                    need_recompute = true;
                }
            }

            if need_recompute && !self.active_tasks.contains_key(&gkey) {
                let task = ComputeTask {
                    id: gkey.clone(),
                    precision: gkey.precision.clone(),
                    series: gkey.series.clone(),
                    n_points: self.state.n_points,
                    noise: gkey.noise.clone(),
                    algorithms: algos,
                    filters,
                };

                let tx = self.event_tx.clone();
                let cache = self.state.cache.lock().unwrap().clone();
                let handle = compute::spawn_task(task, cache, tx);
                self.active_tasks.insert(gkey, handle);
                self.status = "Computing...".to_string();
            }
        }

        self.active_tasks.retain(|k, handle| {
            // Check if ANY requested ResultKey matches this GroupKey
            let requested = requested_keys.iter().any(|rk| {
                rk.series == k.series && rk.precision == k.precision && rk.noise == k.noise
            });

            if !requested {
                handle.abort();
                false
            } else {
                true
            }
        });

        let old_size = self.results.len();
        self.results.retain(|k, _| requested_keys.contains(k));
        if self.results.len() != old_size {
            self.plot_cache.dirty = true;
        }
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
                    let rkey = ResultKey {
                        series: id.series.clone(),
                        precision: id.precision.clone(),
                        noise: id.noise.clone(),
                        accel: accel.as_ref().map(|(ad, _)| ad.accel.clone()),
                        filter: accel.as_ref().and_then(|(ad, _)| ad.filter.clone()),
                    };

                    let adata = accel.map(|(_, data)| data);
                    self.results.insert(rkey, (series_data, adata));
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

    fn arr_to_f64(&self, arr: &Arr, symlog: bool, log_linthresh: f64) -> ArrF64 {
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
        &self,
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
                self.arr_to_f64(arr, symlog, log_linthresh),
            ));
        }
        results
    }

    fn process_collected_lines(
        &self,
        raw_lines: Vec<(
            ResultKey,
            String,
            ArrF64,
            egui::Color32,
            f32,
            LineStyle,
            Vec<compute::SeriesEvent>,
        )>,
    ) -> Vec<BakedLine> {
        if raw_lines.is_empty() {
            return Vec::new();
        }

        // 1. Generate all LineInfos for all components
        let mut all_infos = Vec::new();
        for (key, ltype, data, _, _, _, _) in &raw_lines {
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
        let full_names: Vec<String> = all_infos.iter().map(|info| {
            let mut parts = Vec::new();
            parts.push(info.precision.clone());
            parts.push(info.series_name.clone());
            
            let s_args: Vec<_> = info.series_args.iter().map(|(k,v)| format!("{}={}", k, v)).collect();
            if !s_args.is_empty() {
                parts.push(format!("({})", s_args.join(", ")));
            }

            if let Some((ref nt, ref na)) = info.noise {
                let n_args: Vec<_> = na.iter().map(|(k,v)| format!("{}={}", k, v)).collect();
                let mut s = format!("Noise: {}", nt);
                if !n_args.is_empty() {
                    s.push_str(&format!(" ({})", n_args.join(", ")));
                }
                parts.push(s);
            }

            if let Some(ref an) = info.accel_name {
                let a_args: Vec<_> = info.accel_args.iter().map(|(k,v)| format!("{}={}", k, v)).collect();
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
                let f_args: Vec<_> = fa.iter().map(|(k,v)| format!("{}={}", k, v)).collect();
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
        }).collect();

        // 3. Build BakedLines
        let mut baked_lines = Vec::new();
        let mut name_idx = 0;
        for (_key, _ltype, data, color, width, style, events) in raw_lines {
            // Determine full_name (take from the first component of this data)
            let full_name = full_names[name_idx].clone();

            let baked_data = match data {
                ArrF64::Real(v) => {
                    let name = shortened_names[name_idx].clone();
                    name_idx += 1;
                    let pts = v
                        .iter()
                        .enumerate()
                        .map(|(i, &y)| PlotPoint::new(i as f64, y))
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
                        .map(|(i, &y)| PlotPoint::new(i as f64, y))
                        .collect();
                    let im_pts = c
                        .imag
                        .iter()
                        .enumerate()
                        .map(|(i, &y)| PlotPoint::new(i as f64, y))
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
                        .map(|(i, &y)| PlotPoint::new(i as f64, y))
                        .collect();
                    let sup_pts = iv
                        .sup
                        .iter()
                        .enumerate()
                        .map(|(i, &y)| PlotPoint::new(i as f64, y))
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
                        .map(|(i, &y)| PlotPoint::new(i as f64, y))
                        .collect();
                    let re_sup_pts = ci
                        .real
                        .sup
                        .iter()
                        .enumerate()
                        .map(|(i, &y)| PlotPoint::new(i as f64, y))
                        .collect();
                    let im_inf_pts = ci
                        .imag
                        .inf
                        .iter()
                        .enumerate()
                        .map(|(i, &y)| PlotPoint::new(i as f64, y))
                        .collect();
                    let im_sup_pts = ci
                        .imag
                        .sup
                        .iter()
                        .enumerate()
                        .map(|(i, &y)| PlotPoint::new(i as f64, y))
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
            if self.show_interval_shading {
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
            }

            baked_lines.push(BakedLine {
                data: baked_data,
                full_name,
                color,
                width,
                style,
                shading_polygons,
                events,
            });
        }
        baked_lines
    }

    pub fn bake_plot_cache(&mut self) {
        if !self.plot_cache.dirty {
            return;
        }

        self.plot_cache.lines_main.clear();
        self.plot_cache.lines_deviation.clear();

        let mut main_raw = Vec::new();
        let mut dev_raw = Vec::new();

        let main_symlog = self.main_tab_state.symlog;
        let main_thresh = self.main_tab_state.log_linthresh;
        let dev_symlog = self.dev_tab_state.symlog;
        let dev_thresh = self.dev_tab_state.log_linthresh;

        let mut max_n = 0usize;
        for (sdata, adata) in self.results.values() {
            max_n = max_n.max(match &sdata.sn {
                Arr::Real(v) => v.len(),
                Arr::Complex(c) => c.real.len(),
                Arr::Interval(iv) => iv.inf.len(),
                Arr::CInterval(ci) => ci.real.inf.len(),
            });
            if let Some(adata) = adata {
                max_n = max_n.max(match &adata.values {
                    Arr::Real(v) => v.len(),
                    Arr::Complex(c) => c.real.len(),
                    Arr::Interval(iv) => iv.inf.len(),
                    Arr::CInterval(ci) => ci.real.inf.len(),
                });
            }
        }

        for (key, (sdata, adata)) in &self.results {
            let base_color = key.color();

            // Main Plot
            if self.show_sn {
                main_raw.push((
                    key.clone(),
                    "Sn".to_string(),
                    self.arr_to_f64(&sdata.sn, main_symlog, main_thresh),
                    base_color.gamma_multiply(0.4),
                    1.0,
                    LineStyle::Dashed { length: 4.0 },
                    Vec::new(), // Series Sn itself has no events usually
                ));
            }

            if let Some(adata) = adata {
                let is_filtered = key.filter.is_some();
                let show = if is_filtered { self.show_smoothed_estimates } else { self.show_accel };

                if show {
                    main_raw.push((
                        key.clone(),
                        if is_filtered { "Estimated".to_string() } else { "Accel".to_string() },
                        self.arr_to_f64(&adata.values, main_symlog, main_thresh),
                        if is_filtered { egui::Color32::from_rgb(240, 230, 140) } else { base_color }, // Khaki-ish
                        if is_filtered { 3.0 } else { 2.0 },
                        LineStyle::Solid,
                        adata.events.clone(),
                    ));
                }
            }

            // Limits
            if self.show_limit_lines {
                if let Some(ref val) = sdata.sum {
                    let points = match val {
                        Value::Real(rv) => ArrF64::Real(vec![Self::to_plot_point(rv, main_symlog, main_thresh); max_n + 1]),
                        Value::Complex(cv) => ArrF64::Complex(ComplexOf {
                            real: vec![Self::to_plot_point(&cv.real, main_symlog, main_thresh); max_n + 1],
                            imag: vec![Self::to_plot_point(&cv.imag, main_symlog, main_thresh); max_n + 1],
                        }),
                        Value::Interval(iv) => ArrF64::Interval(IntervalOf {
                            inf: vec![Self::to_plot_point(&iv.inf, main_symlog, main_thresh); max_n + 1],
                            sup: vec![Self::to_plot_point(&iv.sup, main_symlog, main_thresh); max_n + 1],
                        }),
                        Value::CInterval(ci) => ArrF64::CInterval(ComplexOf {
                            real: IntervalOf {
                                inf: vec![Self::to_plot_point(&ci.real.inf, main_symlog, main_thresh); max_n + 1],
                                sup: vec![Self::to_plot_point(&ci.real.sup, main_symlog, main_thresh); max_n + 1],
                            },
                            imag: IntervalOf {
                                inf: vec![Self::to_plot_point(&ci.imag.inf, main_symlog, main_thresh); max_n + 1],
                                sup: vec![Self::to_plot_point(&ci.imag.sup, main_symlog, main_thresh); max_n + 1],
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
                    ));
                }
            }

            // Deviation Plot
            if self.show_partial_sums {
                let collected = self.collect_deviation_data(
                    key,
                    "Sn Dev",
                    &sdata.deviations,
                    self.deviation_mode,
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
                        LineStyle::Dashed { length: 4.0 },
                        Vec::new(),
                    ));
                }
            }

            if let Some(adata) = adata {
                if self.show_accel_values {
                    let collected = self.collect_deviation_data(
                        key,
                        "Accel Dev",
                        &adata.deviations,
                        self.deviation_mode,
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
                        ));
                    }
                }
            }
        }

        self.plot_cache.lines_main = self.process_collected_lines(main_raw);
        self.plot_cache.lines_deviation = self.process_collected_lines(dev_raw);

        self.plot_cache.dirty = false;
    }
}

impl eframe::App for ShanksApp {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        self.process_events();
        self.sync_with_compute();
        self.bake_plot_cache();

        egui::TopBottomPanel::top("top_panel").show(ctx, |ui| {
            egui::menu::bar(ui, |ui| {
                ui.menu_button("File", |ui| {
                    if ui.button("Clear Cache").clicked() {
                        let cache = self.state.cache.lock().unwrap().clone();
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
                    if ui.checkbox(&mut self.show_sn, "Show Sn").changed() {
                        self.plot_cache.dirty = true;
                    }
                    if ui.checkbox(&mut self.show_accel, "Show Accel").changed() {
                        self.plot_cache.dirty = true;
                    }
                    ui.separator();
                    if ui
                        .checkbox(&mut self.show_partial_sums, "Show Partial Sums")
                        .changed()
                    {
                        self.plot_cache.dirty = true;
                    }
                    if ui
                        .checkbox(&mut self.show_accel_values, "Show Accelerated Deviations")
                        .changed()
                    {
                        self.plot_cache.dirty = true;
                    }
                    if ui
                        .checkbox(&mut self.show_limit_lines, "Show Series Limits")
                        .changed()
                    {
                        self.plot_cache.dirty = true;
                    }
                    if ui
                        .checkbox(&mut self.show_smoothed_estimates, "Show Smoothed Estimates")
                        .changed()
                    {
                        self.plot_cache.dirty = true;
                    }
                    if ui
                        .checkbox(&mut self.show_interval_shading, "Show Interval Shading")
                        .changed()
                    {
                        self.plot_cache.dirty = true;
                    }
                });
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
                    if let Some(ref mut t) = self.filter_tree {
                        tree_ui::draw_tree_with_header(ui, "Filters", t, false);
                    }
                    if let Some(ref mut t) = self.precision_tree {
                        tree_ui::draw_tree_with_header(ui, "Precisions", t, true);
                    }
                });
            });

        egui::CentralPanel::default().show(ctx, |ui| {
            ui.horizontal(|ui| {
                ui.selectable_value(&mut self.selected_tab, PlotTab::Main, "Main Plot");
                ui.selectable_value(&mut self.selected_tab, PlotTab::Deviation, "Deviations");

                ui.separator();

                let tab_state = match self.selected_tab {
                    PlotTab::Main => &mut self.main_tab_state,
                    PlotTab::Deviation => &mut self.dev_tab_state,
                };

                if ui.checkbox(&mut tab_state.symlog, "Symlog").changed() {
                    self.plot_cache.dirty = true;
                }
                if tab_state.symlog {
                    let slider = egui::Slider::new(&mut tab_state.log_linthresh, -100.0..=100.0)
                        .text("Log Threshold")
                        .clamping(egui::SliderClamping::Never);
                    if ui.add(slider).changed() {
                        self.plot_cache.dirty = true;
                    }
                }

                ui.separator();
                let ar_slider = egui::Slider::new(&mut tab_state.aspect_ratio, 0.1..=100.0)
                    .text("Aspect Ratio")
                    .logarithmic(true);
                ui.add(ar_slider);

                if ui.button("Home").clicked() {
                    tab_state.reset_view = true;
                }

                if self.selected_tab == PlotTab::Deviation {
                    ui.separator();
                    ui.label("Dev Mode:");
                    if ui
                        .radio_value(&mut self.deviation_mode, DeviationMode::Magnitude, "Mag")
                        .changed()
                    {
                        self.plot_cache.dirty = true;
                    }
                    if ui
                        .radio_value(&mut self.deviation_mode, DeviationMode::Components, "Comp")
                        .changed()
                    {
                        self.plot_cache.dirty = true;
                    }
                }
            });

            let (plot_lines, current_tab_state) = match self.selected_tab {
                PlotTab::Main => (&self.plot_cache.lines_main, &mut self.main_tab_state),
                PlotTab::Deviation => (&self.plot_cache.lines_deviation, &mut self.dev_tab_state),
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

            let name_to_full: HashMap<String, String> = plot_lines.iter().flat_map(|baked| {
                match &baked.data {
                    ArrLine::Real((n, _)) => vec![(n.clone(), baked.full_name.clone())],
                    ArrLine::Complex(c) => vec![(c.real.0.clone(), baked.full_name.clone()), (c.imag.0.clone(), baked.full_name.clone())],
                    ArrLine::Interval(iv) => vec![(iv.inf.0.clone(), baked.full_name.clone()), (iv.sup.0.clone(), baked.full_name.clone())],
                    ArrLine::CInterval(ci) => vec![
                        (ci.real.inf.0.clone(), baked.full_name.clone()),
                        (ci.real.sup.0.clone(), baked.full_name.clone()),
                        (ci.imag.inf.0.clone(), baked.full_name.clone()),
                        (ci.imag.sup.0.clone(), baked.full_name.clone()),
                    ],
                }
            }).collect();

            let symlog = current_tab_state.symlog;
            let thresh = current_tab_state.log_linthresh;

            plot = plot.label_formatter(move |name, value| {
                if name.is_empty() {
                    return format!("n = {:.0}\ny = {:.4e}", value.x, value.y);
                }
                let full = name_to_full.get(name).cloned().unwrap_or_else(|| name.to_string());
                let y_str = if symlog {
                    crate::plot::symlog_formatter(value.y, thresh)
                } else {
                    format!("{:.10e}", value.y)
                };
                format!("{}\nn = {:.0}\ny = {}", full, value.x, y_str)
            });

            plot.show(ui, |plot_ui| {
                for baked in plot_lines {
                    for poly_pts in &baked.shading_polygons {
                        plot_ui.polygon(
                            egui_plot::Polygon::new(PlotPoints::Borrowed(poly_pts))
                                .fill_color(baked.color.gamma_multiply(0.2))
                                .stroke(egui::Stroke::NONE),
                        );
                    }

                    match &baked.data {
                        ArrLine::Real((name, v)) => {
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(v))
                                    .name(name)
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                            );
                        }
                        ArrLine::Complex(c) => {
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(&c.real.1))
                                    .name(&c.real.0)
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                            );
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(&c.imag.1))
                                    .name(&c.imag.0)
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                            );
                        }
                        ArrLine::Interval(iv) => {
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(&iv.inf.1))
                                    .name(&iv.inf.0)
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                            );
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(&iv.sup.1))
                                    .name(&iv.sup.0)
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                            );
                        }
                        ArrLine::CInterval(ci) => {
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(&ci.real.inf.1))
                                    .name(&ci.real.inf.0)
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                            );
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(&ci.real.sup.1))
                                    .name(&ci.real.sup.0)
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                            );
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(&ci.imag.inf.1))
                                    .name(&ci.imag.inf.0)
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                            );
                            plot_ui.line(
                                Line::new(PlotPoints::Borrowed(&ci.imag.sup.1))
                                    .name(&ci.imag.sup.0)
                                    .color(baked.color)
                                    .width(baked.width)
                                    .style(baked.style),
                            );
                        }
                    }

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
            });
        });

        if !self.active_tasks.is_empty() || self.plot_cache.dirty {
            ctx.request_repaint();
        }
    }
}

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
struct LineInfo {
    precision: String,
    series_name: String,
    series_args: BTreeMap<String, String>,
    noise: Option<(String, BTreeMap<String, String>)>,
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
            .args
            .iter()
            .map(|(k, v)| (k.clone(), v.to_string()))
            .collect();
        let noise = key.noise.as_ref().map(|n| {
            (
                n.noise_type.clone(),
                n.args
                    .iter()
                    .map(|(k, v)| (k.clone(), v.to_string()))
                    .collect(),
            )
        });
        let accel_name = key.accel.as_ref().map(|a| a.name.clone());
        let accel_m = key.accel.as_ref().map(|a| a.m);
        let accel_args = key
            .accel
            .as_ref()
            .map(|a| {
                a.args
                    .iter()
                    .map(|(k, v)| (k.clone(), v.to_string()))
                    .collect()
            })
            .unwrap_or_default();
        let filter = key.filter.as_ref().map(|f| {
            (
                f.filter_type.clone(),
                f.args
                    .iter()
                    .map(|(k, v)| (k.clone(), v.to_string()))
                    .collect(),
            )
        });

        Self {
            precision: key.precision.clone(),
            series_name: key.series.name.clone(),
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
        show_noise_type |=
            infos[i].noise.as_ref().map(|(t, _)| t) != infos[0].noise.as_ref().map(|(t, _)| t);
        show_accel_name |= infos[i].accel_name != infos[0].accel_name;
        show_accel_m |= infos[i].accel_m != infos[0].accel_m;
        show_filter_type |=
            infos[i].filter.as_ref().map(|(t, _)| t) != infos[0].filter.as_ref().map(|(t, _)| t);
        show_line_type |= infos[i].line_type != infos[0].line_type;
        show_component |= infos[i].component != infos[0].component;
    }

    let diff_series_args = find_diff_keys(infos, |inf| &inf.series_args);
    let diff_noise_args = find_diff_keys_opt(infos, |inf| &inf.noise);
    let diff_accel_args = find_diff_keys(infos, |inf| &inf.accel_args);
    let diff_filter_args = find_diff_keys_opt(infos, |inf| &inf.filter);

    // If everything is perfectly identical, show at least names/types to distinguish
    if !show_precision
        && !show_series_name
        && diff_series_args.is_empty()
        && !show_noise_type
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
            if let Some((ref nt, ref na)) = info.noise {
                let mut n_args = Vec::new();
                for key in na.keys() {
                    if diff_noise_args.contains(key) {
                        n_args.push(format!("{}={}", key, na[key]));
                    }
                }
                if show_noise_type || !n_args.is_empty() {
                    let mut s = "Noise".to_string();
                    if show_noise_type {
                        s.push_str(&format!(": {}", nt));
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
