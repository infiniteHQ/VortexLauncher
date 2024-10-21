#!/bin/sh

mkdir -p build_spdlog
mkdir -p build

cd build_spdlog && cmake ../../lib/spdlog && make -j$(nproc) install

cd ../build && cmake ../.. && make -j$(nproc) install

cd ..

cp ./build/libvortex_launcher_shared.so ./build/bin/

mkdir -p shipping/linux
tar -cvzf ./shipping/linux/vortex_launcher_1.2.tar.gz dist/
cd shipping/linux

sha256sum vortex_launcher_1.2.tar.gz > vortex_launcher_1.2.tar.gz.sha256

cd ../..
