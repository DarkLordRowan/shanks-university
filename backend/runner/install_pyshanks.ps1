mkdir -p build/
cd build/
cmake ../../bindings
cmake --build . --target pyshanks --config Release
New-Item -ItemType Directory -Force "../src/pyshanks" | Out-Null
Copy-Item "Release/pyshanks.cp313-win_amd64.pyd" "../src/pyshanks/" -Force
cd ..
rm build/ -r -fo
