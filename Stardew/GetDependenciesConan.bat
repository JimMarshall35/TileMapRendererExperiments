if not exist build mkdir build
conan profile detect --force
cd build
conan install ..
