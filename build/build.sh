#!/bin/sh

# Create build folders
mkdir -p build_spdlog
mkdir -p build

# Compile stack
cd build_spdlog && cmake ../../lib/spdlog && make -j$(nproc) install
cd ../build && cmake ../.. && make -j$(nproc) install

cd ..

# Cpy 
cp ../manifest.json dist/
cp ../LICENSE dist/

# Build embedded installer/updater/uninstaller
cd ../lib/installer/build
sudo bash build.sh
cd ../../../build
cp -rn ../lib/installer/build/build/bin/* dist/bin/

mkdir -p shipping/linux
mkdir -p shipping/windows
mkdir -p shipping/macos

tar -cvzf ./shipping/linux/vortex_launcher_1.2.tar.gz dist/
cd shipping/linux

sha256sum vortex_launcher_1.2.tar.gz > vortex_launcher_1.2.tar.gz.sha256

cd ../..
