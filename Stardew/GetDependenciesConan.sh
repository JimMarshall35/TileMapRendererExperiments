if [ ! -d "build" ]; then
  mkdir build
fi
cd build

sudo pipx run conan install ..
