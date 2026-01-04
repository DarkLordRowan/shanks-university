@echo off
mkdir build 2>nul
cd build
cmake ../lib
cmake --build . --target pyshanks --config Release
cd Release
copy "pyshanks.cp*.pyd" "../../../consumer/pyshanks/" /Y
cd ..
cd ..
rmdir /s /q build
pause
