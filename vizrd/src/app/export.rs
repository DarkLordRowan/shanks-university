use crate::app::data_tab::DataCache;
use crate::ffi::Arr;
use std::fs;
use std::io::Write;
use std::path::PathBuf;

// TODO: INCLUDE SYMLOG DATA?
pub fn perform_export(data: &DataCache) -> std::io::Result<()> {
    let now = chrono::Local::now().format("%Y%m%d_%H%M%S");
    let export_dir = PathBuf::from(format!("article_export_{}", now));

    if !export_dir.exists() {
        fs::create_dir_all(&export_dir)?;
    }

    let csv_file_name = "series_data.csv";
    let csv_path = export_dir.join(csv_file_name);

    let mut csv_file = fs::File::create(&csv_path)?;

    // 1. Collect lines: For each root (Series) and its children (Accels), extract Sn (the main series).
    let mut columns: Vec<(String, Vec<f64>)> = Vec::new();

    fn extract_sn(arr: &Arr) -> Vec<f64> {
        match arr {
            Arr::Real(v) => v.iter().map(|x| x.to_f64()).collect(),
            Arr::Complex(c) => c
                .real
                .iter()
                .zip(c.imag.iter())
                .map(|(r, i)| r.to_f64().hypot(i.to_f64()))
                .collect(), // Take magnitude for complex
            Arr::Interval(i) => i
                .inf
                .iter()
                .zip(i.sup.iter())
                .map(|(inf, sup)| (inf.to_f64() + sup.to_f64()) / 2.0)
                .collect(),
            Arr::CInterval(c) => c
                .real
                .inf
                .iter()
                .zip(c.imag.inf.iter())
                .map(|(re, im)| re.to_f64().hypot(im.to_f64()))
                .collect(), // Simplified mag
        }
    }

    fn clean_name(name: &str) -> String {
        name.replace("Series: ", "")
            .replace("Accel: ", "")
            .split(" (")
            .next()
            .unwrap_or(name)
            .to_string()
    }

    for root in &data.roots {
        let root_clean = clean_name(&root.name);
        if let Some(seq) = &root.sequence {
            columns.push((root_clean.clone(), extract_sn(&seq.sn)));
        }
        for child in &root.children {
            if let Some(seq) = &child.sequence {
                let child_clean = clean_name(&child.name);
                columns.push((
                    format!("{} + {}", root_clean, child_clean),
                    extract_sn(&seq.sn),
                ));
            }
        }
    }

    // 2. Write CSV Header
    write!(csv_file, "n")?;
    for (name, _) in &columns {
        let clean_name = name.replace("\"", "\"\"");
        write!(csv_file, ",\"{}\"", clean_name)?;
    }
    writeln!(csv_file)?;

    // 3. Write CSV Body
    let max_len = columns.iter().map(|(_, v)| v.len()).max().unwrap_or(0);
    for n in 0..max_len {
        write!(csv_file, "{}", n)?;
        for (_, col) in &columns {
            if n < col.len() {
                write!(csv_file, ",{}", col[n])?;
            } else {
                write!(csv_file, ",")?;
            }
        }
        writeln!(csv_file)?;
    }

    Ok(())
}
