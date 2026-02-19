//! Configuration management with hot-reload support.

pub mod experiment;

pub use experiment::*;

use anyhow::Result;
use serde::{Deserialize, Serialize};
use std::path::{Path, PathBuf};
use std::sync::{Arc, RwLock};

/// Application configuration (simplified - experiment config is separate).
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AppConfig {
    /// Path to the C++ library
    pub library_path: Option<PathBuf>,

    /// Default precision for computations
    #[serde(default = "default_precision")]
    pub default_precision: String,

    /// Enable profiling by default
    #[serde(default)]
    pub enable_profiling: bool,

    /// UI settings
    #[serde(default)]
    pub ui: UiConfig,

    /// Computation settings
    #[serde(default)]
    pub compute: ComputeConfig,
}

fn default_precision() -> String {
    "F64".to_string()
}

impl Default for AppConfig {
    fn default() -> Self {
        Self {
            library_path: None,
            default_precision: default_precision(),
            enable_profiling: false,
            ui: UiConfig::default(),
            compute: ComputeConfig::default(),
        }
    }
}

/// UI configuration.
#[derive(Debug, Clone, Serialize, Deserialize, Default)]
pub struct UiConfig {
    /// Show partial sums by default
    #[serde(default = "default_true")]
    pub show_partial_sums: bool,

    /// Show limits by default
    #[serde(default = "default_true")]
    pub show_limits: bool,

    /// Use symlog scale for plots
    #[serde(default)]
    pub use_symlog: bool,

    /// Plot theme
    #[serde(default = "default_theme")]
    pub theme: String,
}

fn default_true() -> bool {
    true
}

fn default_theme() -> String {
    "dark".to_string()
}

/// Computation configuration.
#[derive(Debug, Clone, Serialize, Deserialize, Default)]
pub struct ComputeConfig {
    /// Number of parallel workers
    #[serde(default = "default_workers")]
    pub workers: usize,

    /// Cache results automatically
    #[serde(default = "default_true")]
    pub auto_cache: bool,

    /// Timeout for computations (seconds)
    #[serde(default = "default_timeout")]
    pub timeout_secs: u64,
}

fn default_workers() -> usize {
    4
}

fn default_timeout() -> u64 {
    300
}

impl AppConfig {
    /// Load configuration from a file.
    pub fn load(path: &Path) -> Result<Self> {
        let content = std::fs::read_to_string(path)?;
        let config: AppConfig = serde_json::from_str(&content)?;
        Ok(config)
    }

    /// Save configuration to a file.
    pub fn save(&self, path: &Path) -> Result<()> {
        let content = serde_json::to_string_pretty(self)?;
        std::fs::write(path, content)?;
        Ok(())
    }
}

/// Configuration watcher for hot-reload.
pub struct ConfigWatcher {
    path: PathBuf,
    config: Arc<RwLock<AppConfig>>,
    watcher: Option<notify::RecommendedWatcher>,
}

impl ConfigWatcher {
    /// Create a new configuration watcher.
    pub fn new(path: PathBuf, config: Arc<RwLock<AppConfig>>) -> Result<Self> {
        Ok(Self {
            path,
            config,
            watcher: None,
        })
    }

    /// Start watching for changes.
    pub fn start(&mut self) -> Result<()> {
        use notify::{Event, EventKind, RecursiveMode, Watcher};

        let config = self.config.clone();
        let path = self.path.clone();

        let mut watcher = notify::recommended_watcher(move |res: Result<Event, notify::Error>| {
            if let Ok(event) = res {
                if matches!(event.kind, EventKind::Modify(_)) {
                    if event.paths.contains(&path) {
                        log::info!("Config file changed, reloading...");
                        if let Ok(new_config) = AppConfig::load(&path) {
                            if let Ok(mut cfg) = config.write() {
                                *cfg = new_config;
                                log::info!("Configuration reloaded successfully");
                            }
                        }
                    }
                }
            }
        })?;

        watcher.watch(&self.path, RecursiveMode::NonRecursive)?;
        self.watcher = Some(watcher);

        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_default_config() {
        let config = AppConfig::default();
        assert_eq!(config.default_precision, "F64");
    }

    #[test]
    fn test_config_serialization() {
        let config = AppConfig::default();
        let json = serde_json::to_string(&config).unwrap();
        let parsed: AppConfig = serde_json::from_str(&json).unwrap();
        assert_eq!(config.default_precision, parsed.default_precision);
    }
}
