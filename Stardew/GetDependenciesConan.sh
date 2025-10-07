if [ ! -d "build" ]; then
  mkdir build
fi
cd build
sudo pipx run conan profile detect --force
sudo pipx run conan install -c tools.system.package_manager:mode=install --build=missing ..
