if not exist build mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE -DBUILD_SHARED_LIBS=TRUE
cmake --build . --config Release
