mkdir -p build/

cd build/
cmake ../../shanks_transformation -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=../../shanks_consumer/pyshanks
cmake --build . --target pyshanks

cd ..
rm -rf build/