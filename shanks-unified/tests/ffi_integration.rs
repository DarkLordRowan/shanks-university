//! Integration tests for FFI layer

use shanks_unified::ffi::ShanksLibrary;
use std::path::Path;

/// Test that we can load the library and list series
#[test]
fn test_list_series() {
    let lib = ShanksLibrary::load(Path::new("../backend/ffi/build/lib/libshanks_ffi.so"));
    assert!(lib.is_ok(), "Failed to load library: {:?}", lib.err());

    let lib = lib.unwrap();
    let series = lib.list_series();
    assert!(series.is_ok(), "Failed to list series: {:?}", series.err());

    let series = series.unwrap();
    assert!(!series.is_empty(), "Series list should not be empty");
    assert!(
        series.contains(&"Ln2Series".to_string()),
        "Should contain Ln2Series"
    );
}

/// Test that we can list acceleration algorithms
#[test]
fn test_list_accels() {
    let lib = ShanksLibrary::load(Path::new("../backend/ffi/build/lib/libshanks_ffi.so"));
    assert!(lib.is_ok(), "Failed to load library: {:?}", lib.err());

    let lib = lib.unwrap();
    let accels = lib.list_accels();
    assert!(accels.is_ok(), "Failed to list accels: {:?}", accels.err());

    let accels = accels.unwrap();
    assert!(!accels.is_empty(), "Accel list should not be empty");
}

/// Test creating and generating a series
#[test]
fn test_series_create_and_generate() {
    let lib = ShanksLibrary::load(Path::new("../backend/ffi/build/lib/libshanks_ffi.so"));
    assert!(lib.is_ok(), "Failed to load library: {:?}", lib.err());

    let lib = lib.unwrap();

    // Create series (name, precision, x_value, params_json)
    // Use "F64" precision as in the C++ test
    let handle = lib.series_create("Ln2Series", "F64", "1.0", "{}");
    assert!(
        handle.is_ok(),
        "Failed to create series: {:?}",
        handle.err()
    );

    let handle = handle.unwrap();

    // Generate series
    let result = lib.series_generate(&handle, 10);
    assert!(
        result.is_ok(),
        "Failed to generate series: {:?}",
        result.err()
    );

    let result = result.unwrap();
    assert!(!result.sn.is_empty(), "Series result sn should not be empty");
    assert!(!result.an.is_empty(), "Series result an should not be empty");

    // Cleanup
    lib.series_destroy(handle);
}

/// Test creating an acceleration algorithm
#[test]
fn test_accel_create() {
    let lib = ShanksLibrary::load(Path::new("../backend/ffi/build/lib/libshanks_ffi.so"));
    assert!(lib.is_ok(), "Failed to load library: {:?}", lib.err());

    let lib = lib.unwrap();

    // Create series first (name, precision, x_value, params_json)
    let series_handle = lib.series_create("Ln2Series", "F64", "1.0", "{}");
    assert!(
        series_handle.is_ok(),
        "Failed to create series: {:?}",
        series_handle.err()
    );

    let series_handle = series_handle.unwrap();

    // Generate series
    let gen_result = lib.series_generate(&series_handle, 10);
    assert!(
        gen_result.is_ok(),
        "Failed to generate series: {:?}",
        gen_result.err()
    );

    // Create acceleration algorithm (name, precision, params_json)
    // Use "Shanks Transformation" as in the C++ test
    let accel_handle = lib.accel_create("Shanks Transformation", "F64", "{}");
    assert!(
        accel_handle.is_ok(),
        "Failed to create accel: {:?}",
        accel_handle.err()
    );

    let accel_handle = accel_handle.unwrap();

    // Apply acceleration (accel, series, n, order)
    let accel_result = lib.accel_apply(&accel_handle, &series_handle, 10, 5);
    assert!(
        accel_result.is_ok(),
        "Failed to apply accel: {:?}",
        accel_result.err()
    );

    let accel_result = accel_result.unwrap();
    eprintln!("Acceleration result nodes: {}", accel_result.values.len());
    assert!(
        !accel_result.values.is_empty(),
        "Acceleration result should not be empty"
    );

    // Cleanup
    lib.accel_destroy(accel_handle);
    lib.series_destroy(series_handle);
}
