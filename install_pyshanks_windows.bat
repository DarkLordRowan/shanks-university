cmake -S shanks_transformation -B build
cmake --build build

move /Y build\Debug\pyshanks.c* shanks_consumer\pyshanks

