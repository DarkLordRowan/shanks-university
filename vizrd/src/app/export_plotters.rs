use crate::app::BakedLine;
use egui::Color32;
use plotters::prelude::*;
use std::path::Path;

#[derive(Clone)]
pub struct ExportLineConfig {
    pub id: String,
    pub custom_name: String,
    pub color: Color32,
    pub width: f32,
    pub visible: bool,
    pub style: usize, // 0 solid, 1 dashed, 2 dotted
}

#[derive(Clone)]
pub struct ExportSettings {
    pub line_configs: std::collections::HashMap<String, ExportLineConfig>,
    pub axis_labels: (String, String),
    pub legend_pos: usize, // 0: TopLeft, 1: TopRight, 2: BottomLeft, 3: BottomRight, 4: Outside
    pub legend_font_size: u32,
    pub x_min: f64,
    pub x_max: f64,
    pub y_min: f64,
    pub y_max: f64,
}

pub fn get_baked_line_id(baked: &BakedLine) -> String {
    use crate::ffi::ArrLine;
    match &baked.data {
        ArrLine::Real((n, _)) => n.clone(),
        ArrLine::Complex(c) => format!("Re: {}, Im: {}", c.real.0, c.imag.0),
        ArrLine::Interval(iv) => format!("Inf: {}, Sup: {}", iv.inf.0, iv.sup.0),
        ArrLine::CInterval(ci) => format!("CI: {}", ci.real.inf.0),
    }
}

pub fn render_to_buffer(
    config: &ExportSettings,
    live_lines: &[BakedLine],
    width: u32,
    height: u32,
    symlog: bool,
    log_linthresh: f64,
) -> Result<Vec<u8>, Box<dyn std::error::Error>> {
    let mut buffer = vec![0; (width * height * 3) as usize];
    {
        let root = BitMapBackend::with_buffer(&mut buffer, (width, height)).into_drawing_area();
        draw_chart(&root, config, live_lines, symlog, log_linthresh)?;
        root.present()?;
    }
    Ok(buffer)
}

pub fn export_to_jpg(
    path: &Path,
    config: &ExportSettings,
    live_lines: &[BakedLine],
    width: u32,
    height: u32,
    symlog: bool,
    log_linthresh: f64,
) -> Result<(), Box<dyn std::error::Error>> {
    let root = BitMapBackend::new(path, (width, height)).into_drawing_area();
    draw_chart(&root, config, live_lines, symlog, log_linthresh)?;
    root.present()?;
    Ok(())
}

fn draw_chart<DB: DrawingBackend>(
    root: &DrawingArea<DB, plotters::coord::Shift>,
    config: &ExportSettings,
    live_lines: &[BakedLine],
    symlog: bool,
    log_linthresh: f64,
) -> Result<(), Box<dyn std::error::Error>>
where
    DB::ErrorType: 'static,
{
    // Fill background
    root.fill(&plotters::style::WHITE)
        .map_err(|_| "Fill error")?;

    let min_x = config.x_min;
    let max_x = config.x_max.max(min_x + 1e-6); // Prevent zero-range
    let min_y = config.y_min;
    let max_y = config.y_max.max(min_y + 1e-6);

    let mut chart = ChartBuilder::on(root)
        .margin(30)
        .x_label_area_size(40)
        .y_label_area_size(50)
        .build_cartesian_2d(min_x..max_x, min_y..max_y)
        .map_err(|_| "Chart error")?;

    let mut mesh = chart.configure_mesh();
    mesh.x_desc(config.axis_labels.0.clone())
        .y_desc(config.axis_labels.1.clone());

    let symlog_fmt = |y: &f64| crate::plot::symlog_grid_formatter(*y, log_linthresh);
    let normal_fmt = |y: &f64| crate::plot::format_grid_value(*y);

    if symlog {
        mesh.y_label_formatter(&symlog_fmt);
        mesh.draw().map_err(|_| "Mesh error")?;
    } else {
        mesh.y_label_formatter(&normal_fmt);
        mesh.draw().map_err(|_| "Mesh error")?;
    }

    for baked in live_lines {
        let id = get_baked_line_id(baked);
        let Some(cfg) = config.line_configs.get(&id) else {
            continue;
        };

        if !cfg.visible {
            continue;
        }

        let color = plotters::style::RGBColor(cfg.color.r(), cfg.color.g(), cfg.color.b());

        let pts_iter = extract_pts_iter(baked);
        let filtered = pts_iter
            .filter(|(x, y)| *x >= min_x && *x <= max_x && *y >= min_y && *y <= max_y);

        let stroke_width = cfg.width as u32;

        let series = plotters::series::LineSeries::new(filtered, color.stroke_width(stroke_width));
        chart
            .draw_series(series)
            .map_err(|_| "Series error")?
            .label(cfg.custom_name.clone())
            .legend(move |(x, y)| {
                plotters::element::PathElement::new(
                    vec![(x, y), (x + 20, y)],
                    color.stroke_width(stroke_width),
                )
            });
    }

    chart
        .configure_series_labels()
        .background_style(&plotters::style::WHITE.mix(0.8))
        .border_style(&plotters::style::BLACK)
        .position(plotters::chart::SeriesLabelPosition::UpperRight)
        .draw()
        .map_err(|_| "Label error")?;

    Ok(())
}

fn extract_pts_iter<'a>(line: &'a BakedLine) -> Box<dyn Iterator<Item = (f64, f64)> + 'a> {
    use crate::ffi::ArrLine;
    match &line.data {
        ArrLine::Real((_, v)) => Box::new(v.iter().map(|p| (p.x, p.y))),
        ArrLine::Complex(c) => Box::new(c.real.1.iter().map(|p| (p.x, p.y))),
        ArrLine::Interval(iv) => Box::new(iv.inf.1.iter().map(|p| (p.x, p.y))),
        ArrLine::CInterval(ci) => Box::new(ci.real.inf.1.iter().map(|p| (p.x, p.y))),
    }
}
