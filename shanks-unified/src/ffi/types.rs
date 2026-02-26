//! Data types for FFI communication.

use serde::{Deserialize, Serialize};
use std::collections::HashMap;

// ============================================================================
// Raw FFI Data Structures
// ============================================================================

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct FfiLine {
    pub mantissas: *const f64,
    pub exponents: *const i64,
    pub len: u64,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct FfiLineColl {
    pub type_id: u32, // 0=Real, 1=Complex, 2=Interval, 3=CInterval
    pub lines: [FfiLine; 4],
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct FfiSeriesResult {
    pub sn: FfiLineColl,
    pub an: FfiLineColl,
    pub has_sum: u32,
    pub sum_type: u32,
    pub sum_m: [f64; 4],
    pub sum_e: [i64; 4],
    pub deviations: FfiLineColl,
}

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct FfiAccelResult {
    pub values: FfiLineColl,
    pub deviations: FfiLine,
}

impl FfiLine {
    pub unsafe fn to_scientific_array(&self) -> ScientificArray {
        let len = self.len as usize;
        if len == 0 || self.mantissas.is_null() || self.exponents.is_null() {
            return ScientificArray {
                mantissa: vec![0.0; len],
                exponent: vec![0; len],
            };
        }
        
        let mantissas = std::slice::from_raw_parts(self.mantissas, len).to_vec();
        let exponents = std::slice::from_raw_parts(self.exponents, len).to_vec();
        
        ScientificArray {
            mantissa: mantissas,
            exponent: exponents,
        }
    }
}

impl FfiLineColl {
    pub unsafe fn to_series_point_array(&self) -> SeriesPointArray {
        match self.type_id {
            0 => { // Real
                SeriesPointArray::Real(self.lines[0].to_scientific_array())
            }
            1 => { // Complex
                SeriesPointArray::Complex(ComplexArray {
                    real: self.lines[0].to_scientific_array(),
                    imag: self.lines[1].to_scientific_array(),
                })
            }
            2 => { // Interval
                SeriesPointArray::Interval(IntervalArray {
                    inf: self.lines[0].to_scientific_array(),
                    sup: self.lines[1].to_scientific_array(),
                })
            }
            3 => { // CInterval
                SeriesPointArray::CInterval(CIntervalArray {
                    real: IntervalArray {
                        inf: self.lines[0].to_scientific_array(),
                        sup: self.lines[1].to_scientific_array(),
                    },
                    imag: IntervalArray {
                        inf: self.lines[2].to_scientific_array(),
                        sup: self.lines[3].to_scientific_array(),
                    }
                })
            }
            _ => SeriesPointArray::Real(ScientificArray::default())
        }
    }
}

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
            return ScientificValue {
                mantissa: 0.0,
                exponent: 0,
            };
        }
        let exponent = value.abs().log10().floor() as i64;
        let mantissa = value / 10f64.powi(exponent as i32);
        ScientificValue { mantissa, exponent }
    }

    pub fn format_high_precision(&self) -> String {
        format!("{:.17e}", self.to_f64())
    }

    pub fn sub_approx(&self, other: &Self) -> Self {
        Self::from_f64(self.to_f64() - other.to_f64())
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

    /// Format as a high-precision scientific notation string.
    pub fn format_high_precision(&self) -> String {
        if self.imag.mantissa.abs() < 1e-18 {
            self.real.format_high_precision()
        } else {
            format!(
                "{} + {}j",
                self.real.format_high_precision(),
                self.imag.format_high_precision()
            )
        }
    }

    pub fn sub_approx(&self, other: &Self) -> Self {
        Self {
            real: self.real.sub_approx(&other.real),
            imag: self.imag.sub_approx(&other.imag),
        }
    }
}

/// Interval value bounds
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct IntervalValue {
    /// Infimum (lower bound)
    pub inf: ScientificValue,
    /// Supremum (upper bound)
    pub sup: ScientificValue,
}

impl IntervalValue {
    /// Format as a string.
    pub fn format(&self) -> String {
        format!("[{}, {}]", self.inf.format(), self.sup.format())
    }

    /// Format as a high-precision scientific notation string.
    pub fn format_high_precision(&self) -> String {
        format!(
            "[{}, {}]",
            self.inf.format_high_precision(),
            self.sup.format_high_precision()
        )
    }

    pub fn sub_approx(&self, other: &Self) -> Self {
        Self {
            inf: self.inf.sub_approx(&other.inf),
            sup: self.sup.sub_approx(&other.sup),
        }
    }
}

/// Complex interval value
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CIntervalValue {
    /// Real part
    pub real: IntervalValue,
    /// Imaginary part
    pub imag: IntervalValue,
}

impl CIntervalValue {
    /// Format as a string.
    pub fn format(&self) -> String {
        format!("{} + {}j", self.real.format(), self.imag.format())
    }

    /// Format as a high-precision scientific notation string.
    pub fn format_high_precision(&self) -> String {
        format!(
            "{} + {}j",
            self.real.format_high_precision(),
            self.imag.format_high_precision()
        )
    }
    pub fn sub_approx(&self, other: &Self) -> Self {
        Self {
            real: self.real.sub_approx(&other.real),
            imag: self.imag.sub_approx(&other.imag),
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(untagged)]
pub enum SeriesPoint {
    Real(ScientificValue),
    Complex(ComplexValue),
    Interval(IntervalValue),
    CInterval(CIntervalValue),
}

impl SeriesPoint {
    pub fn as_f64(&self) -> f64 {
        match self {
            SeriesPoint::Real(v) => v.to_f64(),
            SeriesPoint::Complex(v) => v.real.to_f64(),
            SeriesPoint::Interval(v) => (v.inf.to_f64() + v.sup.to_f64()) / 2.0,
            SeriesPoint::CInterval(v) => (v.real.inf.to_f64() + v.real.sup.to_f64()) / 2.0,
        }
    }

    pub fn format_high_precision(&self) -> String {
        match self {
            SeriesPoint::Real(v) => v.format_high_precision(),
            SeriesPoint::Complex(v) => v.format_high_precision(),
            SeriesPoint::Interval(v) => v.format_high_precision(),
            SeriesPoint::CInterval(v) => v.format_high_precision(),
        }
    }

    pub fn sub_approx(&self, other: &Self) -> Option<Self> {
        match (self, other) {
            (SeriesPoint::Real(a), SeriesPoint::Real(b)) => Some(SeriesPoint::Real(a.sub_approx(b))),
            (SeriesPoint::Complex(a), SeriesPoint::Complex(b)) => {
                Some(SeriesPoint::Complex(a.sub_approx(b)))
            }
            (SeriesPoint::Interval(a), SeriesPoint::Interval(b)) => {
                // Approximate difference as difference of midpoints
                let mid_a = (a.inf.to_f64() + a.sup.to_f64()) / 2.0;
                let mid_b = (b.inf.to_f64() + b.sup.to_f64()) / 2.0;
                Some(SeriesPoint::Real(ScientificValue::from_f64(mid_a - mid_b)))
            }
            (SeriesPoint::CInterval(a), SeriesPoint::CInterval(b)) => {
                let mid_re_a = (a.real.inf.to_f64() + a.real.sup.to_f64()) / 2.0;
                let mid_im_a = (a.imag.inf.to_f64() + a.imag.sup.to_f64()) / 2.0;
                let mid_re_b = (b.real.inf.to_f64() + b.real.sup.to_f64()) / 2.0;
                let mid_im_b = (b.imag.inf.to_f64() + b.imag.sup.to_f64()) / 2.0;
                Some(SeriesPoint::Complex(ComplexValue {
                    real: ScientificValue::from_f64(mid_re_a - mid_re_b),
                    imag: ScientificValue::from_f64(mid_im_a - mid_im_b),
                }))
            }
            _ => None,
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize, Default)]
pub struct ScientificArray {
    pub mantissa: Vec<f64>,
    pub exponent: Vec<i64>,
}

impl ScientificArray {
    pub fn len(&self) -> usize {
        self.mantissa.len().min(self.exponent.len())
    }

    pub fn is_empty(&self) -> bool {
        self.len() == 0
    }

    pub fn to_f64(&self, idx: usize) -> f64 {
        self.mantissa[idx] * 10f64.powi(self.exponent[idx] as i32)
    }

    pub fn get(&self, idx: usize) -> ScientificValue {
        ScientificValue {
            mantissa: self.mantissa.get(idx).copied().unwrap_or(0.0),
            exponent: self.exponent.get(idx).copied().unwrap_or(0),
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ComplexArray {
    pub real: ScientificArray,
    pub imag: ScientificArray,
}

impl ComplexArray {
    pub fn get(&self, idx: usize) -> ComplexValue {
        ComplexValue {
            real: self.real.get(idx),
            imag: self.imag.get(idx),
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct IntervalArray {
    pub inf: ScientificArray,
    pub sup: ScientificArray,
}

impl IntervalArray {
    pub fn get(&self, idx: usize) -> IntervalValue {
        IntervalValue {
            inf: self.inf.get(idx),
            sup: self.sup.get(idx),
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CIntervalArray {
    pub real: IntervalArray,
    pub imag: IntervalArray,
}

impl CIntervalArray {
    pub fn get(&self, idx: usize) -> CIntervalValue {
        CIntervalValue {
            real: self.real.get(idx),
            imag: self.imag.get(idx),
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
#[serde(untagged)]
pub enum SeriesPointArray {
    Real(ScientificArray),
    Complex(ComplexArray),
    Interval(IntervalArray),
    CInterval(CIntervalArray),
}

impl SeriesPointArray {
    pub fn len(&self) -> usize {
        match self {
            Self::Real(r) => r.len(),
            Self::Complex(c) => c.real.len(),
            Self::Interval(i) => i.inf.len(),
            Self::CInterval(ci) => ci.real.inf.len(),
        }
    }

    pub fn is_empty(&self) -> bool {
        self.len() == 0
    }

    pub fn get(&self, idx: usize) -> SeriesPoint {
        match self {
            Self::Real(r) => SeriesPoint::Real(r.get(idx)),
            Self::Complex(c) => SeriesPoint::Complex(c.get(idx)),
            Self::Interval(i) => SeriesPoint::Interval(i.get(idx)),
            Self::CInterval(ci) => SeriesPoint::CInterval(ci.get(idx)),
        }
    }

    pub fn as_f64(&self, idx: usize) -> f64 {
        match self {
            Self::Real(r) => r.to_f64(idx),
            Self::Complex(c) => c.real.to_f64(idx),
            Self::Interval(i) => (i.inf.to_f64(idx) + i.sup.to_f64(idx)) / 2.0,
            Self::CInterval(ci) => (ci.real.inf.to_f64(idx) + ci.real.sup.to_f64(idx)) / 2.0,
        }
    }

    pub fn from_vec(points: &[SeriesPoint]) -> Self {
        if points.is_empty() {
            return Self::Real(Default::default());
        }
        match &points[0] {
            SeriesPoint::Real(_) => {
                let mut mantissa = Vec::with_capacity(points.len());
                let mut exponent = Vec::with_capacity(points.len());
                for p in points {
                    if let SeriesPoint::Real(r) = p {
                        mantissa.push(r.mantissa);
                        exponent.push(r.exponent);
                    } else {
                        mantissa.push(0.0);
                        exponent.push(0);
                    }
                }
                Self::Real(ScientificArray { mantissa, exponent })
            }
            SeriesPoint::Complex(_) => {
                let mut r_mantissa = Vec::with_capacity(points.len());
                let mut r_exponent = Vec::with_capacity(points.len());
                let mut i_mantissa = Vec::with_capacity(points.len());
                let mut i_exponent = Vec::with_capacity(points.len());
                for p in points {
                    if let SeriesPoint::Complex(c) = p {
                        r_mantissa.push(c.real.mantissa);
                        r_exponent.push(c.real.exponent);
                        i_mantissa.push(c.imag.mantissa);
                        i_exponent.push(c.imag.exponent);
                    } else {
                        r_mantissa.push(0.0);
                        r_exponent.push(0);
                        i_mantissa.push(0.0);
                        i_exponent.push(0);
                    }
                }
                Self::Complex(ComplexArray {
                    real: ScientificArray {
                        mantissa: r_mantissa,
                        exponent: r_exponent,
                    },
                    imag: ScientificArray {
                        mantissa: i_mantissa,
                        exponent: i_exponent,
                    },
                })
            }
            SeriesPoint::Interval(_) => {
                let mut inf_m = Vec::with_capacity(points.len());
                let mut inf_e = Vec::with_capacity(points.len());
                let mut sup_m = Vec::with_capacity(points.len());
                let mut sup_e = Vec::with_capacity(points.len());
                for p in points {
                    if let SeriesPoint::Interval(i) = p {
                        inf_m.push(i.inf.mantissa);
                        inf_e.push(i.inf.exponent);
                        sup_m.push(i.sup.mantissa);
                        sup_e.push(i.sup.exponent);
                    } else {
                        inf_m.push(0.0);
                        inf_e.push(0);
                        sup_m.push(0.0);
                        sup_e.push(0);
                    }
                }
                Self::Interval(IntervalArray {
                    inf: ScientificArray {
                        mantissa: inf_m,
                        exponent: inf_e,
                    },
                    sup: ScientificArray {
                        mantissa: sup_m,
                        exponent: sup_e,
                    },
                })
            }
            SeriesPoint::CInterval(_) => {
                let mut r_inf_m = Vec::with_capacity(points.len());
                let mut r_inf_e = Vec::with_capacity(points.len());
                let mut r_sup_m = Vec::with_capacity(points.len());
                let mut r_sup_e = Vec::with_capacity(points.len());
                let mut i_inf_m = Vec::with_capacity(points.len());
                let mut i_inf_e = Vec::with_capacity(points.len());
                let mut i_sup_m = Vec::with_capacity(points.len());
                let mut i_sup_e = Vec::with_capacity(points.len());

                for p in points {
                    if let SeriesPoint::CInterval(ci) = p {
                        r_inf_m.push(ci.real.inf.mantissa);
                        r_inf_e.push(ci.real.inf.exponent);
                        r_sup_m.push(ci.real.sup.mantissa);
                        r_sup_e.push(ci.real.sup.exponent);
                        i_inf_m.push(ci.imag.inf.mantissa);
                        i_inf_e.push(ci.imag.inf.exponent);
                        i_sup_m.push(ci.imag.sup.mantissa);
                        i_sup_e.push(ci.imag.sup.exponent);
                    } else {
                        r_inf_m.push(0.0);
                        r_inf_e.push(0);
                        r_sup_m.push(0.0);
                        r_sup_e.push(0);
                        i_inf_m.push(0.0);
                        i_inf_e.push(0);
                        i_sup_m.push(0.0);
                        i_sup_e.push(0);
                    }
                }
                Self::CInterval(CIntervalArray {
                    real: IntervalArray {
                        inf: ScientificArray {
                            mantissa: r_inf_m,
                            exponent: r_inf_e,
                        },
                        sup: ScientificArray {
                            mantissa: r_sup_m,
                            exponent: r_sup_e,
                        },
                    },
                    imag: IntervalArray {
                        inf: ScientificArray {
                            mantissa: i_inf_m,
                            exponent: i_inf_e,
                        },
                        sup: ScientificArray {
                            mantissa: i_sup_m,
                            exponent: i_sup_e,
                        },
                    },
                })
            }
        }
    }
}

/// Result of series generation.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SeriesResult {
    /// Partial sums Sn
    #[serde(alias = "Sn")]
    pub sn: SeriesPointArray,
    /// Individual terms an
    #[serde(alias = "an")]
    pub an: SeriesPointArray,
    /// Analytical sum (if known)
    #[serde(skip_serializing_if = "Option::is_none")]
    pub sum: Option<SeriesPoint>,
    /// Deviations |Sn - S| for each partial sum (if the true limit is known)
    #[serde(default)]
    pub deviations: ScientificArray,
}

/// Profiling trace for operation counting per point.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ProfilingTrace {
    /// Number of additions per point
    pub add: Vec<u64>,
    /// Number of multiplications per point
    pub mul: Vec<u64>,
    /// Number of divisions per point
    pub div: Vec<u64>,
    /// Number of special operations per point
    pub special: Vec<u64>,
}

/// Result of acceleration algorithm application.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AccelResult {
    /// Accelerated values
    pub values: SeriesPointArray,
    /// Whether each point is valid (e.g., algorithm didn't fail)
    pub valid: Vec<bool>,
    /// Deviations/errors for each value
    #[serde(default)]
    pub deviations: ScientificArray,
    /// Events during computation
    #[serde(default)]
    pub events: Vec<ComputeEventEntry>,
    /// Errors during computation
    #[serde(default)]
    pub errors: Vec<ComputeError>,
    /// Smoothed estimates for divergent tails
    #[serde(default, skip_serializing_if = "Vec::is_empty")]
    pub filtered_estimates: Vec<SmoothedEstimate>,
    /// Profiling information (if enabled)
    #[serde(skip_serializing_if = "Option::is_none")]
    pub profiling: Option<ProfilingTrace>,
}

/// A smoothed limit estimate from a divergent tail.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SmoothedEstimate {
    /// The name of the event that triggered smoothing (e.g. "divergent_accel")
    pub event_name: String,
    /// Name of the filter applied (e.g. "kz")
    pub filter: String,
    /// The calculated limit points matching the tail segment
    pub limit: Vec<SeriesPoint>,
    /// The index where divergence started
    pub start_n: u64,
    /// Length of the smoothed segment
    pub length: u64,
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
    Progress {
        stage: String,
        current: u64,
        total: u64,
    },
    /// Series generation complete
    SeriesComplete { name: String, result: SeriesResult },
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
