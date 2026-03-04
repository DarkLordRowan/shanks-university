#![feature(gen_blocks)]
#![feature(yield_expr)]
#![feature(trait_alias)]
#![feature(map_try_insert)]
#![feature(generic_atomic)]

//! Vizrd - Series Acceleration Visualization Tool
//!
//! This library provides:
//! - FFI bindings to the C++ series acceleration library
//! - Compute engine for running series and algorithms
//! - SQLite-based caching
//! - Configuration management
//! - Headless runner for batch computation

pub mod app;
pub mod cache;
pub mod compute;
pub mod experiment;
pub mod export;
pub mod ffi;
pub mod headless;
pub mod plot;
