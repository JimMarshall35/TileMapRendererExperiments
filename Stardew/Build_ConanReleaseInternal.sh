if [ ! -d "build" ]; then
  mkdir build
fi

echo $1

cd ./build
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake  -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Debug
cd ./Debug/game

if [ ! -d "Assets" ]; then
  mkdir Assets
fi

cd ..
cd ..
cd ..

echo "Copying assets folder..."
cp -a Assets build/Debug/game/
cd build/Debug
cmake --build . --config Debug
