//! Experiment configuration matching the JSON format from backend/runner/config/.
//!
//! This module provides structures for defining experiments including:
//! - Series definitions with parameter expansion
//! - Noise configurations
//! - Filter configurations
//! - Acceleration methods with events

use anyhow::Result;
use serde::{Deserialize, Serialize};
use std::{
    collections::{BTreeMap, HashMap},
    fmt::Debug,
    ops::{Add, Mul},
    path::Path,
};
// TODO: Convert String to Arc<str>, and Vec<Event...> to Arc<[...]>.

// Range
trait NumLike = PartialOrd + From<u32> + Mul<Output = Self> + Add<Output = Self> + Copy;
// + Clone
// + Copy
// + From<u32>
// + Mul<Output = Self::T>
// + Add<Output = Self::T>
// + Debug
// + Serialize
// + for<'de> Deserialize<'de>;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct RangeDef<T> {
    pub start: T,
    pub stop: T,
    pub step: T,
}

impl<T: NumLike> RangeDef<T> {
    pub fn iter(&self) -> impl Iterator<Item = T> {
        let RangeDef { start, stop, step } = *self;

        (0_u32..)
            .map(move |i| start + T::from(i) * step)
            .take_while(move |&val| {
                if step > T::from(0u32) {
                    val < stop
                } else if step < T::from(0u32) {
                    val > stop
                } else {
                    false
                }
            })
    }
}

/// Argument value - can be single value, array, or range.
#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(untagged)]
pub enum Arg {
    /// Array of values
    Array(Vec<serde_json::Value>),
    /// Range i64
    RangeI64(RangeDef<i64>),
    /// Range f64
    RangeF64(RangeDef<f64>),
    /// Single value
    Single(serde_json::Value),
}

// I don't know why compiler requires me to implement this.
// TODO: remove, source of unsound behaviour.
impl Default for Arg {
    fn default() -> Self {
        Arg::Array(vec![])
    }
}

impl Arg {
    /// Get iterator of values.
    fn iter(&self) -> impl Iterator<Item = serde_json::Value> {
        gen {
            match self {
                Arg::Single(v) => yield (*v).clone(),
                Arg::Array(arr) => {
                    for x in arr {
                        yield x.clone();
                    }
                }
                Arg::RangeF64(range) => {
                    for v in range.iter() {
                        yield v.into();
                    }
                }
                Arg::RangeI64(range) => {
                    for v in range.iter() {
                        yield v.into();
                    }
                }
            }
        }
    }

    /// Expand arguments into all combinations.
    fn expand(args: &BTreeMap<String, Arg>) -> Vec<BTreeMap<String, serde_json::Value>> {
        let mut result = vec![BTreeMap::new()];

        for (key, value) in args {
            let mut new_result = Vec::new();
            for existing in result {
                for v in value.iter() {
                    let mut new_map = existing.clone();
                    new_map.insert(key.clone(), v.clone());
                    new_result.push(new_map);
                }
            }
            result = new_result;
        }

        result
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(untagged)]
pub enum ArgStr {
    Array(Vec<String>),
    Single(String),
}

impl ArgStr {
    fn iter(&self) -> impl Iterator<Item = &str> {
        gen {
            match self {
                ArgStr::Array(xs) => {
                    for x in xs {
                        yield x.as_str();
                    }
                }
                ArgStr::Single(x) => yield x.as_str(),
            }
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(untagged)]
pub enum ArgI {
    Array(Vec<i64>),
    RangeI64(RangeDef<i64>),
    Single(i64),
}

impl ArgI {
    /// Get iterator of values.
    fn iter(&self) -> impl Iterator<Item = i64> {
        gen {
            match self {
                ArgI::Single(v) => yield (*v).clone(),
                ArgI::Array(arr) => {
                    for x in arr {
                        yield x.clone();
                    }
                }
                ArgI::RangeI64(range) => {
                    for v in range.iter() {
                        yield v.into();
                    }
                }
            }
        }
    }
}

pub type SeriesDef = Series<Arg>;
pub type NoiseDef = Noise<ArgStr, Arg, Option<ArgI>>;
pub type FilterDef = Filter<Arg>;
pub type AccelDef = Accel<ArgI, Arg>;

pub type SeriesInstance = Series<serde_json::Value>;
pub type NoiseInstance = Noise<String, serde_json::Value, i64>;
pub type FilterInstance = Filter<serde_json::Value>;
pub type AccelInstance = Accel<i64, serde_json::Value>;

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
    pub accels: Vec<AccelDef>,

    /// Precision types to use (optional override)
    #[serde(default)]
    pub precisions: Option<Vec<String>>,

    /// Number of terms for computation (high-level parameter)
    pub n_points: Option<u64>,
}

impl ExperimentConfig {
    /// Load configuration from a JSON file.
    pub fn load(path: &Path) -> Result<Self> {
        let content = std::fs::read_to_string(path)?;
        let config: ExperimentConfig = serde_json::from_str(&content)?;
        Ok(config)
    }
}

/// Full series definition with parameters.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Series<T> {
    /// Series name from registry
    pub name: String,
    /// Series x
    pub x: T,
    /// Constructor arguments with value expansion support
    #[serde(default)]
    pub args: BTreeMap<String, T>,
}

impl SeriesDef {
    /// Expand this definition into concrete instances.
    pub fn expand(&self) -> impl Iterator<Item = SeriesInstance> {
        // Collect all argument combinations
        let arg_combinations = Arg::expand(&self.args);

        arg_combinations
            .into_iter()
            .zip(self.x.iter())
            .map(|(args, x)| SeriesInstance {
                name: self.name.clone(),
                args,
                x,
            })
    }
}

// Float range def/// Noise configuration.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Noise<S, F, I> {
    /// Noise type (like "Normal", "Uniform", "Poisson")
    #[serde(rename = "type")]
    pub noise_type: String,
    /// Application method (like "jitter", "scaling")
    pub method: S,
    /// Args (mean/stddev/min/max — different for all the types)
    pub args: BTreeMap<String, F>,
    /// Random seed
    #[serde(default)]
    pub seed: I,
}

impl NoiseDef {
    /// Expand this definition into concrete instances.
    pub fn expand(&self) -> impl Iterator<Item = NoiseInstance> {
        self.method
            .iter()
            .zip(Arg::expand(&self.args))
            .zip(self.seed.as_ref().unwrap_or(&ArgI::Single(0)).iter())
            .map(|((method, args), seed)| NoiseInstance {
                noise_type: self.noise_type.clone(),
                method: method.to_string(),
                args,
                seed,
            })
    }
}

/// Filter configuration.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Filter<T> {
    /// Filter type: "savitzkyGolay", "kolmogorovZurbenko"
    #[serde(rename = "type")]
    pub filter_type: String,
    /// Filter arguments
    #[serde(default)]
    pub args: BTreeMap<String, T>,
}

impl FilterDef {
    /// Expand this definition into concrete instances.
    pub fn expand(&self) -> impl Iterator<Item = FilterInstance> {
        Arg::expand(&self.args)
            .into_iter()
            .map(|args| FilterInstance {
                filter_type: self.filter_type.clone(),
                args,
            })
    }
}

/// Accel definition with parameter expansion.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Accel<I, F> {
    /// Accel name from registry
    pub name: String,

    /// M values (order parameter)
    pub m: I,

    /// Additional arguments with expansion
    #[serde(default)]
    pub args: BTreeMap<String, F>,

    /// Event configurations
    #[serde(default)]
    pub events: Vec<EventDef>,
}

impl AccelDef {
    /// Expand this definition into concrete instances.
    pub fn expand(&self) -> impl Iterator<Item = AccelInstance> {
        self.m
            .iter()
            .zip(Arg::expand(&self.args))
            .map(|(m, args)| AccelInstance {
                name: self.name.clone(),
                m,
                args,
                events: self.events.clone(),
            })
    }
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct EventDef {
    /// Event type: "slow_accel", "monotone", "divergent", "sign_changed", "second_diff"
    #[serde(rename = "type")]
    pub event_type: String,

    /// Maximum number of events to log (None = unlimited)
    pub log_action_capacity: Option<i64>,

    /// Number of events before stopping (None = never stop)
    pub stop_action_limit: Option<i64>,
}

// #[cfg(test)]
// mod tests {
//     use super::*;

//     #[test]
//     fn test_range_def_float() {
//         let range = RangeDefFloat {
//             start: 0.0,
//             stop: 1.0,
//             step: 0.2,
//         };
//         let values = range.values();
//         assert_eq!(values.len(), 6);
//         assert!((values[0] - 0.0).abs() < 1e-10);
//         assert!((values[5] - 1.0).abs() < 1e-10);
//     }

//     #[test]
//     fn test_range_def_int() {
//         let range = RangeDefInt {
//             start: 1,
//             stop: 10,
//             step: 2,
//         };
//         let values = range.values();
//         assert_eq!(values, vec![1, 3, 5, 7, 9]);
//     }

//     #[test]
//     fn test_series_expansion() {
//         let def = FullSeriesDef {
//             name: "CosSeries".to_string(),
//             args: {
//                 let mut args = HashMap::new();
//                 args.insert(
//                     "x".to_string(),
//                     ArgF::Array(vec![
//                         serde_json::Value::Number(serde_json::Number::from(1)),
//                         serde_json::Value::Number(serde_json::Number::from(2)),
//                     ]),
//                 );
//                 args
//             },
//             vec_size: None,
//         };

//         let instances = def.expand();
//         assert_eq!(instances.len(), 2);
//     }

//     #[test]
//     fn test_method_expansion() {
//         let def = AccelDef {
//             name: "LevinAlgorithm".to_string(),
//             m: vec![4],
//             args: {
//                 let mut args = HashMap::new();
//                 args.insert(
//                     "remainder".to_string(),
//                     vec![serde_json::Value::String("u_type".to_string())],
//                 );
//                 args
//             },
//             events: vec![],
//         };

//         let instances = def.expand();
//         assert_eq!(instances.len(), 2); // 2 n values × 1 m value × 1 arg combo
//     }

//     #[test]
//     fn test_config_loading() {
//         let json = r#"{
//             "series": [
//                 {"name": "ExpSeries", "args": {"x": [0.1]}}
//             ],
//             "methods": [
//                 {"name": "ShanksAlgorithm", "n": {"start": 1, "stop": 5, "step": 2}, "m": [4]}
//             ]
//         }"#;

//         let config: ExperimentConfig = serde_json::from_str(json).unwrap();
//         assert_eq!(config.series.len(), 1);
//         assert_eq!(config.methods.len(), 1);

//         let series = config.expand_series();
//         assert_eq!(series.len(), 1);

//         let methods = config.expand_methods();
//         assert_eq!(methods.len(), 3); // n = 1, 3, 5
//     }
// }
