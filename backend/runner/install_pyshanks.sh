echo "BUILDING PYSHANKS..."
cmake -S ../bindings -B build
make -C build pyshanks

echo "PREPARING FOLDER FOR PYTHON..."
mkdir -p pyshanks
touch pyshanks/__init__.py && echo 'from .pyshanks import *' > pyshanks/__init__.py
cp build/*.so pyshanks/

echo "CLEANING..."
rm -rf build

