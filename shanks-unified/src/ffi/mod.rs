//! FFI bindings to the C++ Shanks library.
//!
//! This module provides safe Rust wrappers around the C FFI interface
//! defined in `backend/ffi/include/shanks_ffi.hpp`.

pub mod types;

pub use types::*;

use libloading::{Library, Symbol};
use std::ffi::{CStr, CString};
use std::path::Path;

/// Error type for FFI operations.
#[derive(Debug, thiserror::Error)]
pub enum FfiError {
    #[error("Failed to load library: {0}")]
    LibraryLoad(#[from] libloading::Error),

    #[error("Failed to get symbol: {0}")]
    SymbolNotFound(String),

    #[error("Null pointer returned")]
    NullPointer,

    #[error("Invalid UTF-8 string")]
    InvalidUtf8(#[from] std::str::Utf8Error),

    #[error("JSON error: {0}")]
    JsonError(#[from] serde_json::Error),

    #[error("Library error: {0}")]
    LibraryError(String),

    #[error("Nul error: {0}")]
    NulError(#[from] std::ffi::NulError),
}

/// Handle to the loaded Shanks library.
pub struct ShanksLibrary {
    library: Library,
}

impl ShanksLibrary {
    /// Load the Shanks library from the given path.
    pub fn load(path: &Path) -> Result<Self, FfiError> {
        let library = unsafe { Library::new(path)? };
        Ok(Self { library })
    }

    /// Try to find the library path automatically in common locations.
    pub fn find_library() -> Option<std::path::PathBuf> {
        let lib_names = if cfg!(target_os = "windows") {
            vec!["shanks_ffi.dll", "libshanks_ffi.dll"]
        } else if cfg!(target_os = "macos") {
            vec!["libshanks_ffi.dylib"]
        } else {
            vec!["libshanks_ffi.so"]
        };

        // 1. Check environment variable
        if let Ok(env_path) = std::env::var("SHANKSLIB_PATH") {
            let path = std::path::PathBuf::from(env_path);
            if path.exists() {
                return Some(path);
            }
        }

        // 2. Check current working directory
        if let Ok(cwd) = std::env::current_dir() {
            for lib_name in &lib_names {
                let lib_path = cwd.join(lib_name);
                if lib_path.exists() {
                    return Some(lib_path);
                }
            }
        }

        // 3. Check next to the executable
        if let Ok(exe_path) = std::env::current_exe() {
            if let Some(exe_dir) = exe_path.parent() {
                for lib_name in &lib_names {
                    let lib_path = exe_dir.join(lib_name);
                    if lib_path.exists() {
                        return Some(lib_path);
                    }
                }
            }
        }

        None
    }

    /// Get the library version.
    pub fn version(&self) -> Result<&'static str, FfiError> {
        unsafe {
            let func: Symbol<unsafe extern "C" fn() -> *const i8> =
                self.library.get(b"shanks_get_version")?;
            let ptr = func();
            if ptr.is_null() {
                return Err(FfiError::NullPointer);
            }
            Ok(CStr::from_ptr(ptr).to_str()?)
        }
    }

    /// Get the last error message.
    pub fn last_error(&self) -> Option<String> {
        unsafe {
            let func: Symbol<unsafe extern "C" fn() -> *const i8> =
                self.library.get(b"shanks_last_error").ok()?;
            let ptr = func();
            if ptr.is_null() {
                return None;
            }
            CStr::from_ptr(ptr).to_str().ok().map(|s| s.to_string())
        }
    }

    /// Get list of available series names.
    pub fn list_series(&self) -> Result<Vec<String>, FfiError> {
        unsafe {
            let func: Symbol<unsafe extern "C" fn() -> *mut i8> =
                self.library.get(b"shanks_list_series")?;
            let ptr = func();
            if ptr.is_null() {
                return Err(FfiError::NullPointer);
            }
            let json_str = CStr::from_ptr(ptr).to_str()?;
            let result: Vec<String> = serde_json::from_str(json_str)?;
            self.free_string(ptr);
            Ok(result)
        }
    }

    /// Get list of available acceleration algorithms.
    pub fn list_accels(&self) -> Result<Vec<String>, FfiError> {
        unsafe {
            let func: Symbol<unsafe extern "C" fn() -> *mut i8> =
                self.library.get(b"shanks_list_accels")?;
            let ptr = func();
            if ptr.is_null() {
                return Err(FfiError::NullPointer);
            }
            let json_str = CStr::from_ptr(ptr).to_str()?;
            let result: Vec<String> = serde_json::from_str(json_str)?;
            self.free_string(ptr);
            Ok(result)
        }
    }

    /// Get list of supported precision types.
    pub fn list_precisions(&self) -> Result<Vec<String>, FfiError> {
        unsafe {
            let func: Symbol<unsafe extern "C" fn() -> *mut i8> =
                self.library.get(b"shanks_list_precisions")?;
            let ptr = func();
            if ptr.is_null() {
                return Err(FfiError::NullPointer);
            }
            let json_str = CStr::from_ptr(ptr).to_str()?;
            let result: Vec<String> = serde_json::from_str(json_str)?;
            self.free_string(ptr);
            Ok(result)
        }
    }

    /// Get list of available noise types.
    pub fn list_noises(&self) -> Result<Vec<String>, FfiError> {
        unsafe {
            let func: Symbol<unsafe extern "C" fn() -> *mut i8> =
                self.library.get(b"shanks_list_noises")?;
            let ptr = func();
            if ptr.is_null() {
                return Err(FfiError::NullPointer);
            }
            let json_str = CStr::from_ptr(ptr).to_str()?;
            let result: Vec<String> = serde_json::from_str(json_str)?;
            self.free_string(ptr);
            Ok(result)
        }
    }

    /// Get list of available noise methods.
    pub fn list_noise_methods(&self) -> Result<Vec<String>, FfiError> {
        unsafe {
            let func: Symbol<unsafe extern "C" fn() -> *mut i8> =
                self.library.get(b"shanks_list_noise_methods")?;
            let ptr = func();
            if ptr.is_null() {
                return Err(FfiError::NullPointer);
            }
            let json_str = CStr::from_ptr(ptr).to_str()?;
            let result: Vec<String> = serde_json::from_str(json_str)?;
            self.free_string(ptr);
            Ok(result)
        }
    }

    /// Get noise type metadata.
    pub fn get_noise_info(&self, name: &str) -> Result<NoiseInfo, FfiError> {
        unsafe {
            let func: Symbol<unsafe extern "C" fn(*const i8) -> *mut i8> =
                self.library.get(b"shanks_get_noise_info")?;
            let name_c = CString::new(name)?;
            let ptr = func(name_c.as_ptr());
            if ptr.is_null() {
                return Err(FfiError::NullPointer);
            }
            let json_str = CStr::from_ptr(ptr).to_str()?;
            let result: NoiseInfo = serde_json::from_str(json_str)?;
            self.free_string(ptr);
            Ok(result)
        }
    }

    /// Get series metadata.
    pub fn get_series_info(&self, name: &str) -> Result<SeriesInfo, FfiError> {
        unsafe {
            let func: Symbol<unsafe extern "C" fn(*const i8) -> *mut i8> =
                self.library.get(b"shanks_get_series_info")?;
            let name_c = CString::new(name)?;
            let ptr = func(name_c.as_ptr());
            if ptr.is_null() {
                return Err(FfiError::NullPointer);
            }
            let json_str = CStr::from_ptr(ptr).to_str()?;
            let result: SeriesInfo = serde_json::from_str(json_str)?;
            self.free_string(ptr);
            Ok(result)
        }
    }

    /// Get acceleration algorithm metadata.
    pub fn get_accel_info(&self, name: &str) -> Result<AccelInfo, FfiError> {
        unsafe {
            let func: Symbol<unsafe extern "C" fn(*const i8) -> *mut i8> =
                self.library.get(b"shanks_get_accel_info")?;
            let name_c = CString::new(name)?;
            let ptr = func(name_c.as_ptr());
            if ptr.is_null() {
                return Err(FfiError::NullPointer);
            }
            let json_str = CStr::from_ptr(ptr).to_str()?;
            let result: AccelInfo = serde_json::from_str(json_str)?;
            self.free_string(ptr);
            Ok(result)
        }
    }

    /// Create a series instance.
    pub fn series_create(
        &self,
        name: &str,
        precision: &str,
        x_value: &str,
        params_json: &str,
    ) -> Result<ShanksSeriesHandle, FfiError> {
        unsafe {
            let func: Symbol<
                unsafe extern "C" fn(
                    *const i8,
                    *const i8,
                    *const i8,
                    *const i8,
                ) -> *mut std::ffi::c_void,
            > = self.library.get(b"shanks_series_create")?;

            let name_c = CString::new(name)?;
            let precision_c = CString::new(precision)?;
            let x_value_c = CString::new(x_value)?;
            let args_json = CString::new(params_json)?;

            let handle = func(
                name_c.as_ptr(),
                precision_c.as_ptr(),
                x_value_c.as_ptr(),
                args_json.as_ptr(),
            );

            if handle.is_null() {
                let err = self
                    .last_error()
                    .unwrap_or_else(|| "Unknown error".to_string());
                return Err(FfiError::LibraryError(err));
            }

            Ok(ShanksSeriesHandle { handle })
        }
    }

    /// Create a series instance with noise.
    pub fn series_create_with_noise(
        &self,
        name: &str,
        precision: &str,
        x_value: &str,
        params_json: &str,
        noise_type: &str,
        noise_method: &str,
        param1: f64,
        param2: f64,
        seed: u64,
    ) -> Result<ShanksSeriesHandle, FfiError> {
        unsafe {
            let func: Symbol<
                unsafe extern "C" fn(
                    *const i8,
                    *const i8,
                    *const i8,
                    *const i8,
                    *const i8,
                    *const i8,
                    f64,
                    f64,
                    u64,
                ) -> *mut std::ffi::c_void,
            > = self.library.get(b"shanks_series_create_with_noise")?;

            let name_c = CString::new(name)?;
            let precision_c = CString::new(precision)?;
            let x_value_c = CString::new(x_value)?;
            let args_json = CString::new(params_json)?;
            let type_c = CString::new(noise_type)?;
            let method_c = CString::new(noise_method)?;

            let handle = func(
                name_c.as_ptr(),
                precision_c.as_ptr(),
                x_value_c.as_ptr(),
                args_json.as_ptr(),
                type_c.as_ptr(),
                method_c.as_ptr(),
                param1,
                param2,
                seed,
            );

            if handle.is_null() {
                let err = self
                    .last_error()
                    .unwrap_or_else(|| "Unknown error".to_string());
                return Err(FfiError::LibraryError(err));
            }

            Ok(ShanksSeriesHandle { handle })
        }
    }

    /// Generate series data (returns the structural result directly).
    pub fn series_generate(
        &self,
        handle: &ShanksSeriesHandle,
        n: u64,
    ) -> Result<SeriesResult, FfiError> {
        unsafe {
            let func: Symbol<unsafe extern "C" fn(*mut std::ffi::c_void, u64) -> *mut FfiSeriesResult> =
                self.library.get(b"shanks_series_generate")?;

            let ptr = func(handle.handle, n);
            if ptr.is_null() {
                let err = self
                    .last_error()
                    .unwrap_or_else(|| "Unknown error".to_string());
                return Err(FfiError::LibraryError(err));
            }

            let ffi_res = *ptr;
            
            // Convert to native safe types
            let sn = ffi_res.sn.to_series_point_array();
            let an = ffi_res.an.to_series_point_array();
            let deviations = ffi_res.deviations.lines[0].to_scientific_array();

            let sum = if ffi_res.has_sum != 0 {
                match ffi_res.sum_type {
                    0 => Some(SeriesPoint::Real(ScientificValue {
                        mantissa: ffi_res.sum_m[0],
                        exponent: ffi_res.sum_e[0],
                    })),
                    1 => Some(SeriesPoint::Complex(ComplexValue {
                        real: ScientificValue { mantissa: ffi_res.sum_m[0], exponent: ffi_res.sum_e[0] },
                        imag: ScientificValue { mantissa: ffi_res.sum_m[1], exponent: ffi_res.sum_e[1] },
                    })),
                    2 => Some(SeriesPoint::Interval(IntervalValue {
                        inf: ScientificValue { mantissa: ffi_res.sum_m[0], exponent: ffi_res.sum_e[0] },
                        sup: ScientificValue { mantissa: ffi_res.sum_m[1], exponent: ffi_res.sum_e[1] },
                    })),
                    3 => Some(SeriesPoint::CInterval(CIntervalValue {
                        real: IntervalValue {
                            inf: ScientificValue { mantissa: ffi_res.sum_m[0], exponent: ffi_res.sum_e[0] },
                            sup: ScientificValue { mantissa: ffi_res.sum_m[1], exponent: ffi_res.sum_e[1] },
                        },
                        imag: IntervalValue {
                            inf: ScientificValue { mantissa: ffi_res.sum_m[2], exponent: ffi_res.sum_e[2] },
                            sup: ScientificValue { mantissa: ffi_res.sum_m[3], exponent: ffi_res.sum_e[3] },
                        },
                    })),
                    _ => None,
                }
            } else {
                None
            };
            
            let result = SeriesResult { sn, an, sum, deviations };

            // Free the result struct
            let free_func: Symbol<unsafe extern "C" fn(*mut FfiSeriesResult)> =
                self.library.get(b"shanks_series_result_free")?;
            free_func(ptr);

            Ok(result)
        }
    }

    /// Get the sum of a series.
    pub fn series_get_sum(&self, handle: &ShanksSeriesHandle) -> Result<String, FfiError> {
        unsafe {
            let func: Symbol<unsafe extern "C" fn(*mut std::ffi::c_void) -> *mut i8> =
                self.library.get(b"shanks_series_get_sum")?;

            let ptr = func(handle.handle);
            if ptr.is_null() {
                return Ok(String::new());
            }

            let sum = CStr::from_ptr(ptr).to_str()?.to_string();
            self.free_string(ptr);
            Ok(sum)
        }
    }

    /// Destroy a series handle.
    pub fn series_destroy(&self, handle: ShanksSeriesHandle) {
        unsafe {
            if let Ok(func) = self
                .library
                .get::<Symbol<unsafe extern "C" fn(*mut std::ffi::c_void)>>(
                    b"shanks_series_destroy",
                )
            {
                func(handle.handle);
            }
        }
    }

    /// Create an acceleration algorithm instance.
    pub fn accel_create(
        &self,
        name: &str,
        precision: &str,
        params_json: &str,
    ) -> Result<ShanksAccelHandle, FfiError> {
        unsafe {
            let func: Symbol<
                unsafe extern "C" fn(*const i8, *const i8, *const i8) -> *mut std::ffi::c_void,
            > = self.library.get(b"shanks_accel_create")?;

            let name_c = CString::new(name)?;
            let precision_c = CString::new(precision)?;
            let args_json = CString::new(params_json)?;

            let handle = func(name_c.as_ptr(), precision_c.as_ptr(), args_json.as_ptr());

            if handle.is_null() {
                let err = self
                    .last_error()
                    .unwrap_or_else(|| "Unknown error".to_string());
                return Err(FfiError::LibraryError(err));
            }

            Ok(ShanksAccelHandle { handle })
        }
    }

    /// Apply acceleration algorithm to a series (returns the structural result directly).
    pub fn accel_apply(
        &self,
        accel: &ShanksAccelHandle,
        series: &ShanksSeriesHandle,
        n: u64,
        order: u64,
    ) -> Result<AccelResult, FfiError> {
        unsafe {
            let func: Symbol<
                unsafe extern "C" fn(
                    *mut std::ffi::c_void,
                    *mut std::ffi::c_void,
                    u64,
                    u64,
                ) -> *mut FfiAccelResult,
            > = self.library.get(b"shanks_accel_apply")?;

            let ptr = func(
                accel.handle,
                series.handle,
                n,
                order,
            );

            if ptr.is_null() {
                let err = self
                    .last_error()
                    .unwrap_or_else(|| "Unknown error".to_string());
                return Err(FfiError::LibraryError(err));
            }

            let ffi_res = *ptr;
            
            // Convert to native safe types
            let values = ffi_res.values.to_series_point_array();
            let deviations = ffi_res.deviations.to_scientific_array();

            let result = AccelResult {
                values,
                valid: vec![true; n as usize], // TODO: Re-introduce valid tracker in binary FFI later if needed or derive from values
                deviations,
                events: Vec::new(),
                errors: Vec::new(),
                filtered_estimates: Vec::new(),
                profiling: None,
            };

            // Free the result struct
            let free_func: Symbol<unsafe extern "C" fn(*mut FfiAccelResult)> =
                self.library.get(b"shanks_accel_result_free")?;
            free_func(ptr);

            Ok(result)
        }
    }

    /// Destroy an acceleration handle.
    pub fn accel_destroy(&self, handle: ShanksAccelHandle) {
        unsafe {
            if let Ok(func) = self
                .library
                .get::<Symbol<unsafe extern "C" fn(*mut std::ffi::c_void)>>(b"shanks_accel_destroy")
            {
                func(handle.handle);
            }
        }
    }

    /// Compute a smoothed limit from a divergent tail.
    pub fn compute_smoothed_limit(
        &self,
        precision: &str,
        values: &[String],
        filter_type: &str,
        args_json: &str,
    ) -> Result<String, FfiError> {
        unsafe {
            let func: Symbol<
                unsafe extern "C" fn(
                    *const i8,
                    *const *const i8,
                    u64,
                    *const i8,
                    *const i8,
                ) -> *mut i8,
            > = self.library.get(b"shanks_compute_smoothed_limit")?;

            let precision_c = CString::new(precision)?;
            let filter_type_c = CString::new(filter_type)?;
            let args_json_c = CString::new(args_json)?;
            
            let mut c_strings: Vec<CString> = Vec::with_capacity(values.len());
            let mut c_ptrs: Vec<*const i8> = Vec::with_capacity(values.len());
            
            for v in values {
                let c_str = CString::new(v.as_str())?;
                c_ptrs.push(c_str.as_ptr());
                c_strings.push(c_str);
            }

            let ptr = func(
                precision_c.as_ptr(),
                c_ptrs.as_ptr(),
                values.len() as u64,
                filter_type_c.as_ptr(),
                args_json_c.as_ptr(),
            );
            
            if ptr.is_null() {
                let err = self
                    .last_error()
                    .unwrap_or_else(|| "Unknown error".to_string());
                return Err(FfiError::LibraryError(err));
            }

            let json_str = CStr::from_ptr(ptr).to_str()?.to_string();
            self.free_string(ptr);
            Ok(json_str)
        }
    }

    /// Free a string allocated by the library.
    fn free_string(&self, ptr: *mut i8) {
        unsafe {
            if let Ok(func) = self
                .library
                .get::<Symbol<unsafe extern "C" fn(*mut i8)>>(b"shanks_free_string")
            {
                func(ptr);
            }
        }
    }
}

/// Handle to a series instance (for compute engine).
pub struct ShanksSeriesHandle {
    handle: *mut std::ffi::c_void,
}

// Safety: The handle is just a pointer that we manage carefully
unsafe impl Send for ShanksSeriesHandle {}
unsafe impl Sync for ShanksSeriesHandle {}

impl Clone for ShanksSeriesHandle {
    fn clone(&self) -> Self {
        Self {
            handle: self.handle,
        }
    }
}

/// Handle to an acceleration algorithm instance (for compute engine).
pub struct ShanksAccelHandle {
    handle: *mut std::ffi::c_void,
}

unsafe impl Send for ShanksAccelHandle {}
unsafe impl Sync for ShanksAccelHandle {}

impl Clone for ShanksAccelHandle {
    fn clone(&self) -> Self {
        Self {
            handle: self.handle,
        }
    }
}
