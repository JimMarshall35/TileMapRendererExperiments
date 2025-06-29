if not exist build mkdir build
cd build
cmake .. -G "Visual Studio 14 Win64"
cmake --build . --config Release
