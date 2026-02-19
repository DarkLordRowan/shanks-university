//! Data types for FFI communication.

use serde::{Deserialize, Serialize};
use std::collections::HashMap;

// ============================================================================
// Parameter Types (Generic)
// ============================================================================

/// A parameter value that can be passed to series or algorithms.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(untagged)]
pub enum ParamValue {
    Float(f64),
    Int(i64),
    Bool(bool),
    String(String),
}

impl ParamValue {
    pub fn as_f64(&self) -> Option<f64> {
        match self {
            ParamValue::Float(f) => Some(*f),
            ParamValue::Int(i) => Some(*i as f64),
            _ => None,
        }
    }

    pub fn as_bool(&self) -> Option<bool> {
        match self {
            ParamValue::Bool(b) => Some(*b),
            _ => None,
        }
    }

    pub fn as_str(&self) -> Option<&str> {
        match self {
            ParamValue::String(s) => Some(s),
            _ => None,
        }
    }
}

/// Parameter metadata from the FFI.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ParamMeta {
    /// Parameter name
    pub name: String,
    /// Parameter type: "float", "int", "bool", "string", "enum"
    #[serde(rename = "type")]
    pub param_type: String,
    /// For enum types, the list of valid values
    #[serde(default)]
    pub values: Vec<String>,
    /// Default value (as string)
    #[serde(skip_serializing_if = "Option::is_none")]
    pub default: Option<String>,
    /// Whether this parameter is required
    #[serde(default)]
    pub required: bool,
}

// ============================================================================
// Registry Types
// ============================================================================

/// Series metadata from the registry.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SeriesInfo {
    /// Series name (e.g., "PiSeries")
    pub name: String,
    /// Parameter metadata
    pub params: Vec<ParamMeta>,
    /// Human-readable description
    #[serde(default)]
    pub description: String,
}

/// Acceleration algorithm metadata from the registry.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AccelInfo {
    /// Algorithm name (e.g., "Shanks Transformation")
    pub name: String,
    /// Internal ID (e.g., "shanks_transformation_id")
    #[serde(skip_serializing_if = "Option::is_none")]
    pub id: Option<String>,
    /// Parameter metadata
    pub params: Vec<ParamMeta>,
    /// Human-readable description
    #[serde(default)]
    pub description: String,
}

// ============================================================================
// Task Types
// ============================================================================

/// Parameters for a series computation.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SeriesParams {
    /// Series name (e.g., "Ln2Series")
    pub name: String,
    /// X value as string (scientific notation supported)
    pub x_value: String,
    /// Named parameters (e.g., "alpha" for BinSeries)
    #[serde(default)]
    pub params: HashMap<String, ParamValue>,
}

/// Parameters for an acceleration algorithm.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AccelParams {
    /// Algorithm name (e.g., "Shanks Transformation")
    pub name: String,
    /// Named parameters (e.g., "remainder_type", "recurrent")
    #[serde(default)]
    pub params: HashMap<String, ParamValue>,
}

/// A computation task.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ComputeTask {
    /// Unique task ID
    #[serde(default = "uuid::Uuid::new_v4")]
    pub id: uuid::Uuid,
    /// Precision type: "F32", "F64", "FLong", "Arb", "CF32", "CF64", "CFLong", "CArb"
    pub precision: String,
    /// Series parameters
    pub series: SeriesParams,
    /// Number of points to generate
    pub n_points: u64,
    /// Algorithms to apply
    #[serde(default)]
    pub algorithms: Vec<AccelParams>,
}

// ============================================================================
// Result Types
// ============================================================================

/// Scientific notation value.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ScientificValue {
    /// Mantissa (significant digits)
    pub mantissa: f64,
    /// Exponent (power of 10)
    pub exponent: i64,
}

impl ScientificValue {
    /// Convert to a regular f64 (may lose precision for very large/small values).
    pub fn to_f64(&self) -> f64 {
        self.mantissa * 10f64.powi(self.exponent as i32)
    }

    /// Format as a string in scientific notation.
    pub fn format(&self) -> String {
        if self.exponent == 0 {
            format!("{}", self.mantissa)
        } else {
            format!("{}e{}", self.mantissa, self.exponent)
        }
    }

    /// Create from f64.
    pub fn from_f64(value: f64) -> Self {
        if value == 0.0 {
            return ScientificValue { mantissa: 0.0, exponent: 0 };
        }
        let exponent = value.abs().log10().floor() as i64;
        let mantissa = value / 10f64.powi(exponent as i32);
        ScientificValue { mantissa, exponent }
    }
}

/// Complex number in scientific notation.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ComplexValue {
    /// Real part
    pub real: ScientificValue,
    /// Imaginary part
    pub imag: ScientificValue,
}

impl ComplexValue {
    /// Format as a string.
    pub fn format(&self) -> String {
        if self.imag.mantissa.abs() < 1e-15 {
            self.real.format()
        } else {
            format!("{} + {}j", self.real.format(), self.imag.format())
        }
    }
}

/// A point in the series (Sn or an value).
#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(untagged)]
pub enum SeriesPoint {
    /// Real value
    Real(ScientificValue),
    /// Complex value
    Complex(ComplexValue),
}

impl SeriesPoint {
    /// Get as f64 (returns real part for complex).
    pub fn as_f64(&self) -> f64 {
        match self {
            SeriesPoint::Real(v) => v.to_f64(),
            SeriesPoint::Complex(v) => v.real.to_f64(),
        }
    }
}

/// Result of series generation.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SeriesResult {
    /// Partial sums Sn
    #[serde(alias = "Sn")]
    pub sn: Vec<SeriesPoint>,
    /// Individual terms an
    #[serde(alias = "an")]
    pub an: Vec<SeriesPoint>,
    /// Analytical sum (if known)
    #[serde(skip_serializing_if = "Option::is_none")]
    pub sum: Option<String>,
    /// Profiling information (if enabled)
    #[serde(skip_serializing_if = "Option::is_none")]
    pub profiling: Option<ProfilingInfo>,
}

/// Profiling information for operation counting.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ProfilingInfo {
    /// Number of additions
    pub add: u64,
    /// Number of multiplications
    pub mul: u64,
    /// Number of divisions
    pub div: u64,
    /// Number of special operations (sqrt, exp, etc.)
    pub special: u64,
}

/// Result of acceleration algorithm application.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AccelResult {
    /// Accelerated values
    pub values: Vec<Option<SeriesPoint>>,
    /// Deviations/errors for each value
    #[serde(default)]
    pub deviations: Vec<ScientificValue>,
    /// Events during computation
    #[serde(default)]
    pub events: Vec<ComputeEventEntry>,
    /// Errors during computation
    #[serde(default)]
    pub errors: Vec<ComputeError>,
    /// Profiling information (if enabled)
    #[serde(skip_serializing_if = "Option::is_none")]
    pub profiling: Option<ProfilingInfo>,
}

/// An event during computation (convergence, strategy change, etc.)
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ComputeEventEntry {
    /// Index n where the event occurred
    pub n: u64,
    /// Event name
    pub name: String,
    /// Event description
    #[serde(default)]
    pub description: String,
}

/// An error during computation.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ComputeError {
    /// Index n where the error occurred (0 for global errors)
    pub n: u64,
    /// Error message
    pub message: String,
}

// ============================================================================
// Compute Event Types (for async orchestration)
// ============================================================================

/// Event body for compute orchestration.
#[derive(Debug, Clone)]
pub enum ComputeEventBody {
    /// Task started
    Started,
    /// Progress update
    Progress { stage: String, current: u64, total: u64 },
    /// Series generation complete
    SeriesComplete { result: SeriesResult },
    /// Algorithm application complete
    AccelComplete { name: String, result: AccelResult },
    /// Error occurred
    Error { error: String },
    /// Task cancelled
    Cancelled,
    /// All computations complete
    Complete,
}

/// Compute event with task ID.
#[derive(Debug, Clone)]
pub struct ComputeEvent {
    /// Task ID
    pub task_id: uuid::Uuid,
    /// Event body
    pub body: ComputeEventBody,
}

// ============================================================================
// Noise Configuration
// ============================================================================

/// Noise configuration for series.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct NoiseConfig {
    /// Noise type: "normal", "uniform", "poisson"
    #[serde(rename = "type")]
    pub noise_type: String,
    /// First parameter (mean for normal, min for uniform, lambda for poisson)
    #[serde(skip_serializing_if = "Option::is_none")]
    pub param1: Option<String>,
    /// Second parameter (stddev for normal, max for uniform)
    #[serde(skip_serializing_if = "Option::is_none")]
    pub param2: Option<String>,
    /// Random seed
    #[serde(skip_serializing_if = "Option::is_none")]
    pub seed: Option<u64>,
}

/// Noise type metadata from the registry.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct NoiseInfo {
    /// Noise type name (e.g., "Normal")
    pub name: String,
    /// Parameter metadata
    pub params: Vec<ParamMeta>,
    /// Available methods
    #[serde(default)]
    pub methods: Vec<String>,
}
