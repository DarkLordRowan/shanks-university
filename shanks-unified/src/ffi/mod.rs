//! FFI bindings to the C++ Shanks library.
//!
//! This module provides safe Rust wrappers around the C FFI interface
//! defined in `backend/ffi/include/shanks_ffi.hpp`.

pub mod types;

pub use types::*;

use libloading::{Library, Symbol};
use std::ffi::{CStr, CString};
use std::path::Path;
use std::collections::HashMap;

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
                unsafe extern "C" fn(*const i8, *const i8, *const i8, *const i8) -> *mut std::ffi::c_void,
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
                let err = self.last_error().unwrap_or_else(|| "Unknown error".to_string());
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
        noise_json: &str,
    ) -> Result<ShanksSeriesHandle, FfiError> {
        unsafe {
            let func: Symbol<
                unsafe extern "C" fn(*const i8, *const i8, *const i8, *const i8, *const i8) -> *mut std::ffi::c_void,
            > = self.library.get(b"shanks_series_create_with_noise")?;

            let name_c = CString::new(name)?;
            let precision_c = CString::new(precision)?;
            let x_value_c = CString::new(x_value)?;
            let args_json = CString::new(params_json)?;
            let noise_c = CString::new(noise_json)?;

            let handle = func(
                name_c.as_ptr(),
                precision_c.as_ptr(),
                x_value_c.as_ptr(),
                args_json.as_ptr(),
                noise_c.as_ptr(),
            );

            if handle.is_null() {
                let err = self.last_error().unwrap_or_else(|| "Unknown error".to_string());
                return Err(FfiError::LibraryError(err));
            }

            Ok(ShanksSeriesHandle { handle })
        }
    }

    /// Generate series data (returns JSON string).
    pub fn series_generate(
        &self,
        handle: &ShanksSeriesHandle,
        n: u64,
        enable_profiling: bool,
    ) -> Result<String, FfiError> {
        unsafe {
            let func: Symbol<unsafe extern "C" fn(*mut std::ffi::c_void, u64, i32) -> *mut i8> =
                self.library.get(b"shanks_series_generate")?;

            let ptr = func(handle.handle, n, if enable_profiling { 1 } else { 0 });
            if ptr.is_null() {
                let err = self.last_error().unwrap_or_else(|| "Unknown error".to_string());
                return Err(FfiError::LibraryError(err));
            }

            let json_str = CStr::from_ptr(ptr).to_str()?.to_string();
            self.free_string(ptr);
            Ok(json_str)
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
            if let Ok(func) = self.library.get::<Symbol<unsafe extern "C" fn(*mut std::ffi::c_void)>>(
                b"shanks_series_destroy",
            ) {
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
                let err = self.last_error().unwrap_or_else(|| "Unknown error".to_string());
                return Err(FfiError::LibraryError(err));
            }

            Ok(ShanksAccelHandle { handle })
        }
    }

    /// Apply acceleration algorithm to series (returns JSON string).
    pub fn accel_apply(
        &self,
        accel: &ShanksAccelHandle,
        series: &ShanksSeriesHandle,
        n: u64,
        order: u64,
    ) -> Result<String, FfiError> {
        unsafe {
            let func: Symbol<
                unsafe extern "C" fn(*mut std::ffi::c_void, *mut std::ffi::c_void, u64, u64) -> *mut i8,
            > = self.library.get(b"shanks_accel_apply")?;

            let ptr = func(accel.handle, series.handle, n, order);
            if ptr.is_null() {
                let err = self.last_error().unwrap_or_else(|| "Unknown error".to_string());
                return Err(FfiError::LibraryError(err));
            }

            let json_str = CStr::from_ptr(ptr).to_str()?.to_string();
            self.free_string(ptr);
            Ok(json_str)
        }
    }

    /// Destroy an acceleration handle.
    pub fn accel_destroy(&self, handle: ShanksAccelHandle) {
        unsafe {
            if let Ok(func) = self.library.get::<Symbol<unsafe extern "C" fn(*mut std::ffi::c_void)>>(
                b"shanks_accel_destroy",
            ) {
                func(handle.handle);
            }
        }
    }

    /// Free a string allocated by the library.
    fn free_string(&self, ptr: *mut i8) {
        unsafe {
            if let Ok(func) = self.library.get::<Symbol<unsafe extern "C" fn(*mut i8)>>(
                b"shanks_free_string",
            ) {
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
        Self { handle: self.handle }
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
        Self { handle: self.handle }
    }
}
