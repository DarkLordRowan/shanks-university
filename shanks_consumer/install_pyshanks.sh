cmake -S ../pyshanks -B build
make -C build pyshanks
cp build/*.so pyshanks/
rm -rf build