if [ ! -d "build" ]; then
  mkdir build
fi

echo $1

cd build
cmake .. -DCMAKE_BUILD_TYPE=$1
make
cd src

if [ ! -d "Assets" ]; then
  mkdir Assets
fi

cd ..
cd ..

echo "Copying assets folder..."
cp -a Assets build/src
