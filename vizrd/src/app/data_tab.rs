use crate::app::ResultKey;
use crate::ffi::Arr;
use std::collections::HashMap;
use std::sync::Arc;

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
pub struct SequenceDisplay {
    pub sn: Arr,
    pub an: Arr,
    pub dev: Arr,
    pub events: HashMap<usize, Vec<String>>,

    pub sn_stats: Vec<ComponentStats>,
    pub an_stats: Vec<ComponentStats>,
    pub dev_stats: Vec<ComponentStats>,

    pub len: usize,
}

#[derive(Debug, Clone)]
pub struct DataNode {
    pub name: String,
    pub sequence: Option<SequenceDisplay>,
    pub children: Vec<DataNode>,
}

#[derive(Default)]
pub struct DataCache {
    pub dirty: bool,
    pub roots: Vec<DataNode>,
}

fn get_arr_headers(arr: &Arr) -> Vec<&'static str> {
    match arr {
        Arr::Real(_) => vec!["Real"],
        Arr::Complex(_) => vec!["Re", "Im", "Mag"],
        Arr::Interval(_) => vec!["Inf", "Sup", "Width"],
        Arr::CInterval(_) => vec!["Re(Inf)", "Re(Sup)", "Im(Inf)", "Im(Sup)"],
    }
}

fn extract_at(arr: &Arr, n: usize) -> Vec<String> {
    match arr {
        Arr::Real(v) => {
            if n < v.len() {
                vec![format!("{:.4e}", v[n].to_f64())]
            } else {
                vec!["-".to_string()]
            }
        }
        Arr::Complex(c) => {
            if n < c.real.len() {
                let re = c.real[n].to_f64();
                let im = c.imag[n].to_f64();
                vec![
                    format!("{:.4e}", re),
                    format!("{:.4e}", im),
                    format!("{:.4e}", re.hypot(im)),
                ]
            } else {
                vec!["-".to_string(), "-".to_string(), "-".to_string()]
            }
        }
        Arr::Interval(i) => {
            if n < i.inf.len() {
                let inf = i.inf[n].to_f64();
                let sup = i.sup[n].to_f64();
                vec![
                    format!("{:.4e}", inf),
                    format!("{:.4e}", sup),
                    format!("{:.4e}", sup - inf),
                ]
            } else {
                vec!["-".to_string(), "-".to_string(), "-".to_string()]
            }
        }
        Arr::CInterval(c) => {
            if n < c.real.inf.len() {
                vec![
                    format!("{:.4e}", c.real.inf[n].to_f64()),
                    format!("{:.4e}", c.real.sup[n].to_f64()),
                    format!("{:.4e}", c.imag.inf[n].to_f64()),
                    format!("{:.4e}", c.imag.sup[n].to_f64()),
                ]
            } else {
                vec![
                    "-".to_string(),
                    "-".to_string(),
                    "-".to_string(),
                    "-".to_string(),
                ]
            }
        }
    }
}

fn create_sequence_display(
    sn: &Arr,
    an: &Arr,
    dev: &Arr,
    events: &[crate::compute::SeriesEvent],
) -> SequenceDisplay {
    let mut ev_map: HashMap<usize, Vec<String>> = HashMap::new();
    for ev in events {
        ev_map
            .entry(ev.n as usize)
            .or_default()
            .push(format!("{}: {}", ev.name, ev.description));
    }

    let len = match sn {
        Arr::Real(v) => v.len(),
        Arr::Complex(c) => c.real.len(),
        Arr::Interval(i) => i.inf.len(),
        Arr::CInterval(c) => c.real.inf.len(),
    };

    SequenceDisplay {
        sn: sn.clone(),
        an: an.clone(),
        dev: dev.clone(),
        events: ev_map,
        sn_stats: compute_arr_stats(sn),
        an_stats: compute_arr_stats(an),
        dev_stats: compute_arr_stats(dev),
        len,
    }
}

impl DataCache {
    pub fn rebuild(
        &mut self,
        series_results: &HashMap<
            crate::compute::SeriesDesc,
            Option<Arc<crate::compute::SeriesData>>,
        >,
        accel_results: &HashMap<ResultKey, Option<Arc<crate::compute::AccelData>>>,
        cancel: std::sync::Arc<std::sync::atomic::AtomicBool>,
    ) {
        if !self.dirty {
            return;
        }

        let mut roots = Vec::new();

        let mut grouped: HashMap<
            crate::compute::SeriesDesc,
            Vec<(&ResultKey, &crate::compute::AccelData)>,
        > = HashMap::new();
        for (k, v) in accel_results {
            if let Some(adata) = v {
                grouped
                    .entry(k.series.clone())
                    .or_default()
                    .push((k, adata));
            }
        }

        for (sdesc, sdata_opt) in series_results {
            if cancel.load(std::sync::atomic::Ordering::Relaxed) {
                return;
            }
            if let Some(sdata) = sdata_opt {
                let mut series_node = DataNode {
                    name: format!("Series: {}", sdesc.series.name),
                    sequence: Some(create_sequence_display(
                        &sdata.result.sn,
                        &sdata.result.an,
                        &sdata.result.deviations,
                        &[],
                    )),
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
                            sequence: Some(create_sequence_display(
                                &adata.result.sn,
                                &adata.result.an,
                                &adata.result.deviations,
                                &adata.events,
                            )),
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
                show_node(ui, root, "");
            }
        });
}

fn show_node(ui: &mut egui::Ui, node: &DataNode, parent_id: &str) {
    let id_source = format!("{}_{}", parent_id, node.name);
    egui::CollapsingHeader::new(&node.name)
        .id_salt(&id_source)
        .default_open(false)
        .show(ui, |ui| {
            if let Some(seq) = &node.sequence {
                show_sequence(ui, seq, &id_source);
            }

            for child in &node.children {
                show_node(ui, child, &id_source);
            }
        });
}

fn show_sequence(ui: &mut egui::Ui, seq: &SequenceDisplay, id_source: &str) {
    // Show overall characteristics first
    egui::CollapsingHeader::new("Characteristics (Stats)")
        .id_salt(format!("{}_stats", id_source))
        .default_open(false)
        .show(ui, |ui| {
            egui::Grid::new(format!("{}_sgrid", id_source))
                .striped(true)
                .show(ui, |ui| {
                    ui.label("Component");
                    ui.label("Min");
                    ui.label("Max");
                    ui.label("Mean");
                    ui.label("Median");
                    ui.end_row();

                    let mut show_stats = |title: &str, stats: &[ComponentStats]| {
                        if !stats.is_empty() {
                            ui.label(egui::RichText::new(title).strong());
                            ui.end_row();
                            for comp in stats {
                                ui.label(format!("  {}", comp.name));
                                ui.label(format!("{:.4e}", comp.stats.min));
                                ui.label(format!("{:.4e}", comp.stats.max));
                                ui.label(format!("{:.4e}", comp.stats.mean));
                                ui.label(format!("{:.4e}", comp.stats.median));
                                ui.end_row();
                            }
                        }
                    };

                    show_stats("Sn (Values)", &seq.sn_stats);
                    show_stats("An (Terms)", &seq.an_stats);
                    show_stats("Deviations", &seq.dev_stats);
                });
        });

    ui.separator();

    let sn_headers = get_arr_headers(&seq.sn);
    let an_headers = get_arr_headers(&seq.an);
    let dev_headers = get_arr_headers(&seq.dev);

    // Build table columns definition
    let mut builder = egui_extras::TableBuilder::new(ui)
        .striped(true)
        .cell_layout(egui::Layout::left_to_right(egui::Align::Center))
        .column(egui_extras::Column::auto().at_least(40.0)); // n

    for _ in &sn_headers {
        builder = builder.column(egui_extras::Column::auto().at_least(80.0));
    }
    for _ in &an_headers {
        builder = builder.column(egui_extras::Column::auto().at_least(80.0));
    }
    for _ in &dev_headers {
        builder = builder.column(egui_extras::Column::auto().at_least(80.0));
    }

    // Events column
    builder = builder.column(egui_extras::Column::remainder().at_least(100.0));

    builder
        .header(20.0, |mut header| {
            header.col(|ui| {
                ui.strong("n");
            });
            for h in &sn_headers {
                header.col(|ui| {
                    ui.strong(format!("Sn {}", h));
                });
            }
            for h in &an_headers {
                header.col(|ui| {
                    ui.strong(format!("An {}", h));
                });
            }
            for h in &dev_headers {
                header.col(|ui| {
                    ui.strong(format!("Dev {}", h));
                });
            }
            header.col(|ui| {
                ui.strong("Events");
            });
        })
        .body(|body| {
            body.rows(20.0, seq.len, |mut row| {
                let n = row.index();

                row.col(|ui| {
                    ui.label(n.to_string());
                });

                for val in extract_at(&seq.sn, n) {
                    row.col(|ui| {
                        ui.label(val);
                    });
                }
                for val in extract_at(&seq.an, n) {
                    row.col(|ui| {
                        ui.label(val);
                    });
                }
                for val in extract_at(&seq.dev, n) {
                    row.col(|ui| {
                        ui.label(val);
                    });
                }

                row.col(|ui| {
                    if let Some(evs) = seq.events.get(&n) {
                        ui.label(evs.join(", "));
                    }
                });
            });
        });
}
