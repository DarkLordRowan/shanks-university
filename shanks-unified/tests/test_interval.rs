use shanks_unified::ffi::types::{AccelResult, SeriesPoint, SeriesResult};
use shanks_unified::ffi::ShanksLibrary;
use std::path::Path;

#[test]
fn test_interval_ffi_integration() {
    let lib = ShanksLibrary::load(Path::new("../backend/ffi/build/lib/libshanks_ffi.so"))
        .expect("Failed to load libshanks_ffi.so");

    // Create Interval series (name, precision, x_value, params_json)
    let handle = lib
        .series_create("Ln2Series", "IntervalF64", "1.0", "{}")
        .expect("Failed to create IntervalF64 series");

    // Generate series
    let result_json = lib
        .series_generate(&handle, 10, false)
        .expect("Failed to generate series");

    // Parse it using the typed structs handling Interval parsing
    println!("{}", result_json);
    let parsed: SeriesResult =
        serde_json::from_str(&result_json).expect("Failed to deserialize SeriesResult JSON");

    // Verify it contains interval points
    assert!(!parsed.sn.is_empty(), "Series result should not be empty");

    let first_point = parsed.sn.get(0);
    match first_point {
        SeriesPoint::Interval(i) => {
            // interval bounds should be finite
            assert!(i.inf.to_f64().is_finite());
            assert!(i.sup.to_f64().is_finite());
            assert!(i.inf.to_f64() <= i.sup.to_f64(), "inf should be <= sup");
        }
        _ => panic!("Expected Interval point, got something else"),
    }

    // Test acceleration with Interval precision
    let accel_handle = lib
        .accel_create("Shanks Transformation", "IntervalF64", "{}")
        .expect("Failed to create accel handle");

    let accel_result = lib
        .accel_apply(&accel_handle, &handle, 10, 5, true)
        .expect("Failed to apply acceleration");

    println!("{}", result_json);
    let parsed_accel: AccelResult =
        serde_json::from_str(&accel_result).expect("Failed to deserialize AccelResult JSON");
    assert!(
        !parsed_accel.values.is_empty(),
        "Accel results should not be empty"
    );

    // Verify at least one valid computation exists and is an Interval
    let mut has_interval = false;
    for i in 0..parsed_accel.values.len() {
        if parsed_accel.valid.get(i).copied().unwrap_or(false) {
            let v = parsed_accel.values.get(i);
            if matches!(v, SeriesPoint::Interval(_)) {
                has_interval = true;
                break;
            }
        }
    }
    assert!(
        has_interval,
        "Expected at least one Interval result in acceleration values"
    );

    // Cleanup
    lib.accel_destroy(accel_handle);
    lib.series_destroy(handle);
}
