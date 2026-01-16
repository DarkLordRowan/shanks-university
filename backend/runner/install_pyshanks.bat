@echo off
mkdir build 2>nul
cd build
cmake ../../bindings
cmake --build . --target pyshanks --config Release
cd Release
mkdir "../../../pyshanks/"
echo "from .pyshanks import *" >> "../../../pyshanks/__init__.py"
copy "pyshanks.cp*.pyd" "../../../pyshanks/" /Y
cd ..
cd ..
rmdir /s /q build
pause
