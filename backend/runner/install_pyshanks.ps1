mkdir -p build/
cd build/
cmake ../../bindings
cmake --build . --target pyshanks --config Release
Copy-Item "Release/pyshanks.cp313-win_amd64.pyd" "../pyshanks/" -Force
cd ..
rm build/ -r -fo
