pub mod bridge;

pub use bridge::ffi::RealValue;

impl RealValue {
    pub fn to_f64(&self) -> f64 {
        self.mantissa * 10f64.powi(self.exponent as i32)
    }
}
use egui_plot::PlotPoint;
use serde::{Deserialize, Serialize};

/// Error type for FFI operations.
#[derive(Debug, thiserror::Error)]
pub enum FfiError {
    #[error("Null pointer returned")]
    NullPointer,

    #[error("Invalid UTF-8 string")]
    InvalidUtf8(#[from] std::str::Utf8Error),

    #[error("JSON error: {0}")]
    JsonError(#[from] serde_json::Error),

    #[error("Library error: {0}")]
    LibraryError(String),

    #[error("CXX error: {0}")]
    CxxError(#[from] cxx::Exception),
}

#[derive(Clone, Copy, Debug, PartialEq, Serialize, Deserialize)]
pub struct ComplexOf<T> {
    pub real: T,
    pub imag: T,
}
#[derive(Clone, Copy, Debug, PartialEq, Serialize, Deserialize)]
pub struct IntervalOf<T> {
    pub inf: T,
    pub sup: T,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ValueOf<T> {
    Real(T),
    Complex(ComplexOf<T>),
    Interval(IntervalOf<T>),
    CInterval(ComplexOf<IntervalOf<T>>),
}

pub type Value = ValueOf<RealValue>;
pub type Arr = ValueOf<Vec<RealValue>>;
pub type ArrF64 = ValueOf<Vec<f64>>;
pub type ArrLine = ValueOf<(String, Vec<PlotPoint>)>;

// /// Helper to convert f64 slice to bytes
// fn f64_to_bytes(data: &[f64]) -> &[u8] {
//     unsafe { std::slice::from_raw_parts(data.as_ptr() as *const u8, data.len() * 8) }
// }

// /// Helper to convert i64 slice to bytes
// fn i64_to_bytes(data: &[i64]) -> &[u8] {
//     unsafe { std::slice::from_raw_parts(data.as_ptr() as *const u8, data.len() * 8) }
// }

// /// Helper to parse f64 vec from bytes carefully (alignment safe)
// fn bytes_to_f64(data: &[u8]) -> Vec<f64> {
//     if data.is_empty() {
//         return Vec::new();
//     }
//     let count = data.len() / 8;
//     let mut vec = Vec::with_capacity(count);
//     unsafe {
//         std::ptr::copy_nonoverlapping(data.as_ptr(), vec.as_mut_ptr() as *mut u8, count * 8);
//         vec.set_len(count);
//     }
//     vec
// }

// /// Helper to parse i64 vec from bytes carefully (alignment safe)
// fn bytes_to_i64(data: &[u8]) -> Vec<i64> {
//     if data.is_empty() {
//         return Vec::new();
//     }
//     let count = data.len() / 8;
//     let mut vec = Vec::with_capacity(count);
//     unsafe {
//         std::ptr::copy_nonoverlapping(data.as_ptr(), vec.as_mut_ptr() as *mut u8, count * 8);
//         vec.set_len(count);
//     }
//     vec
// }

/// A parameter value that can be passed to series or algorithms.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(untagged)]
pub enum ParamValue {
    Float(f64),
    Int(i64),
    Bool(bool),
    String(String),
}

impl std::fmt::Display for ParamValue {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ParamValue::Float(val) => write!(f, "{}", val),
            ParamValue::Int(val) => write!(f, "{}", val),
            ParamValue::Bool(val) => write!(f, "{}", val),
            ParamValue::String(val) => write!(f, "{}", val),
        }
    }
}
// impl Arr {
//     fn from_raw(raw: ffi::RawArr) -> Self {
//         match raw.tag {
//             ffi::ArrKind::Real => Arr::Real(raw.r1),
//             ffi::ArrKind::Complex => Arr::Complex(
//                 raw.r2
//                     .into_iter()
//                     .zip(raw.r1)
//                     .map(|(imag, real)| ComplexValue { real, imag })
//                     .collect(),
//             ),
//             ffi::ArrKind::Interval => Arr::Interval(
//                 raw.r2
//                     .into_iter()
//                     .zip(raw.r1)
//                     .map(|(sup, inf)| IntervalValue { inf, sup })
//                     .collect(),
//             ),
//             ffi::ArrKind::CInterval => Arr::CInterval(
//                 raw.r1
//                     .into_iter()
//                     .zip(raw.r2)
//                     .zip(raw.r3)
//                     .zip(raw.r4)
//                     .map(|(((r1, r2), r3), r4)| CIntervalValue {
//                         inf: ComplexValue { real: r1, imag: r3 },
//                         sup: ComplexValue { real: r2, imag: r4 },
//                     })
//                     .collect(),
//             ),
//             _ => Arr::Real(Vec::new()),
//         }
//     }
// }

// impl Value {
//     fn from_raw(raw: ffi::RawValue) -> Self {
//         match raw.tag {
//             ffi::ValueKind::Real => Value::Real(raw.r1),
//             ffi::ValueKind::Complex => Value::Complex(ComplexValue {
//                 real: raw.r1,
//                 imag: raw.r2,
//             }),
//             ffi::ValueKind::Interval => Value::Interval(IntervalValue {
//                 inf: raw.r1,
//                 sup: raw.r2,
//             }),
//             ffi::ValueKind::CInterval => Value::CInterval(CIntervalValue {
//                 inf: ComplexValue {
//                     real: raw.r1,
//                     imag: raw.r3,
//                 },
//                 sup: ComplexValue {
//                     real: raw.r2,
//                     imag: raw.r4,
//                 },
//             }),
//             _ => Value::Real(RealValue {
//                 mantissa: 0.0,
//                 exponent: 0,
//             }),
//         }
//     }
// }

// /// A lightweight wrapper around the C++ library functions.
// /// Preserved for architectural compatibility.
// pub struct ShanksLibrary;

// impl ShanksLibrary {
//     pub fn new() -> Self {
//         Self
//     }

//     pub fn list_series(&self) -> Result<Vec<String>, FfiError> {
//         Ok(ffi::list_series())
//     }

//     pub fn list_accels(&self) -> Result<Vec<String>, FfiError> {
//         Ok(ffi::list_accels())
//     }

//     pub fn list_precisions(&self) -> Result<Vec<String>, FfiError> {
//         Ok(ffi::list_precisions())
//     }

//     pub fn list_noises(&self) -> Result<Vec<String>, FfiError> {
//         Ok(ffi::list_noises())
//     }

//     pub fn list_noise_methods(&self) -> Result<Vec<String>, FfiError> {
//         Ok(ffi::list_noise_methods())
//     }

//     // Legacy series methods, delegating to Series wrapper
//     pub fn series_create(
//         &self,
//         name: &str,
//         precision: &str,
//         x: &str,
//         params: &str,
//         n: usize,
//     ) -> Result<Series, FfiError> {
//         Series::new(name, precision, params, n, x)
//     }

//     pub fn series_create_with_noise(
//         &self,
//         name: &str,
//         precision: &str,
//         x: &str,
//         params: &str,
//         nt: &str,
//         nm: &str,
//         p1: f64,
//         p2: f64,
//         seed: u64,
//         n: usize,
//     ) -> Result<Series, FfiError> {
//         // For now, we apply noise after creation if needed, or we could update mk_series to handle it.
//         // But the C++ mk_series doesn't handle noise yet.
//         // We'll follow the pattern of creating then applying noise or just updating this to use n/x.
//         Series::new(name, precision, params, n, x)
//     }

//     pub fn series_generate(&self, series: &Series, n: u64) -> Result<SeriesResult, FfiError> {
//         Ok(SeriesResult {
//             sn: SeriesPointArray::from_vec(&series.sn().to_series_points()),
//             an: SeriesPointArray::from_vec(&series.an().to_series_points()),
//             sum: Some(series.limit().to_series_point()),
//             deviations: SeriesPointArray::from_vec(&series.deviation().to_series_points()),
//         })
//     }

//     pub fn series_get_sum(&self, series: &Series) -> Result<String, FfiError> {
//         let point = series.limit().to_series_point();
//         Ok(serde_json::to_string(&point)?)
//     }

//     pub fn accel_create(
//         &self,
//         name: &str,
//         precision: &str,
//         params: &str,
//     ) -> Result<String, FfiError> {
//         Ok(serde_json::to_string(&(name, precision, params))?)
//     }

//     pub fn accel_apply(
//         &self,
//         accel_spec: &str,
//         series: &Series,
//         n: usize,
//         m: usize,
//     ) -> Result<AccelResult, FfiError> {
//         let (name, _prec, params): (String, String, String) = serde_json::from_str(accel_spec)?;
//         let accel_series = series.run_algo(&name, &params, m, n)?;

//         let val_arr = accel_series.sn();
//         let dev_arr = accel_series.deviation();

//         Ok(AccelResult {
//             values: SeriesPointArray::from_vec(&val_arr.to_series_points()),
//             an: SeriesPointArray::from_vec(&accel_series.an().to_series_points()),
//             valid: vec![true; val_arr_len(&val_arr)],
//             deviations: SeriesPointArray::from_vec(&dev_arr.to_series_points()),
//             events: vec![],
//             errors: vec![],
//             filtered_estimates: vec![],
//             profiling: None,
//         })
//     }

//     pub fn series_destroy(&self, _series: Series) {}
//     pub fn accel_destroy(&self, _handle: String) {}
// }

// fn val_arr_len(arr: &Arr) -> usize {
//     match arr {
//         Arr::Real(v) => v.len(),
//         Arr::Complex(v) => v.len(),
//         Arr::Interval(v) => v.len(),
//         Arr::CInterval(v) => v.len(),
//     }
// }

// /// A high-level wrapper around a C++ series object.
// pub struct Series {
//     inner: UniquePtr<ffi::CSeries>,
// }
// impl Series {
//     /// Create a new series instance.
//     pub fn new(
//         name: &str,
//         precision: &str,
//         params_json: &str,
//         n: usize,
//         x: &str,
//     ) -> Result<Self, FfiError> {
//         let inner = ffi::mk_series(name, precision, params_json, n, x)?;
//         Ok(Self { inner })
//     }

//     /// Apply noise to the series and return a new series.
//     pub fn apply_noise(
//         &self,
//         name: &str,
//         params_json: &str,
//         start_n: u64,
//     ) -> Result<Self, FfiError> {
//         let inner = ffi::apply_noise(&self.inner, name, params_json, start_n)?;
//         Ok(Self { inner })
//     }

//     /// Run an acceleration algorithm on the series and return a result series.
//     pub fn run_algo(
//         &self,
//         name: &str,
//         params_json: &str,
//         m: usize,
//         n: usize,
//     ) -> Result<Self, FfiError> {
//         let inner = ffi::run_algo(&self.inner, name, params_json, m, n)?;
//         Ok(Self { inner })
//     }

//     /// Get partial sums Sn.
//     pub fn sn(&self) -> Arr {
//         Arr::from_raw(ffi::get_sn(&self.inner))
//     }

//     /// Get individual terms an.
//     pub fn an(&self) -> Arr {
//         Arr::from_raw(ffi::get_an(&self.inner))
//     }

//     /// Get deviations |Sn - limit|.
//     pub fn deviation(&self) -> Arr {
//         Arr::from_raw(ffi::get_deviation(&self.inner))
//     }

//     /// Get the limit value if known.
//     pub fn limit(&self) -> Value {
//         Value::from_raw(ffi::get_limit(&self.inner))
//     }

//     /// Apply a filter and return the filtered data.
//     pub fn filter(&self, name: &str, params_json: &str, start_n: u64) -> Arr {
//         Arr::from_raw(ffi::filter(&self.inner, name, params_json, start_n))
//     }
// }

// impl Arr {
//     pub fn to_series_points(&self) -> Vec<SeriesPoint> {
//         match self {
//             Arr::Real(v) => v
//                 .iter()
//                 .copied()
//                 .map(Into::into)
//                 .map(SeriesPoint::Real)
//                 .collect(),
//             Arr::Complex(v) => v
//                 .iter()
//                 .copied()
//                 .map(Into::into)
//                 .map(SeriesPoint::Complex)
//                 .collect(),
//             Arr::Interval(v) => v
//                 .iter()
//                 .copied()
//                 .map(Into::into)
//                 .map(SeriesPoint::Interval)
//                 .collect(),
//             Arr::CInterval(v) => v
//                 .iter()
//                 .copied()
//                 .map(Into::into)
//                 .map(SeriesPoint::CInterval)
//                 .collect(),
//         }
//     }
// }

// impl Value {
//     pub fn to_series_point(&self) -> SeriesPoint {
//         match self {
//             Value::Real(v) => SeriesPoint::Real((*v).into()),
//             Value::Complex(v) => SeriesPoint::Complex((*v).into()),
//             Value::Interval(v) => SeriesPoint::Interval((*v).into()),
//             Value::CInterval(v) => SeriesPoint::CInterval((*v).into()),
//         }
//     }
// }

// impl From<ffi::RealValue> for ScientificValue {
//     fn from(rv: ffi::RealValue) -> Self {
//         Self {
//             mantissa: rv.mantissa,
//             exponent: rv.exponent,
//         }
//     }
// }

// impl From<ffi::ComplexValue> for crate::ffi::types::ComplexValue {
//     fn from(cv: ffi::ComplexValue) -> Self {
//         Self {
//             real: cv.real.into(),
//             imag: cv.imag.into(),
//         }
//     }
// }

// impl From<ffi::IntervalValue> for crate::ffi::types::IntervalValue {
//     fn from(iv: ffi::IntervalValue) -> Self {
//         Self {
//             inf: iv.inf.into(),
//             sup: iv.sup.into(),
//         }
//     }
// }

// impl From<ffi::CIntervalValue> for crate::ffi::types::CIntervalValue {
//     fn from(civ: ffi::CIntervalValue) -> Self {
//         Self {
//             real: crate::ffi::types::IntervalValue {
//                 inf: civ.inf.real.into(),
//                 sup: civ.sup.real.into(),
//             },
//             imag: crate::ffi::types::IntervalValue {
//                 inf: civ.inf.imag.into(),
//                 sup: civ.sup.imag.into(),
//             },
//         }
//     }
// }
