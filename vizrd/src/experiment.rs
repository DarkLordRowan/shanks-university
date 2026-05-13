//! Experiment configuration matching the JSON format from backend/runner/config/.
//!
//! This module provides structures for defining experiments including:
//! - Series definitions with parameter expansion
//! - Noise configurations
//! - Filter configurations
//! - Acceleration methods with events
//! - File-based series (numbers loaded from .txt/.csv files)

use anyhow::{Context, Result};
use serde::{Deserialize, Serialize};
use std::{
    collections::BTreeMap,
    fmt::Debug,
    ops::{Add, Mul},
    path::Path,
    sync::Arc,
};

use crate::compute::SeriesEventKind;
// TODO: Convert String to Arc<str>, and Vec<Event...> to Arc<[...]>.

// Range
pub trait NumLike = PartialOrd + From<u32> + Mul<Output = Self> + Add<Output = Self> + Copy;
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
    pub fn iter(&self) -> impl Iterator<Item = serde_json::Value> {
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
    pub fn iter(&self) -> impl Iterator<Item = &str> {
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
    pub fn iter(&self) -> impl Iterator<Item = i64> {
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
/// A single expanded filter instance with one (or no) trigger condition.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq, Hash)]
pub struct FilterInstance {
    pub filter_type: String,
    #[serde(default)]
    pub args: BTreeMap<String, serde_json::Value>,
    /// The single trigger condition for this filter instance.
    /// `None` = filter from n=0 (no trigger condition).
    pub trigger_after: Option<(SeriesEventKind, i64)>,
}

/// A trigger threshold value — accepts a single integer or an array in JSON.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq, PartialOrd, Ord, Hash)]
#[serde(untagged)]
pub enum TriggerAfterValue {
    Single(i64),
    Multi(Vec<i64>),
}

impl TriggerAfterValue {
    pub fn iter_values(&self) -> Vec<i64> {
        match self {
            TriggerAfterValue::Single(v) => vec![*v],
            TriggerAfterValue::Multi(vs) => vs.clone(),
        }
    }
}
pub type AccelInstance = Accel<i64, serde_json::Value>;

// ---------------------------------------------------------------------------
// Series entry (raw from JSON + resolved with file data)
// ---------------------------------------------------------------------------

/// Raw series entry as it appears in JSON: either a proper `SeriesDef` object
/// or a bare string treated as a path to a data file.
#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(untagged)]
pub enum RawSeriesEntry {
    Def(SeriesDef),
    Path(String),
}

/// Resolved series entry. File paths have been read and their numeric content
/// is carried inline inside the `File` variant as `Arc<Vec<String>>`.
///
/// Strings (not f64) are kept so that C++ can parse them with the full
/// precision required by types like `mpfr::mpreal`.
#[derive(Debug, Clone)]
pub enum SeriesEntry {
    Registry(SeriesDef),
    File {
        name: String,
        sn: Arc<Vec<String>>,
    },
}

impl SeriesEntry {
    pub fn name(&self) -> &str {
        match self {
            SeriesEntry::Registry(d) => &d.name,
            SeriesEntry::File { name, .. } => name,
        }
    }

    /// File data, if this is a file-based entry.
    pub fn file_sn(&self) -> Option<&Arc<Vec<String>>> {
        match self {
            SeriesEntry::Registry(_) => None,
            SeriesEntry::File { sn, .. } => Some(sn),
        }
    }

    /// Expand this entry into concrete instances.
    /// Returns `(SeriesInstance, file_sn)` — `file_sn` is `Some` only for
    /// file-based entries.
    pub fn expand(&self) -> Vec<(SeriesInstance, Option<Arc<Vec<String>>>)> {
        match self {
            SeriesEntry::Registry(def) => def
                .expand()
                .map(|inst| (inst, None))
                .collect(),
            SeriesEntry::File { name, sn } => {
                let inst = SeriesInstance {
                    name: name.clone(),
                    x: serde_json::Value::Null,
                    args: BTreeMap::new(),
                };
                vec![(inst, Some(sn.clone()))]
            }
        }
    }
}

impl RawSeriesEntry {
    /// Resolve a raw entry: file paths are read and converted into
    /// `SeriesEntry::File`; object definitions become `SeriesEntry::Registry`.
    pub fn resolve(self, config_dir: &Path) -> Result<SeriesEntry> {
        match self {
            RawSeriesEntry::Def(def) => Ok(SeriesEntry::Registry(def)),
            RawSeriesEntry::Path(rel_path) => {
                let full_path = config_dir.join(&rel_path);
                let name = Path::new(&rel_path)
                    .file_stem()
                    .and_then(|s| s.to_str())
                    .unwrap_or(&rel_path)
                    .to_string();

                let sn = load_text_series_as_strings(&full_path)
                    .with_context(|| format!("Failed to load series file: {}", rel_path))?;

                Ok(SeriesEntry::File {
                    name,
                    sn: Arc::new(sn),
                })
            }
        }
    }
}

/// Main experiment configuration - matches JSON format from backend/runner/config/
#[derive(Debug, Clone)]
pub struct ExperimentConfig<T = SeriesEntry> {
    /// Series definitions
    pub series: Vec<T>,

    /// Noise configurations
    pub noises: Vec<NoiseDef>,

    /// Filter configurations
    pub filters: Vec<FilterDef>,

    /// Acceleration methods
    pub accels: Vec<AccelDef>,

    /// Precision types to use (optional override)
    pub precisions: Option<Vec<String>>,

    /// Event configurations (stop limits)
    pub events: BTreeMap<SeriesEventKind, EventConfig>,

    /// Number of terms for computation (high-level parameter)
    pub n_points: Option<u64>,
}

// --- Deserialization: ExperimentConfig<RawSeriesEntry> ---

impl ExperimentConfig<RawSeriesEntry> {
    /// Load a raw (unresolved) experiment config from a JSON file.
    ///
    /// The `series` array may contain `RawSeriesEntry::Path` entries (bare
    /// strings like `"./brent-daily.txt"`). Call `.resolve(dir)` on the result
    /// to convert those into `SeriesEntry::File` with loaded data.
    pub fn load_raw(path: &Path) -> Result<Self> {
        let content = std::fs::read_to_string(path)
            .with_context(|| format!("Failed to read config file: {}", path.display()))?;

        // Deserialize via a helper that has the right serde attributes.
        let raw: RawExperimentConfig = serde_json::from_str(&content)
            .with_context(|| "Failed to parse config JSON")?;

        Ok(raw.into())
    }

    /// Resolve all `Path` entries by reading their files, returning a fully
    /// resolved `ExperimentConfig<SeriesEntry>`.
    pub fn resolve(self, config_dir: &Path) -> Result<ExperimentConfig<SeriesEntry>> {
        let series = self
            .series
            .into_iter()
            .map(|e| e.resolve(config_dir))
            .collect::<Result<Vec<_>>>()?;

        Ok(ExperimentConfig {
            series,
            noises: self.noises,
            filters: self.filters,
            accels: self.accels,
            precisions: self.precisions,
            events: self.events,
            n_points: self.n_points,
        })
    }
}

// --- Public API ---

impl ExperimentConfig<SeriesEntry> {
    /// Load and resolve an experiment configuration from a JSON file.
    pub fn load(path: &Path) -> Result<Self> {
        let config_dir = path.parent().unwrap_or(Path::new("."));
        let mut config = ExperimentConfig::<RawSeriesEntry>::load_raw(path)?.resolve(config_dir)?;
        config.merge_filter_after();
        Ok(config)
    }

    /// Backward compatibility: merge `EventConfig.filter_after` into each
    /// filter's `trigger_after` (only for event kinds not already specified).
    fn merge_filter_after(&mut self) {
        for (kind, ecfg) in &self.events {
            if let Some(ref fav) = ecfg.filter_after {
                for filter in &mut self.filters {
                    filter.trigger_after.entry(kind.clone()).or_insert_with(|| fav.clone());
                }
            }
        }
    }
}

// --- Internal helper for serde (mirrors ExperimentConfig fields with serde attrs) ---

#[derive(Deserialize)]
struct RawExperimentConfig {
    #[serde(default)]
    series: Vec<RawSeriesEntry>,
    #[serde(default)]
    noises: Vec<NoiseDef>,
    #[serde(default)]
    filters: Vec<FilterDef>,
    #[serde(default)]
    accels: Vec<AccelDef>,
    #[serde(default)]
    precisions: Option<Vec<String>>,
    #[serde(default)]
    events: BTreeMap<SeriesEventKind, EventConfig>,
    n_points: Option<u64>,
}

impl From<RawExperimentConfig> for ExperimentConfig<RawSeriesEntry> {
    fn from(raw: RawExperimentConfig) -> Self {
        ExperimentConfig {
            series: raw.series,
            noises: raw.noises,
            filters: raw.filters,
            accels: raw.accels,
            precisions: raw.precisions,
            events: raw.events,
            n_points: raw.n_points,
        }
    }
}

/// Load a text file where each line contains one number.
///
/// Blank lines and lines starting with `#` are skipped.
/// Returns number-as-string values (not f64) to preserve full precision for C++.
fn load_text_series_as_strings(path: &Path) -> Result<Vec<String>> {
    let content = std::fs::read_to_string(path)
        .with_context(|| format!("Cannot read file: {}", path.display()))?;
    let mut values = Vec::new();
    for line in content.lines() {
        let trimmed = line.trim();
        if trimmed.is_empty() || trimmed.starts_with('#') {
            continue;
        }
        // Validate that the string is a valid number, but keep the original text.
        let _val: f64 = trimmed
            .parse()
            .with_context(|| format!("Failed to parse '{}' as number in {}", trimmed, path.display()))?;
        values.push(trimmed.to_string());
    }
    Ok(values)
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

        self.x.iter().flat_map(move |x| {
            let arg_combinations = arg_combinations.clone();
            let x = x.clone();
            let name = self.name.clone();
            arg_combinations
                .into_iter()
                .map(move |args| SeriesInstance {
                    name: name.clone(),
                    args,
                    x: x.clone(),
                })
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
        let arg_combinations = Arg::expand(&self.args);

        self.method.iter().flat_map(move |method| {
            let arg_combinations = arg_combinations.clone();
            let method = method.to_string();
            let noise_type = self.noise_type.clone();
            self.seed
                .as_ref()
                .unwrap_or(&ArgI::Single(0))
                .iter()
                .flat_map(move |seed| {
                    let arg_combinations = arg_combinations.clone();
                    let method = method.clone();
                    let noise_type = noise_type.clone();
                    arg_combinations.into_iter().map(move |args| NoiseInstance {
                        noise_type: noise_type.clone(),
                        method: method.clone(),
                        args,
                        seed,
                    })
                })
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
    /// Trigger condition: which event kinds trigger this filter,
    /// and after how many occurrences. Empty = filter from n=0.
    /// Each value can be a single integer or an array; each entry expands
    /// into a separate `FilterInstance`.
    /// Example: `{"divergent_accel": [2, 5, 7], "monotone": [3, 6]}`
    /// expands into 5 filter instances.
    #[serde(default)]
    pub trigger_after: BTreeMap<SeriesEventKind, TriggerAfterValue>,
}

impl FilterDef {
    /// Expand this definition into concrete instances.
    /// Each threshold in `trigger_after` produces a separate `FilterInstance`.
    /// `trigger_after: {"divergent_accel": [2, 5], "monotone": [3]}`
    /// with one arg combo → 3 FilterInstances.
    /// Empty `trigger_after` produces one instance with `trigger_after: None`.
    pub fn expand(&self) -> impl Iterator<Item = FilterInstance> {
        let filter_type = self.filter_type.clone();

        // Collect all (kind, threshold) pairs from trigger_after
        let trigger_pairs: Vec<(SeriesEventKind, i64)> = self
            .trigger_after
            .iter()
            .flat_map(|(kind, val)| {
                val.iter_values()
                    .into_iter()
                    .map(move |limit| (*kind, limit))
            })
            .collect();

        let arg_combos = Arg::expand(&self.args);

        arg_combos
            .into_iter()
            .flat_map(move |args| {
                let filter_type = filter_type.clone();
                if trigger_pairs.is_empty() {
                    vec![FilterInstance {
                        filter_type,
                        args,
                        trigger_after: None,
                    }]
                } else {
                    trigger_pairs
                        .iter()
                        .map(|&(kind, limit)| FilterInstance {
                            filter_type: filter_type.clone(),
                            args: args.clone(),
                            trigger_after: Some((kind, limit)),
                        })
                        .collect()
                }
            })
    }
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub struct Accel<I, F> {
    /// Accel name from registry
    pub name: String,

    /// M values (order parameter)
    pub m: I,

    /// Additional arguments with expansion
    #[serde(default)]
    pub args: BTreeMap<String, F>,
}

impl AccelDef {
    /// Expand this definition into concrete instances.
    pub fn expand(&self) -> impl Iterator<Item = AccelInstance> {
        let arg_combinations = Arg::expand(&self.args);

        self.m.iter().flat_map(move |m| {
            let arg_combinations = arg_combinations.clone();
            let name = self.name.clone();
            arg_combinations.into_iter().map(move |args| AccelInstance {
                name: name.clone(),
                m,
                args,
            })
        })
    }
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq, PartialOrd, Ord, Hash, Default)]
pub struct EventConfig {
    /// Deprecated: use `trigger_after` on `FilterDef` instead.
    /// Kept for backward compatibility in config files.
    /// Accepts a single integer or an array.
    #[serde(default, skip_serializing_if = "Option::is_none")]
    pub filter_after: Option<TriggerAfterValue>,
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
