fn main() {
    // Use ccache if available
    let ccache = std::process::Command::new("which")
        .arg("ccache")
        .output()
        .map(|o| o.status.success())
        .unwrap_or(false);

    // Skip unnecessary precisions in debug build to save time
    let is_debug = std::env::var("PROFILE").unwrap_or_default() == "debug";

    let mut build = cxx_build::bridge("src/ffi/bridge.rs");

    if ccache {
        unsafe {
            std::env::set_var("CC", "ccache gcc");
            std::env::set_var("CXX", "ccache g++");
        }
    }

    if let Ok(ffi_dir) = std::env::var("SHANKS_PREBUILT_FFI_DIR") {
        println!("cargo:rustc-link-search=native={}", ffi_dir);
        println!("cargo:rustc-link-lib=static=shanks_ffi_cpp");
    } else {
        build
            .file("ffi/src/bridge_core.cpp")
            .file("ffi/src/bridge_f64.cpp")
            .file("ffi/src/bridge_arb.cpp")
            .file("ffi/src/series_registry_impl.cpp");
    }

    build
        .include("ffi/include")
        .include("../backend/core/include")
        .include("/usr/include/eigen3")
        .flag_if_supported("-std=gnu++20")
        .flag_if_supported("-DSHANKS_ENABLE_PROFILING");
    // .flag_if_supported("-O3");

    // if is_debug {
    // build.define("SHANKS_SKIP_PRECISION", None);
    // }

    build.compile("shanks_ffi");

    // Tell cargo when to rebuild
    println!("cargo:rerun-if-changed=src/ffi/bridge.rs");
    println!("cargo:rerun-if-changed=ffi/");
    println!("cargo:rerun-if-changed=../backend/core/include");

    // Link to MPFR/GSL and set RPATH
    let libs = ["mpfr", "gsl"];
    let target_os = std::env::var("CARGO_CFG_TARGET_OS").unwrap_or_default();

    for lib in libs {
        if let Ok(l) = pkg_config::Config::new().atleast_version("0.1").probe(lib) {
            for path in &l.link_paths {
                println!("cargo:rustc-link-search=native={}", path.display());
                println!("cargo:rustc-link-arg=-Wl,-rpath,{}", path.display());
            }

            for lib_name in &l.libs {
                // NixOS / Linux workaround: Force the linker to keep the BLAS library
                // even if it thinks the Rust code isn't directly using it.
                if lib_name.contains("blas") && target_os == "linux" {
                    println!("cargo:rustc-link-arg=-Wl,--no-as-needed");
                    println!("cargo:rustc-link-arg=-l{}", lib_name);
                    println!("cargo:rustc-link-arg=-Wl,--as-needed");
                } else {
                    println!("cargo:rustc-link-lib=dylib={}", lib_name);
                }
            }
        }
    }
    // let libs = ["mpfr", "gsl"];
    // for lib in libs {
    //     if let Ok(l) = pkg_config::Config::new().atleast_version("0.1").probe(lib) {
    //         for path in l.link_paths {
    //             println!("cargo:rustc-link-search=native={}", path.display());
    //             println!("cargo:rustc-link-arg=-Wl,-rpath,{}", path.display());
    //         }

    //         // For GSL, we need to ensure linkage of gslcblas because libgsl depends on it.
    //         // We use force_link.cpp to guarantee this.
    //         for lib_name in l.libs {
    //             println!("cargo:rustc-link-lib=dylib={}", lib_name);
    //         }
    //     }
    // }

    // Also handle libstdc++ which isn't always handled correctly by cxx
    if let Ok(output) = std::process::Command::new("g++")
        .arg("-print-file-name=libstdc++.so.6")
        .output()
    {
        let path_str = String::from_utf8_lossy(&output.stdout).trim().to_string();
        if !path_str.is_empty() && path_str != "libstdc++.so.6" {
            if let Some(parent) = std::path::Path::new(&path_str).parent() {
                println!("cargo:rustc-link-arg=-Wl,-rpath,{}", parent.display());
            }
        }
    }

    println!("cargo:rustc-link-lib=dylib=gmp");
}
