@echo off
mkdir build 2>nul
cd build
cmake ../pyshanks
cmake --build . --target pyshanks --config Release
cd Release
copy "pyshanks.cp*.pyd" "../../../shanks_consumer/pyshanks/" /Y
cd ..
cd ..
rmdir /s /q build
pause
