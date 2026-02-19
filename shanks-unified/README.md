# Shanks Unified

A unified Rust application for series acceleration computation and visualization.

## Overview

Shanks Unified replaces the Python runner + old-vizr combination with a single Rust binary that:
- Calls the C++ series acceleration library via FFI
- Caches results in SQLite
- Provides an interactive GUI for visualization
- Supports headless batch computation mode
- Uses experiment configuration files as the source of settings

## Prerequisites

- C++ compiler with C++20 support
- CMake 3.11+
- Rust 1.70+
- GMP, MPFR, Eigen3, GSL libraries

### Installing Dependencies (Ubuntu/Debian)

```bash
sudo apt install build-essential cmake libgmp-dev libmpfr-dev libeigen3-dev libgsl-dev
```

### Installing Dependencies (Fedora)

```bash
sudo dnf install gcc-c++ cmake gmp-devel mpfr-devel eigen3-devel gsl-devel
```

## Building

### 1. Build the C++ FFI Library

```bash
cd backend/ffi
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

This produces `backend/ffi/build/lib/libshanks_ffi.so`.

### 2. Build the Rust Application

```bash
cd shanks-unified
cargo build --release
```

## Running

### GUI Mode (Default)

```bash
cd shanks-unified
cargo run --release -- gui \
  --lib-path $(realpath ../backend/ffi/build/lib/libshanks_ffi.so)
```

### Headless Mode

Run batch computations without UI:

```bash
cargo run --release -- headless \
  --config ../backend/runner/config/test.json \
  --lib-path $(realpath ../backend/ffi/build/lib/libshanks_ffi.so)
```

### List Available Options

```bash
# List series from library
cargo run --release -- list series --lib-path ../backend/ffi/build/lib/libshanks_ffi.so

# List acceleration algorithms
cargo run --release -- list accels --lib-path ../backend/ffi/build/lib/libshanks_ffi.so

# List precision types
cargo run --release -- list precisions --lib-path ../backend/ffi/build/lib/libshanks_ffi.so

# List noise types
cargo run --release -- list noises --lib-path ../backend/ffi/build/lib/libshanks_ffi.so
```

### Command Line Options

| Option | Short | Description | Default |
|--------|-------|-------------|---------|
| `--lib-path` | `-l` | Path to libshanks_ffi.so | None (offline mode) |
| `--db-path` | `-d` | Path to SQLite database | `shanks.db` |
| `--verbose` | `-v` | Increase logging (use multiple times) | Warn level |

## Experiment Configuration

The application uses JSON configuration files as the source of settings. These files define:
- Series to compute with parameter ranges
- Acceleration methods with n and m values
- Noise configurations
- Precision types

### Example Configuration

```json
{
  "series": [
    {
      "name": "CosSeries",
      "args": {
        "x": [0.1, 0.5, 1.0]
      }
    },
    {
      "name": "ExpSeries",
      "args": {
        "x": {"start": 0.1, "stop": 1.0, "step": 0.1}
      }
    }
  ],
  "methods": [
    {
      "name": "Shanks",
      "n": {"start": 10, "stop": 100, "step": 10},
      "m": [2, 4, 6]
    },
    {
      "name": "WynnEpsilon",
      "n": [50, 100, 200],
      "m": [1, 2, 3]
    }
  ],
  "noises": [
    {
      "type": "Normal",
      "method": "jitter",
      "param1": 0.0,
      "param2": 0.01,
      "seed": 42
    }
  ],
  "precisions": ["F64", "F128"]
}
```

### Configuration Fields

#### Series Definition

| Field | Type | Description |
|-------|------|-------------|
| `name` | string | Series name from registry |
| `args` | object | Constructor arguments (supports arrays and ranges) |
| `vec_size` | number | Optional: number of terms override |

#### Method Definition

| Field | Type | Description |
|-------|------|-------------|
| `name` | string | Method name from registry |
| `n` | array or range | Number of terms values |
| `m` | array | Order parameter values |
| `args` | object | Additional method arguments |
| `events` | array | Event configurations |

#### Noise Definition

| Field | Type | Description |
|-------|------|-------------|
| `type` | string | Noise type: "Normal", "Uniform", "Poisson" |
| `method` | string | Application method: "jitter", "scaling" |
| `param1` | number | First parameter (mean for normal) |
| `param2` | number | Second parameter (stddev for normal) |
| `seed` | number | Random seed |

## Using the GUI

### Tree Selection Mode

When an experiment config is loaded, the GUI shows a hierarchical tree for selection:

1. **Series Tree**: Select series and parameter values
2. **Accelerations Tree**: Select methods with n and m values
3. **Noise Tree**: Select noise configurations
4. **Precision Tree**: Select precision types

Each tree uses tri-state checkboxes:
- ☑ All children selected
- ☐ Nothing selected
- ▣ Some children selected

Click on a parent to toggle all children. The selected count shows the cross-product of selections.

### Legacy Mode

Without an experiment config, the GUI shows:
1. **Select a Series**: Click on a series name
2. **Set Parameters**: Adjust X value and precision
3. **Select Algorithms**: Check algorithms to apply
4. **Compute**: Click the "Compute" button

### Plot Controls

- **View Menu**: Toggle partial sums, accelerated values, symlog scale
- **File Menu**: Clear cache, quit
- **Legend**: Shows color coding for each line

### Available Precisions

- `F32` - Single precision
- `F64` - Double precision
- `FLong` - Long double (if supported)
- `Arb` - Arbitrary precision
- `CF32`, `CF64`, `CFLong`, `CArb` - Complex variants

## Development

### Running Tests

```bash
cd shanks-unified
LD_LIBRARY_PATH=../backend/ffi/build/lib:$LD_LIBRARY_PATH cargo test
```

### Project Structure

```
shanks-unified/
├── src/
│   ├── lib.rs              # Library exports
│   ├── main.rs             # Entry point with CLI
│   ├── headless.rs         # Headless batch runner
│   ├── ffi/
│   │   ├── mod.rs          # FFI bindings (libloading)
│   │   └── types.rs        # Data types
│   ├── compute/
│   │   ├── mod.rs          # Module exports
│   │   ├── engine.rs       # Async compute engine
│   │   └── task.rs         # Task definitions
│   ├── cache/
│   │   └── mod.rs          # SQLite caching
│   ├── config/
│   │   ├── mod.rs          # App configuration
│   │   └── experiment.rs   # Experiment config parsing
│   ├── plot/
│   │   ├── mod.rs          # Plot utilities
│   │   └── symlog.rs       # Symlog scale
│   └── app/
│       ├── mod.rs          # egui application
│       ├── selection.rs    # Tree selection state
│       └── ui.rs           # UI components
└── tests/
    └── ffi_integration.rs  # Integration tests
```

## Troubleshooting

### "Library not found" error

Make sure `LD_LIBRARY_PATH` includes the path to `libshanks_ffi.so`:

```bash
export LD_LIBRARY_PATH=/path/to/backend/ffi/build/lib:$LD_LIBRARY_PATH
```

Or use `--lib-path` with an absolute path.

### "Symbol not found" error

The C++ library may need to be rebuilt. Ensure you're using the same compiler for both the library and the Rust application.

### GUI doesn't start

Ensure you have a display server running (X11 or Wayland). For headless systems, use the `headless` subcommand.

## License

Same as the parent project.
