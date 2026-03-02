use serde::{Deserialize, Serialize};

#[cfg(not(target_endian = "little"))]
compile_error!(
    "RealValue serialization is little-endian, please implement proper serialization/deserialization."
);

impl ffi::RealValue {
    #[cfg(target_endian = "little")]
    pub fn to_bytes(slice: &[ffi::RealValue]) -> &[u8] {
        let byte_len = slice.len() * std::mem::size_of::<ffi::RealValue>();
        let ptr = slice.as_ptr() as *const u8;
        unsafe { std::slice::from_raw_parts(ptr, byte_len) }
    }

    #[cfg(target_endian = "little")]
    pub fn from_bytes(bytes: &[u8]) -> Vec<ffi::RealValue> {
        let size = std::mem::size_of::<ffi::RealValue>();
        assert_eq!(
            bytes.len() % size,
            0,
            "Bytes length must be a multiple of 16"
        );

        let target_len = bytes.len() / size;
        let mut vec = Vec::with_capacity(target_len);

        // SAFETY: We copy exactly `bytes.len()` bytes into the newly allocated
        // vector. Because `ffi::RealValue` is Copy, a bitwise copy is perfectly valid.
        unsafe {
            std::ptr::copy_nonoverlapping(bytes.as_ptr(), vec.as_mut_ptr() as *mut u8, bytes.len());
            vec.set_len(target_len);
        }

        vec
    }
}

#[cxx::bridge(namespace = "shanks::ffi::bridge")]
pub mod ffi {
    #[derive(Clone, Copy, Debug, PartialEq, Serialize, Deserialize)]
    struct RealValue {
        mantissa: f64,
        exponent: i64,
    }

    #[derive(Clone, Copy, Debug, PartialEq)]
    enum ValueKind {
        Real,
        Complex,
        Interval,
        CInterval,
    }

    #[derive(Clone, Copy, Debug, PartialEq)]
    struct RawValue {
        tag: ValueKind,
        r1: RealValue, // real, inf
        r2: RealValue, // real, sup
        r3: RealValue, // imag, inf
        r4: RealValue, // imag, sup
    }

    #[derive(Clone, Copy, Debug, PartialEq)]
    enum ArrKind {
        Real,
        Complex,
        Interval,
        CInterval,
    }

    #[derive(Clone, Debug)]
    struct RawArr {
        tag: ArrKind,
        r1: Vec<RealValue>,
        r2: Vec<RealValue>,
        r3: Vec<RealValue>,
        r4: Vec<RealValue>,
    }

    unsafe extern "C++" {
        include!("shanks-unified/ffi/include/bridge.hpp");

        type CSeries;

        fn mk_series(
            name: &str,
            precision: &str,
            params_json: &str,
            n: usize,
            x: &str,
        ) -> Result<UniquePtr<CSeries>>;

        fn apply_noise(
            series: &CSeries,
            name: &str,
            params_json: &str,
            start_n: u64,
        ) -> Result<UniquePtr<CSeries>>;
        fn run_algo(
            series: &CSeries,
            name: &str,
            params_json: &str,
            m: usize,
            n: usize,
        ) -> Result<UniquePtr<CSeries>>;

        fn get_sn(series: &CSeries) -> RawArr;
        fn get_an(series: &CSeries) -> RawArr;
        fn get_deviation(series: &CSeries) -> RawArr;
        fn get_limit(series: &CSeries) -> RawValue;

        fn filter(series: &CSeries, name: &str, params_json: &str, start_n: u64) -> RawArr;

        /// Returns events from the last `run_algo` call on this series.
        /// Each entry is tab-separated: "n\tname\tdescription".
        fn get_events(series: &CSeries) -> Vec<String>;

        fn list_series() -> Vec<String>;

        fn list_accels() -> Vec<String>;
        fn list_precisions() -> Vec<String>;
        fn list_noises() -> Vec<String>;
        fn list_noise_methods() -> Vec<String>;
    }
}
