#[cxx::bridge(namespace = "shanks::ffi::bridge")]
pub mod ffi {
    #[derive(Clone, Copy, Debug, PartialEq)]
    enum ValueKind {
        Real,
        Complex,
        Interval,
        CInterval,
    }

    #[derive(Clone, Copy, Debug, PartialEq)]
    struct RealValue {
        mantissa: f64,
        exponent: i64,
    }

    #[derive(Clone, Copy, Debug, PartialEq)]
    struct ComplexValue {
        real: RealValue,
        imag: RealValue,
    }
    #[derive(Clone, Copy, Debug, PartialEq)]
    struct IntervalValue {
        inf: RealValue,
        sup: RealValue,
    }
    #[derive(Clone, Copy, Debug, PartialEq)]
    struct CIntervalValue {
        inf: ComplexValue,
        sup: ComplexValue,
    }

    #[derive(Clone, Copy, Debug, PartialEq)]
    struct RawValue {
        tag: ValueKind,
        r1: RealValue,
        r2: RealValue,
        r3: RealValue,
        r4: RealValue,
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

        fn list_series() -> Vec<String>;

        fn list_accels() -> Vec<String>;
        fn list_precisions() -> Vec<String>;
        fn list_noises() -> Vec<String>;
        fn list_noise_methods() -> Vec<String>;
    }
}
