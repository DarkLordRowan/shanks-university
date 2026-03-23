use crate::app::{BakedLine, Grid};
use crate::ffi::ArrLine;
use egui_plot::PlotBounds;
use serde::Serialize;
use std::fs;
use std::io::Write;
use std::path::PathBuf;

pub fn perform_export(lines: &[BakedLine]) -> Result<(), Box<dyn std::error::Error>> {
    let now = chrono::Local::now().format("%Y%m%d_%H%M%S");
    let csv_path = PathBuf::from(format!("series_data_{}.csv", now));

    let mut csv_file = fs::File::create(&csv_path)?;

    // Collect visible lines with their custom names and data
    let mut columns: Vec<(String, Vec<(f64, f64)>)> = Vec::new();

    for line in lines {
        for (name, pts) in extract_columns(line) {
            if !pts.is_empty() {
                columns.push((name, pts));
            }
        }
    }

    if columns.is_empty() {
        return Err("No lines to export".into());
    }

    // Write CSV Header
    write!(csv_file, "n")?;
    for (name, _) in &columns {
        let clean_name = name.replace("\"", "\"\"");
        write!(csv_file, ",\"{}\"", clean_name)?;
    }
    writeln!(csv_file)?;

    // Find max length
    let max_len = columns.iter().map(|(_, v)| v.len()).max().unwrap_or(0);

    // Write CSV Body
    for idx in 0..max_len {
        let x_val = columns
            .first()
            .and_then(|(_, pts)| pts.get(idx))
            .map(|(x, _)| *x as usize)
            .unwrap_or(idx);

        write!(csv_file, "{}", x_val)?;
        for (_, col) in &columns {
            if idx < col.len() {
                write!(csv_file, ",{}", col[idx].1)?;
            } else {
                write!(csv_file, ",")?;
            }
        }
        writeln!(csv_file)?;
    }

    Ok(())
}

#[derive(Serialize)]
struct ExportData {
    lines: Vec<ExportLine>,
    grid: GridConfig,
    bounds: BoundsConfig,
}

#[derive(Serialize)]
#[serde(tag = "type")]
enum GridConfig {
    Normal,
    Symlog { log_linthresh: f64 },
}

#[derive(Serialize)]
struct BoundsConfig {
    x_min: f64,
    x_max: f64,
    y_min: f64,
    y_max: f64,
}

#[derive(Serialize)]
struct ExportLine {
    name: String,
    color: [u8; 4],
    width: f32,
    style: String,
    x: Vec<f64>,
    y: Vec<f64>,
}

pub fn perform_export_json(
    lines: &[BakedLine],
    grid: Grid,
    bounds: PlotBounds,
) -> Result<(), Box<dyn std::error::Error>> {
    let now = chrono::Local::now().format("%Y%m%d_%H%M%S");
    let json_path = PathBuf::from(format!("series_data_{}.json", now));
    let json_file = fs::File::create(&json_path)?;

    let mut export_lines = Vec::new();

    for line in lines {
        let color = [
            line.color.r(),
            line.color.g(),
            line.color.b(),
            line.color.a(),
        ];
        let width = line.width;
        let style = match line.style {
            crate::app::LineStyle::Solid => "Solid",
            crate::app::LineStyle::Dashed { .. } => "Dashed",
            crate::app::LineStyle::Dotted { .. } => "Dotted",
        }
        .to_string();

        for (name, pts) in extract_columns(line) {
            if !pts.is_empty() {
                let x: Vec<f64> = pts.iter().map(|p| p.0).collect();
                let y: Vec<f64> = pts.iter().map(|p| p.1).collect();
                export_lines.push(ExportLine {
                    name,
                    color,
                    width,
                    style: style.clone(),
                    x,
                    y,
                });
            }
        }
    }

    if export_lines.is_empty() {
        return Err("No lines to export".into());
    }

    let grid_config = match grid {
        Grid::Normal => GridConfig::Normal,
        Grid::Symlog { log_linthresh } => GridConfig::Symlog { log_linthresh },
    };

    let bounds_config = BoundsConfig {
        x_min: bounds.min()[0],
        x_max: bounds.max()[0],
        y_min: bounds.min()[1],
        y_max: bounds.max()[1],
    };

    let data = ExportData {
        lines: export_lines,
        grid: grid_config,
        bounds: bounds_config,
    };
    serde_json::to_writer_pretty(json_file, &data)?;

    let viewer_path = if PathBuf::from("plot_viewer.py").exists() {
        "plot_viewer.py"
    } else if PathBuf::from("vizrd/plot_viewer.py").exists() {
        "vizrd/plot_viewer.py"
    } else {
        "plot_viewer.py"
    };

    if let Err(e) = std::process::Command::new("python3")
        .arg(viewer_path)
        .arg(&json_path)
        .spawn()
    {
        log::error!("Failed to launch {}: {}", viewer_path, e);
    }

    Ok(())
}

fn extract_columns(line: &BakedLine) -> Vec<(String, Vec<(f64, f64)>)> {
    match &line.data {
        ArrLine::Real((name, pts)) => {
            vec![(name.clone(), pts.iter().map(|p| (p.x, p.y)).collect())]
        }
        ArrLine::Complex(c) => vec![
            (
                c.real.0.clone(),
                c.real.1.iter().map(|p| (p.x, p.y)).collect(),
            ),
            (
                c.imag.0.clone(),
                c.imag.1.iter().map(|p| (p.x, p.y)).collect(),
            ),
        ],
        ArrLine::Interval(iv) => vec![
            (
                iv.inf.0.clone(),
                iv.inf.1.iter().map(|p| (p.x, p.y)).collect(),
            ),
            (
                iv.sup.0.clone(),
                iv.sup.1.iter().map(|p| (p.x, p.y)).collect(),
            ),
        ],
        ArrLine::CInterval(ci) => vec![
            (
                ci.real.inf.0.clone(),
                ci.real.inf.1.iter().map(|p| (p.x, p.y)).collect(),
            ),
            (
                ci.real.sup.0.clone(),
                ci.real.sup.1.iter().map(|p| (p.x, p.y)).collect(),
            ),
            (
                ci.imag.inf.0.clone(),
                ci.imag.inf.1.iter().map(|p| (p.x, p.y)).collect(),
            ),
            (
                ci.imag.sup.0.clone(),
                ci.imag.sup.1.iter().map(|p| (p.x, p.y)).collect(),
            ),
        ],
    }
}
