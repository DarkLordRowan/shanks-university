use crate::app::{BakedLine, Grid};
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
    pub style: usize,       // 0 solid, 1 dashed, 2 dotted
    pub marker_type: usize, // 0 none, 1 circle, 2 square, 3 triangle, 4 cross
    pub marker_size: u32,   // default 4
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
    grid: Grid,
) -> Result<Vec<u8>, Box<dyn std::error::Error>> {
    let mut buffer = vec![0; (width * height * 3) as usize];
    {
        let root = BitMapBackend::with_buffer(&mut buffer, (width, height)).into_drawing_area();
        draw_chart(&root, config, live_lines, grid)?;
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
    grid: Grid,
) -> Result<(), Box<dyn std::error::Error>> {
    let root = BitMapBackend::new(path, (width, height)).into_drawing_area();
    draw_chart(&root, config, live_lines, grid)?;
    root.present()?;
    Ok(())
}

fn draw_chart<DB: DrawingBackend>(
    root: &DrawingArea<DB, plotters::coord::Shift>,
    config: &ExportSettings,
    live_lines: &[BakedLine],
    grid: Grid,
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

    let symlog_fmt;
    match grid {
        Grid::Normal => mesh.y_label_formatter(&|y: &f64| crate::plot::format_grid_value(*y)),
        Grid::Symlog { log_linthresh } => {
            symlog_fmt = move |y: &f64| crate::plot::symlog_grid_formatter(*y, log_linthresh);
            mesh.y_label_formatter(&symlog_fmt)
        }
    };
    mesh.draw().map_err(|_| "Mesh error")?;

    for baked in live_lines {
        let id = get_baked_line_id(baked);
        let Some(cfg) = config.line_configs.get(&id) else {
            continue;
        };

        if !cfg.visible {
            continue;
        }

        let color = plotters::style::RGBColor(cfg.color.r(), cfg.color.g(), cfg.color.b());

        let stroke_width = cfg.width as u32;

        let in_bounds =
            |(x, y): &(f64, f64)| *x >= min_x && *x <= max_x && *y >= min_y && *y <= max_y;

        let series = plotters::series::LineSeries::new(
            extract_pts_iter(baked).filter(in_bounds),
            color.stroke_width(stroke_width),
        );
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

        if cfg.marker_type > 0 {
            let marker_color = color.filled();
            let marker_size = cfg.marker_size;
            let pts = extract_pts_iter(baked).filter(in_bounds);

            match cfg.marker_type {
                1 => {
                    chart
                        .draw_series(plotters::series::PointSeries::of_element(
                            pts,
                            marker_size,
                            marker_color,
                            &|coord, size, style| {
                                plotters::element::Circle::new(coord, size, style)
                            },
                        ))
                        .map_err(|_| "Marker error")?;
                }
                2 => {
                    chart
                        .draw_series(plotters::series::PointSeries::of_element(
                            pts,
                            marker_size,
                            marker_color,
                            &|coord, size, style| {
                                let mut rect = plotters::element::Rectangle::new(
                                    [(coord.0, coord.1), (coord.0, coord.1)],
                                    style,
                                );
                                rect.set_margin(size, size, size, size);
                                rect
                            },
                        ))
                        .map_err(|_| "Marker error")?;
                }
                3 => {
                    chart
                        .draw_series(plotters::series::PointSeries::of_element(
                            pts,
                            marker_size,
                            marker_color,
                            &|coord, size, style| {
                                plotters::element::TriangleMarker::new(coord, size, style)
                            },
                        ))
                        .map_err(|_| "Marker error")?;
                }
                4 => {
                    chart
                        .draw_series(plotters::series::PointSeries::of_element(
                            pts,
                            marker_size,
                            marker_color,
                            &|coord, size, style| plotters::element::Cross::new(coord, size, style),
                        ))
                        .map_err(|_| "Marker error")?;
                }
                _ => {}
            }
        }
    }

    chart
        .configure_series_labels()
        .background_style(&plotters::style::WHITE.mix(0.8))
        .border_style(&plotters::style::BLACK)
        .position(plotters::chart::SeriesLabelPosition::UpperRight)
        .label_font(("sans-serif", config.legend_font_size).into_font())
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
