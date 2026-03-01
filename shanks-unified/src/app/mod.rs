//! Application state and UI — being rewritten to use async compute pipeline.
//! This file is a transitional stub that will be replaced.

mod selection;
mod tree_ui;
mod ui;

use crate::cache::Cache;
use crate::experiment::ExperimentConfig;
use std::path::PathBuf;
use std::sync::Arc;

pub use selection::{SelectedCombination, SelectionNode, SelectionState};

/// Application state (transitional stub).
#[allow(dead_code)]
pub struct AppState {
    pub config: Option<ExperimentConfig>,
    pub cache: Cache,
}

impl AppState {
    pub fn new(
        config: Option<ExperimentConfig>,
        cache: Cache,
        library: Option<Arc<ShanksLibrary>>,
    ) -> Self {
        Self {
            config,
            cache,
            library,
        }
    }
}

/// Main application (eframe stub).
#[allow(dead_code)]
pub struct ShanksApp {
    state: AppState,
}

impl ShanksApp {
    pub fn new(state: AppState) -> Self {
        Self { state }
    }
}

impl eframe::App for ShanksApp {
    fn update(&mut self, ctx: &eframe::egui::Context, _frame: &mut eframe::Frame) {
        eframe::egui::CentralPanel::default().show(ctx, |ui| {
            ui.label("Shanks — rewrite in progress");
        });
    }
}
