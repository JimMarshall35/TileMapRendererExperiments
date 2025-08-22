if not exist build mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=generators\conan_toolchain.cmake  -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE
cmake --build . --config Release
robocopy "..\Assets" "game\Release\Assets" /E /XO
robocopy "engine\src\Release" "game\Release" "StardewEngine.dll" /E /XO
pause