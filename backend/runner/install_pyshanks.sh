echo "BUILDING PYSHANKS..."
CMAKE_ARGS="-S ../bindings -B build -DSHANKS_ENABLE_PROFILING=OFF"

echo "PREPARING FOLDER FOR PYTHON..."
mkdir -p src/pyshanks
touch src/pyshanks/__init__.py && echo 'from .pyshanks import *' > src/pyshanks/__init__.py

# Use Ninja if available, fall back to standard generator otherwise
if command -v ninja >/dev/null 2>&1; then
    cmake $CMAKE_ARGS -GNinja
else
    cmake $CMAKE_ARGS
fi
cmake --build build --target pyshanks -j$(nproc)

cp build/*.so src/pyshanks/
