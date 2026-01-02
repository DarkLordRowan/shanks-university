mkdir -p build/                                                                           
cd build/                                                                                  
cmake ../pyshanks                                                         
cmake --build . --target pyshanks --config Release                                         
Copy-Item "Release/pyshanks.cp313-win_amd64.pyd" "../../shanks_consumer/pyshanks/" -Force   
cd ..                                                                                       
rm build/ -r -fo
