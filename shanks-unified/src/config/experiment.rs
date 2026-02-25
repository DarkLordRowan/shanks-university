//! Experiment configuration matching the JSON format from backend/runner/config/.
//!
//! This module provides structures for defining experiments including:
//! - Series definitions with parameter expansion
//! - Noise configurations
//! - Filter configurations
//! - Acceleration methods with events

use anyhow::Result;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::path::Path;

// ============================================================================
// Main Configuration Structure
// ============================================================================

/// Main experiment configuration - matches JSON format from backend/runner/config/
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ExperimentConfig {
    /// Series definitions
    #[serde(default)]
    pub series: Vec<SeriesDef>,

    /// Noise configurations
    #[serde(default)]
    pub noises: Vec<NoiseDef>,

    /// Filter configurations
    #[serde(default)]
    pub filters: Vec<FilterDef>,

    /// Acceleration methods
    #[serde(default)]
    pub methods: Vec<MethodDef>,

    /// Precision types to use (optional override)
    #[serde(default)]
    pub precisions: Option<Vec<String>>,
}

impl ExperimentConfig {
    /// Load configuration from a JSON file.
    pub fn load(path: &Path) -> Result<Self> {
        let content = std::fs::read_to_string(path)?;
        let config: ExperimentConfig = serde_json::from_str(&content)?;
        Ok(config)
    }

    /// Save configuration to a JSON file.
    pub fn save(&self, path: &Path) -> Result<()> {
        let content = serde_json::to_string_pretty(self)?;
        std::fs::write(path, content)?;
        Ok(())
    }

    /// Expand all series definitions into concrete instances.
    pub fn expand_series(&self) -> Vec<SeriesInstance> {
        let mut instances = Vec::new();
        for def in &self.series {
            instances.extend(def.expand());
        }
        instances
    }

    /// Expand all method definitions into concrete instances.
    pub fn expand_methods(&self) -> Vec<MethodInstance> {
        let mut instances = Vec::new();
        for def in &self.methods {
            instances.extend(def.expand());
        }
        instances
    }

    /// Calculate total number of trials.
    pub fn total_trials(&self, precisions: &[String]) -> usize {
        let series_count = self.expand_series().len();
        let methods_count = self.expand_methods().len();
        let noises_count = self.noises.len().max(1);
        let precisions_count = precisions.len().max(1);

        series_count * methods_count * noises_count * precisions_count
    }

    /// Get precisions list (from config or defaults).
    pub fn get_precisions(&self) -> Vec<String> {
        self.precisions.clone().unwrap_or_else(|| {
            vec![
                "F32".to_string(),
                "F64".to_string(),
                "FLong".to_string(),
                "Arb".to_string(),
            ]
        })
    }
}

impl Default for ExperimentConfig {
    fn default() -> Self {
        Self {
            series: Vec::new(),
            noises: Vec::new(),
            filters: Vec::new(),
            methods: Vec::new(),
            precisions: None,
        }
    }
}

// ============================================================================
// Series Definition
// ============================================================================

/// Series definition with parameter expansion support.
#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(untagged)]
pub enum SeriesDef {
    /// Simple string reference (CSV file path or series name)
    Simple(String),
    /// Full definition with parameters
    Full(FullSeriesDef),
}

/// Full series definition with parameters.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct FullSeriesDef {
    /// Series name from registry
    pub name: String,

    /// Constructor arguments with value expansion support
    #[serde(default)]
    pub args: HashMap<String, ArgValue>,

    /// Optional: number of terms override
    #[serde(default)]
    pub vec_size: Option<u64>,
}

impl SeriesDef {
    /// Expand this definition into concrete instances.
    pub fn expand(&self) -> Vec<SeriesInstance> {
        match self {
            SeriesDef::Simple(s) => {
                // Check if it's a CSV file reference or a series name
                vec![SeriesInstance {
                    name: s.clone(),
                    args: HashMap::new(),
                    vec_size: None,
                }]
            }
            SeriesDef::Full(def) => def.expand(),
        }
    }
}

impl FullSeriesDef {
    /// Expand this definition into concrete instances.
    pub fn expand(&self) -> Vec<SeriesInstance> {
        // Collect all argument combinations
        let arg_combinations = expand_args(&self.args);

        arg_combinations
            .into_iter()
            .map(|args| SeriesInstance {
                name: self.name.clone(),
                args,
                vec_size: self.vec_size,
            })
            .collect()
    }
}

/// Expanded series instance ready for computation.
#[derive(Debug, Clone)]
pub struct SeriesInstance {
    /// Series name from registry
    pub name: String,
    /// Concrete argument values
    pub args: HashMap<String, serde_json::Value>,
    /// Number of terms (if specified)
    pub vec_size: Option<u64>,
}

// ============================================================================
// Argument Value Types
// ============================================================================

/// Argument value - can be single value, array, or range.
#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(untagged)]
pub enum ArgValue {
    /// Range definition
    Range(RangeDefFloat),
    /// Array of values
    Array(Vec<serde_json::Value>),
    /// Single value
    Single(serde_json::Value),
}

/// Float range definition for parameter expansion.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct RangeDefFloat {
    pub start: f64,
    pub stop: f64,
    pub step: f64,
}

impl RangeDefFloat {
    /// Generate all values in this range.
    pub fn values(&self) -> Vec<f64> {
        let mut values = Vec::new();
        let mut current = self.start;
        while current <= self.stop + 1e-10 {
            values.push(current);
            current += self.step;
        }
        values
    }
}

/// Expand arguments into all combinations.
fn expand_args(args: &HashMap<String, ArgValue>) -> Vec<HashMap<String, serde_json::Value>> {
    if args.is_empty() {
        return vec![HashMap::new()];
    }

    let mut result = vec![HashMap::new()];

    for (key, value) in args {
        let values: Vec<serde_json::Value> = match value {
            ArgValue::Single(v) => vec![v.clone()],
            ArgValue::Array(arr) => arr.clone(),
            ArgValue::Range(range) => range
                .values()
                .into_iter()
                .map(|v| {
                    serde_json::Value::Number(
                        serde_json::Number::from_f64(v)
                            .unwrap_or_else(|| serde_json::Number::from(0)),
                    )
                })
                .collect(),
        };

        let mut new_result = Vec::new();
        for existing in result {
            for v in &values {
                let mut new_map = existing.clone();
                new_map.insert(key.clone(), v.clone());
                new_result.push(new_map);
            }
        }
        result = new_result;
    }

    result
}

// ============================================================================
// Noise Definition
// ============================================================================

/// Noise configuration.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct NoiseDef {
    /// Noise type: "Normal", "Uniform", "Poisson"
    #[serde(rename = "type")]
    pub noise_type: String,

    /// Application method: "jitter", "scaling"
    pub method: String,

    /// First parameter (mean for normal, min for uniform)
    pub param1: f64,

    /// Second parameter (stddev for normal, max for uniform)
    pub param2: f64,

    /// Random seed
    #[serde(default)]
    pub seed: u64,
}

// ============================================================================
// Filter Definition
// ============================================================================

/// Filter configuration.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct FilterDef {
    /// Filter type: "savitzkyGolay", "kolmogorovZurbenko"
    #[serde(rename = "type")]
    pub filter_type: String,

    /// Filter arguments
    #[serde(default)]
    pub args: HashMap<String, serde_json::Value>,
}

// ============================================================================
// Method Definition
// ============================================================================

/// Method definition with parameter expansion.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct MethodDef {
    /// Method name from registry
    pub name: String,

    /// N values - range or array
    pub n: NValue,

    /// M values (order parameter)
    pub m: Vec<i64>,

    /// Additional arguments with expansion
    #[serde(default)]
    pub args: HashMap<String, Vec<serde_json::Value>>,

    /// Event configurations
    #[serde(default)]
    pub events: Vec<EventDef>,
}

/// N value - can be range or array.
#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(untagged)]
pub enum NValue {
    /// Range definition
    Range(RangeDefInt),
    /// Array of values
    Array(Vec<i64>),
}

impl NValue {
    /// Get all N values.
    pub fn values(&self) -> Vec<i64> {
        match self {
            NValue::Range(r) => r.values(),
            NValue::Array(arr) => arr.clone(),
        }
    }
}

/// Integer range definition for parameter expansion.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct RangeDefInt {
    pub start: i64,
    pub stop: i64,
    pub step: i64,
}

impl RangeDefInt {
    /// Generate all values in this range.
    pub fn values(&self) -> Vec<i64> {
        let mut values = Vec::new();
        let mut current = self.start;
        while current <= self.stop {
            values.push(current);
            current += self.step;
        }
        values
    }
}

impl MethodDef {
    /// Expand this definition into concrete instances.
    pub fn expand(&self) -> Vec<MethodInstance> {
        let n_values = self.n.values();
        let m_values = &self.m;

        // Expand args combinations
        let args_combinations = expand_method_args(&self.args);

        let mut instances = Vec::new();

        for n in &n_values {
            for m in m_values {
                if args_combinations.is_empty() {
                    instances.push(MethodInstance {
                        name: self.name.clone(),
                        n: *n,
                        m: *m,
                        args: HashMap::new(),
                        events: self.events.clone(),
                    });
                } else {
                    for args in &args_combinations {
                        instances.push(MethodInstance {
                            name: self.name.clone(),
                            n: *n,
                            m: *m,
                            args: args.clone(),
                            events: self.events.clone(),
                        });
                    }
                }
            }
        }

        instances
    }
}

/// Expand method arguments into all combinations.
fn expand_method_args(
    args: &HashMap<String, Vec<serde_json::Value>>,
) -> Vec<HashMap<String, serde_json::Value>> {
    if args.is_empty() {
        return vec![];
    }

    let mut result = vec![HashMap::new()];

    for (key, values) in args {
        let mut new_result = Vec::new();
        for existing in result {
            for v in values {
                let mut new_map = existing.clone();
                new_map.insert(key.clone(), v.clone());
                new_result.push(new_map);
            }
        }
        result = new_result;
    }

    result
}

/// Expanded method instance ready for computation.
#[derive(Debug, Clone)]
pub struct MethodInstance {
    /// Method name from registry
    pub name: String,
    /// Number of terms
    pub n: i64,
    /// Order parameter
    pub m: i64,
    /// Concrete argument values
    pub args: HashMap<String, serde_json::Value>,
    /// Event configurations
    pub events: Vec<EventDef>,
}

// ============================================================================
// Event Definition
// ============================================================================

/// Event configuration for computation monitoring.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct EventDef {
    /// Event type: "slow_accel", "monotone", "divergent", "sign_changed", "second_diff"
    #[serde(rename = "type")]
    pub event_type: String,

    /// Maximum number of events to log (None = unlimited)
    pub log_action_capacity: Option<i64>,

    /// Number of events before stopping (None = never stop)
    pub stop_action_limit: Option<i64>,
}

// ============================================================================
// Tests
// ============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_range_def_float() {
        let range = RangeDefFloat {
            start: 0.0,
            stop: 1.0,
            step: 0.2,
        };
        let values = range.values();
        assert_eq!(values.len(), 6);
        assert!((values[0] - 0.0).abs() < 1e-10);
        assert!((values[5] - 1.0).abs() < 1e-10);
    }

    #[test]
    fn test_range_def_int() {
        let range = RangeDefInt {
            start: 1,
            stop: 10,
            step: 2,
        };
        let values = range.values();
        assert_eq!(values, vec![1, 3, 5, 7, 9]);
    }

    #[test]
    fn test_series_expansion() {
        let def = FullSeriesDef {
            name: "CosSeries".to_string(),
            args: {
                let mut args = HashMap::new();
                args.insert(
                    "x".to_string(),
                    ArgValue::Array(vec![
                        serde_json::Value::Number(serde_json::Number::from(1)),
                        serde_json::Value::Number(serde_json::Number::from(2)),
                    ]),
                );
                args
            },
            vec_size: None,
        };

        let instances = def.expand();
        assert_eq!(instances.len(), 2);
    }

    #[test]
    fn test_method_expansion() {
        let def = MethodDef {
            name: "LevinAlgorithm".to_string(),
            n: NValue::Array(vec![10, 20]),
            m: vec![4],
            args: {
                let mut args = HashMap::new();
                args.insert(
                    "remainder".to_string(),
                    vec![serde_json::Value::String("u_type".to_string())],
                );
                args
            },
            events: vec![],
        };

        let instances = def.expand();
        assert_eq!(instances.len(), 2); // 2 n values × 1 m value × 1 arg combo
    }

    #[test]
    fn test_config_loading() {
        let json = r#"{
            "series": [
                {"name": "ExpSeries", "args": {"x": [0.1]}}
            ],
            "methods": [
                {"name": "ShanksAlgorithm", "n": {"start": 1, "stop": 5, "step": 2}, "m": [4]}
            ]
        }"#;

        let config: ExperimentConfig = serde_json::from_str(json).unwrap();
        assert_eq!(config.series.len(), 1);
        assert_eq!(config.methods.len(), 1);

        let series = config.expand_series();
        assert_eq!(series.len(), 1);

        let methods = config.expand_methods();
        assert_eq!(methods.len(), 3); // n = 1, 3, 5
    }
}
