echo "BUILDING PYSHANKS..."
# Use Ninja if available, fall back to standard generator otherwise
if command -v ninja >/dev/null 2>&1; then
    cmake -S ../bindings -B build -GNinja
else
    cmake -S ../bindings -B build
fi
cmake --build build --target pyshanks -j$(nproc)

echo "PREPARING FOLDER FOR PYTHON..."
mkdir -p pyshanks
touch pyshanks/__init__.py && echo 'from .pyshanks import *' > pyshanks/__init__.py
cp build/*.so pyshanks/
