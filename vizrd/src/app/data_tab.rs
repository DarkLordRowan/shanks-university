use crate::app::ResultKey;
use crate::ffi::Arr;

#[derive(Debug, Clone, PartialEq)]
pub struct ArrStats {
    pub min: f64,
    pub max: f64,
    pub mean: f64,
    pub median: f64,
}

#[derive(Debug, Clone)]
pub struct ComponentStats {
    pub name: String,
    pub stats: ArrStats,
}

#[derive(Debug, Clone)]
pub struct DataNode {
    pub name: String,
    pub components: Vec<ComponentStats>,
    pub children: Vec<DataNode>,
}

#[derive(Default)]
pub struct DataCache {
    pub dirty: bool,
    pub roots: Vec<DataNode>,
}

impl DataCache {
    pub fn rebuild(
        &mut self,
        series_results: &std::collections::HashMap<
            crate::compute::SeriesDesc,
            Option<crate::compute::SeriesData>,
        >,
        accel_results: &std::collections::HashMap<ResultKey, Option<crate::compute::AccelData>>,
    ) {
        if !self.dirty {
            return;
        }

        let mut roots = Vec::new();

        // Group keys by series
        let mut grouped: std::collections::HashMap<
            crate::compute::SeriesDesc,
            Vec<(&ResultKey, &crate::compute::AccelData)>,
        > = std::collections::HashMap::new();
        for (k, v) in accel_results {
            if let Some(adata) = v {
                grouped
                    .entry(k.series.clone())
                    .or_default()
                    .push((k, adata));
            }
        }

        for (sdesc, sdata_opt) in series_results {
            if let Some(sdata) = sdata_opt {
                let mut series_node = DataNode {
                    name: format!("Series: {}", sdesc.series.name),
                    components: compute_arr_stats(&sdata.result.values),
                    children: Vec::new(),
                };

                if let Some(accels) = grouped.get(sdesc) {
                    for (k, adata) in accels {
                        let name = if let Some(a) = &k.accel {
                            if let Some(f) = &a.filter {
                                format!("Filter: {} (on {})", f.filter_type, a.accel.name)
                            } else {
                                format!("Accel: {}", a.accel.name)
                            }
                        } else {
                            "Unknown".to_string()
                        };

                        series_node.children.push(DataNode {
                            name,
                            components: compute_arr_stats(&adata.result.values),
                            children: Vec::new(),
                        });
                    }
                }

                roots.push(series_node);
            }
        }

        self.roots = roots;
        self.dirty = false;
    }
}

fn compute_arr_stats(arr: &Arr) -> Vec<ComponentStats> {
    match arr {
        Arr::Real(v) => {
            let vals: Vec<f64> = v.iter().map(|rv| rv.to_f64()).collect();
            vec![ComponentStats {
                name: "Real".to_string(),
                stats: calc_stats(&vals),
            }]
        }
        Arr::Complex(c) => {
            let re: Vec<f64> = c.real.iter().map(|v| v.to_f64()).collect();
            let im: Vec<f64> = c.imag.iter().map(|v| v.to_f64()).collect();
            let mag: Vec<f64> = re.iter().zip(&im).map(|(r, i)| r.hypot(*i)).collect();
            vec![
                ComponentStats {
                    name: "Re".to_string(),
                    stats: calc_stats(&re),
                },
                ComponentStats {
                    name: "Im".to_string(),
                    stats: calc_stats(&im),
                },
                ComponentStats {
                    name: "Mag".to_string(),
                    stats: calc_stats(&mag),
                },
            ]
        }
        Arr::Interval(iv) => {
            let inf: Vec<f64> = iv.inf.iter().map(|v| v.to_f64()).collect();
            let sup: Vec<f64> = iv.sup.iter().map(|v| v.to_f64()).collect();
            let width: Vec<f64> = sup.iter().zip(&inf).map(|(s, i)| s - i).collect();
            vec![
                ComponentStats {
                    name: "Inf".to_string(),
                    stats: calc_stats(&inf),
                },
                ComponentStats {
                    name: "Sup".to_string(),
                    stats: calc_stats(&sup),
                },
                ComponentStats {
                    name: "Width".to_string(),
                    stats: calc_stats(&width),
                },
            ]
        }
        Arr::CInterval(ci) => {
            let re_inf: Vec<f64> = ci.real.inf.iter().map(|v| v.to_f64()).collect();
            let re_sup: Vec<f64> = ci.real.sup.iter().map(|v| v.to_f64()).collect();
            let im_inf: Vec<f64> = ci.imag.inf.iter().map(|v| v.to_f64()).collect();
            let im_sup: Vec<f64> = ci.imag.sup.iter().map(|v| v.to_f64()).collect();
            vec![
                ComponentStats {
                    name: "Re(Inf)".to_string(),
                    stats: calc_stats(&re_inf),
                },
                ComponentStats {
                    name: "Re(Sup)".to_string(),
                    stats: calc_stats(&re_sup),
                },
                ComponentStats {
                    name: "Im(Inf)".to_string(),
                    stats: calc_stats(&im_inf),
                },
                ComponentStats {
                    name: "Im(Sup)".to_string(),
                    stats: calc_stats(&im_sup),
                },
            ]
        }
    }
}

fn calc_stats(vals: &[f64]) -> ArrStats {
    if vals.is_empty() {
        return ArrStats {
            min: 0.0,
            max: 0.0,
            mean: 0.0,
            median: 0.0,
        };
    }

    let mut min = vals[0];
    let mut max = vals[0];
    let mut sum = 0.0;

    let mut valid_vals = Vec::with_capacity(vals.len());

    for &v in vals {
        if v.is_nan() || v.is_infinite() {
            continue;
        }
        if v < min {
            min = v;
        }
        if v > max {
            max = v;
        }
        sum += v;
        valid_vals.push(v);
    }

    let count = valid_vals.len() as f64;
    let mean = if count > 0.0 { sum / count } else { 0.0 };

    let median = if valid_vals.is_empty() {
        0.0
    } else {
        valid_vals.sort_by(|a, b| a.partial_cmp(b).unwrap_or(std::cmp::Ordering::Equal));
        let mid = valid_vals.len() / 2;
        if valid_vals.len() % 2 == 0 {
            (valid_vals[mid - 1] + valid_vals[mid]) / 2.0
        } else {
            valid_vals[mid]
        }
    };

    ArrStats {
        min,
        max,
        mean,
        median,
    }
}

pub fn show(ui: &mut egui::Ui, cache: &DataCache) {
    egui::ScrollArea::both()
        .auto_shrink([false, false])
        .show(ui, |ui| {
            for root in &cache.roots {
                show_node(ui, root);
            }
        });
}

fn show_node(ui: &mut egui::Ui, node: &DataNode) {
    egui::CollapsingHeader::new(&node.name)
        .default_open(false)
        .show(ui, |ui| {
            if !node.components.is_empty() {
                egui::Grid::new(format!("grid_{}", node.name))
                    .striped(true)
                    .show(ui, |ui| {
                        ui.label("Component");
                        ui.label("Min");
                        ui.label("Max");
                        ui.label("Mean");
                        ui.label("Median");
                        ui.end_row();

                        for comp in &node.components {
                            ui.label(&comp.name);
                            ui.label(format!("{:.4e}", comp.stats.min));
                            ui.label(format!("{:.4e}", comp.stats.max));
                            ui.label(format!("{:.4e}", comp.stats.mean));
                            ui.label(format!("{:.4e}", comp.stats.median));
                            ui.end_row();
                        }
                    });
            }

            for child in &node.children {
                show_node(ui, child);
            }
        });
}
