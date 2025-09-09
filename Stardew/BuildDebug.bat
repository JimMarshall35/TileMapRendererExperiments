if not exist build mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=generators\conan_toolchain.cmake  -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE
cmake --build . --config Debug
rem For debugging with visual studio they need to go here
robocopy "..\Assets" "game\Assets" /E /XO
robocopy "engine\src\Debug" "game\Debug" "StardewEngine.dll" /E /XO
robocopy "engine\src\Debug" "enginetest\Debug" "StardewEngine.dll" /E /XO
robocopy "..\enginetest\data" "enginetest\Debug\data" /E /XO
cd enginetest\Debug
StardewEngineTest.exe

pause