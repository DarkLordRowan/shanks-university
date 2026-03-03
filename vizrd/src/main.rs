//! Vizrd - Series Acceleration Visualization Tool
//!
//! This application provides a unified interface for:
//! - Computing series partial sums and terms
//! - Applying acceleration algorithms
//! - Visualizing results with interactive plots
//! - Caching results in SQLite database
//!
//! # Modes
//!
//! - **GUI mode** (default): Launch the interactive visualization UI
//! - **Headless mode** (`--headless`): Run all computations from config without UI

use clap::{Parser, Subcommand};
use std::path::PathBuf;

/// Vizrd - Series Acceleration Visualization Tool
#[derive(Parser, Debug)]
#[command(name = "vizrd")]
#[command(about = "A unified tool for series acceleration and visualization")]
#[command(version)]
struct Args {
    /// Path to experiment configuration file (Drag & Drop / GUI mode)
    config: Option<PathBuf>,

    /// Path to the SQLite database file
    #[arg(short, long, default_value = "vizrd.db")]
    db_path: PathBuf,

    /// Enable verbose logging
    #[arg(short, long, action = clap::ArgAction::Count)]
    verbose: u8,

    /// Disable database caching
    #[arg(long)]
    no_cache: bool,

    #[command(subcommand)]
    command: Option<Commands>,
}

#[derive(Subcommand, Debug)]
enum Commands {
    /// Run in GUI mode
    Gui {
        /// Path to experiment configuration file
        config: PathBuf,
    },

    /// Run in headless mode - compute all series from config
    Headless {
        /// Path to experiment configuration file
        config: PathBuf,

        /// Comma-separated list of precisions to use
        #[arg(long)]
        precisions: Option<String>,

        /// Path to export results in Parquet format
        #[arg(short, long)]
        export: Option<PathBuf>,
    },

    /// List available series, algorithms, and precisions
    List {
        /// What to list: series, accels, precisions, noises
        #[arg(default_value = "all")]
        what: String,
    },
}

#[tokio::main]
async fn main() -> anyhow::Result<()> {
    let args = Args::parse();

    // Initialize logging
    let log_level = match args.verbose {
        0 => log::LevelFilter::Warn,
        1 => log::LevelFilter::Info,
        2 => log::LevelFilter::Debug,
        _ => log::LevelFilter::Trace,
    };
    env_logger::Builder::new()
        .filter_module("vizrd", log_level)
        .init();

    log::info!("Starting Vizrd...");

    // Initialize database
    let cache = if args.no_cache {
        log::info!("Cache disabled by --no-cache flag");
        vizrd::cache::Cache::disabled()
    } else {
        log::info!("Initializing database at {:?}", args.db_path);
        vizrd::cache::Cache::new(&args.db_path).await?
    };

    match &args.command {
        // Drag & Drop or GUI without subcommand
        None => {
            if let Some(config) = &args.config {
                run_gui(cache, Some(config.clone()))
            } else {
                // No command and no config - print help
                use clap::CommandFactory;
                Args::command().print_help()?;
                println!();
                Ok(())
            }
        }
        Some(Commands::Gui { config }) => run_gui(cache, Some(config.clone())),
        Some(Commands::Headless {
            config,
            precisions,
            export,
        }) => {
            run_headless(
                cache,
                config.clone(),
                precisions.clone(),
                export.clone(),
                args.verbose > 0,
            )
            .await
        }
        Some(Commands::List { what }) => run_list(what),
    }
}

fn run_gui(cache: vizrd::cache::Cache, config_path: Option<PathBuf>) -> anyhow::Result<()> {
    // Load experiment config if provided
    let experiment_config = if let Some(path) = config_path {
        log::info!("Loading experiment config from {:?}", path);
        Some(vizrd::experiment::ExperimentConfig::load(&path)?)
    } else {
        None
    };

    // Create application state
    let app_state = vizrd::app::AppState::new(experiment_config, cache);

    // Run GUI
    let native_options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default()
            .with_inner_size([1200.0, 800.0])
            .with_min_inner_size([800.0, 600.0])
            .with_title("Vizrd"),
        ..Default::default()
    };

    let result = eframe::run_native(
        "Vizrd",
        native_options,
        Box::new(move |cc| {
            // Set up egui style
            let mut style = (*cc.egui_ctx.style()).clone();
            style.spacing.item_spacing = egui::vec2(8.0, 6.0);
            cc.egui_ctx.set_style(style);

            Ok(Box::new(vizrd::app::ShanksApp::new(app_state)))
        }),
    );

    if let Err(e) = result {
        log::error!("GUI error: {}", e);
        return Err(anyhow::anyhow!("GUI error: {}", e));
    }

    Ok(())
}

async fn run_headless(
    cache: vizrd::cache::Cache,
    config_path: PathBuf,
    precisions: Option<String>,
    export_path: Option<PathBuf>,
    verbose: bool,
) -> anyhow::Result<()> {
    log::info!("Running in headless mode");

    // Load experiment config
    let mut config = vizrd::experiment::ExperimentConfig::load(&config_path)?;

    // Override precisions if specified
    if let Some(p) = precisions {
        config.precisions = Some(p.split(',').map(|s| s.trim().to_string()).collect());
    }

    // Create headless runner
    let mut runner = vizrd::headless::HeadlessRunner::new(config, cache, export_path)?;

    // Set up progress callback
    let last_print =
        std::sync::Arc::new(std::sync::Mutex::new((std::time::Instant::now(), -1_i32)));
    runner = runner.with_progress(move |info| {
        let now = std::time::Instant::now();
        let mut state = last_print.lock().unwrap();

        let percent = (info.current as f64 / info.total as f64 * 100.0) as i32;
        let should_print = if verbose {
            now.duration_since(state.0).as_millis() > 100 || info.current == info.total
        } else {
            percent >= state.1 + 5 || info.current == info.total
        };

        if should_print {
            state.0 = now;
            state.1 = percent;
            use std::io::Write;

            let status = match info.status {
                vizrd::headless::Status::Computing => "Computing",
                vizrd::headless::Status::Complete => "Complete ",
                vizrd::headless::Status::Error(_) => "Error    ",
            };

            print!(
                "\r[{}/{}] ({}%) {} ({}) - {} - {} [{:.1}s]\x1B[K",
                info.current,
                info.total,
                percent,
                info.series_name,
                info.precision,
                info.method_name,
                status,
                info.elapsed_secs
            );

            if !verbose && info.current != info.total {
                println!();
            } else if info.current == info.total {
                println!();
            } else {
                let _ = std::io::stdout().flush();
            }
        }
    });

    // Run all computations
    let summary = runner.run_all().await?;

    // Print summary
    println!("\n=== Run Summary ===");
    println!("Total trials: {}", summary.total_trials);
    println!("Successful: {}", summary.successful);
    println!("Failed: {}", summary.failed);
    println!("Total time: {:.2}s", summary.total_time_secs);

    if !summary.errors.is_empty() {
        println!("\nErrors:");
        for err in &summary.errors {
            println!("  - {}", err);
        }
    }

    Ok(())
}

fn run_list(what: &str) -> anyhow::Result<()> {
    use vizrd::ffi::bridge::ffi as bridge;
    let what = what.to_lowercase();

    if what == "all" || what == "series" {
        println!("Available series:");
        for name in bridge::list_series() {
            println!("  - {}", name);
        }
        println!();
    }

    if what == "all" || what == "accels" || what == "algorithms" || what == "methods" {
        println!("Available acceleration algorithms:");
        for name in bridge::list_accels() {
            println!("  - {}", name);
        }
        println!();
    }

    if what == "all" || what == "precisions" {
        println!("Available precisions:");
        for name in bridge::list_precisions() {
            println!("  - {}", name);
        }
        println!();
    }

    if what == "all" || what == "noises" {
        println!("Available noise types:");
        for name in bridge::list_noises() {
            println!("  - {}", name);
        }
        println!();

        println!("Available noise methods:");
        for name in bridge::list_noise_methods() {
            println!("  - {}", name);
        }
        println!();
    }

    Ok(())
}
