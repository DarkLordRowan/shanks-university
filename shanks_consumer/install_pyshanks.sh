cd ../shanks_transformation
cmake -B build
cd build && make pyshanks
cp pyshanks.cpython-313-x86_64-linux-gnu.so ../../shanks_consumer/pyshanks/