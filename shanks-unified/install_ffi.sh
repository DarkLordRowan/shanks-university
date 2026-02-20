#!/usr/bin/env bash
set -e

# Shanks FFI Installation Script for shanks-unified
# This script builds the C++ FFI library and places it in the shanks-unified directory.

echo "BUILDING SHANKS FFI..."

# Paths
BACKEND_FFI_DIR="../backend/ffi"
BUILD_DIR="$BACKEND_FFI_DIR/build"
OUTPUT_LIB="libshanks_ffi.so"

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Configure with CMake
CMAKE_ARGS="-S $BACKEND_FFI_DIR -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Release"

# Detect if we should use Ninja
if command -v ninja >/dev/null 2>&1; then
    WANT_NINJA=true
else
    WANT_NINJA=false
fi

# Check for generator mismatch
if [ -f "$BUILD_DIR/CMakeCache.txt" ]; then
    HAS_NINJA=$(grep "CMAKE_GENERATOR:INTERNAL=Ninja" "$BUILD_DIR/CMakeCache.txt" || true)
    if [ "$WANT_NINJA" = true ] && [ -z "$HAS_NINJA" ]; then
        echo "Generator mismatch (wanted Ninja, found Make). Cleaning build directory..."
        rm -rf "$BUILD_DIR"/*
    elif [ "$WANT_NINJA" = false ] && [ -n "$HAS_NINJA" ]; then
        echo "Generator mismatch (wanted Make, found Ninja). Cleaning build directory..."
        rm -rf "$BUILD_DIR"/*
    fi
fi

if [ "$WANT_NINJA" = true ]; then
    echo "Using Ninja generator..."
    cmake $CMAKE_ARGS -GNinja
else
    echo "Using default generator..."
    cmake $CMAKE_ARGS
fi

# Build
echo "Compiling..."
cmake --build "$BUILD_DIR" --target shanks_ffi -j$(nproc)

# Copy to current directory
echo "Installing library to shanks-unified..."
cp "$BUILD_DIR/lib/$OUTPUT_LIB" .

echo "DONE! libshanks_ffi.so is now in the current directory."
