cmake -S ../lib -B build
make -C build pyshanks
cp build/*.so pyshanks/
rm -rf build
