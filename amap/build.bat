:: if use visual studio 2019, change cmake command to:
:: cmake -G "Visual Studio 16 2019" -A Win32 ..
:: need to use cmake 3.14+ version

cd build/
cmake -A Win32 ..