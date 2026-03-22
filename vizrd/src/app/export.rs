use crate::app::export_plotters::ExportSettings;
use crate::app::BakedLine;
use crate::ffi::ArrLine;
use std::collections::HashMap;
use std::fs;
use std::io::Write;
use std::path::PathBuf;

pub fn perform_export(
    lines: &[BakedLine],
    settings: &ExportSettings,
) -> Result<(), Box<dyn std::error::Error>> {
    let now = chrono::Local::now().format("%Y%m%d_%H%M%S");
    let export_dir = PathBuf::from(format!("article_export_{}", now));

    if !export_dir.exists() {
        fs::create_dir_all(&export_dir)?;
    }

    let csv_file_name = "series_data.csv";
    let csv_path = export_dir.join(csv_file_name);

    let mut csv_file = fs::File::create(&csv_path)?;

    // Build a map from line ID to config for quick lookup
    let config_map: HashMap<String, &crate::app::export_plotters::ExportLineConfig> = settings
        .line_configs
        .iter()
        .map(|(k, v)| (k.clone(), v))
        .collect();

    // Collect visible lines with their custom names and data
    let mut columns: Vec<(String, Vec<(f64, f64)>)> = Vec::new();

    for line in lines {
        let line_id = crate::app::export_plotters::get_baked_line_id(line);

        // Check if line is visible in export settings
        if let Some(config) = config_map.get(&line_id) {
            if !config.visible {
                continue;
            }

            // Extract points from the line based on its data type
            let points = extract_points_from_line(line);
            if !points.is_empty() {
                columns.push((config.custom_name.clone(), points));
            }
        }
    }

    if columns.is_empty() {
        return Err("No visible lines to export".into());
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
        // Get x value from first column (assuming all columns have same x values)
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

fn extract_points_from_line(line: &BakedLine) -> Vec<(f64, f64)> {
    match &line.data {
        ArrLine::Real((_, pts)) => pts.iter().map(|p| (p.x, p.y)).collect(),
        ArrLine::Complex(c) => c.real.1.iter().map(|p| (p.x, p.y)).collect(),
        ArrLine::Interval(iv) => iv.inf.1.iter().map(|p| (p.x, p.y)).collect(),
        ArrLine::CInterval(ci) => ci.real.inf.1.iter().map(|p| (p.x, p.y)).collect(),
    }
}
