echo "BUILDING PYSHANKS..."
CMAKE_ARGS="-S ../bindings -B build -DSHANKS_ENABLE_PROFILING=ON"

# Use Ninja if available, fall back to standard generator otherwise
if command -v ninja >/dev/null 2>&1; then
    cmake $CMAKE_ARGS -GNinja
else
    cmake $CMAKE_ARGS
fi
cmake --build build --target pyshanks -j$(nproc)

echo "PREPARING FOLDER FOR PYTHON..."
mkdir -p pyshanks
touch pyshanks/__init__.py && echo 'from .pyshanks import *' > pyshanks/__init__.py
cp build/*.so pyshanks/
