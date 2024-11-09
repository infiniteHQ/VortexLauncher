#!/bin/sh

# Read the version from version.conf
VERSION=$(cat ../version.conf)

# Create build folders
mkdir -p build_spdlog
mkdir -p build
mkdir -p dist


# Compile stack
cd build_spdlog
cmake ../../lib/spdlog
make -j$(nproc)

cd ../build 
cmake ../.. 
make -j$(nproc) install

cd ..

# Copy necessary files to dist/
cp ../manifest.json dist/
cp ../LICENSE dist/

# Build embedded installer/updater/uninstaller
cd ../lib/installer/build
sudo bash build.sh
cd ../../../build

# Embed the installer and tools into Launcher
cp -rn ../lib/installer/build/build/bin/* dist/bin/

sudo chown root:root ./dist/bin/vortex_uninstall
sudo chown root:root ./dist/bin/vortex_update

sudo chmod u+s ./dist/bin/vortex_uninstall
sudo chmod u+s ./dist/bin/vortex_update

# Prepare shipping folders
rm -rf shipping/*
mkdir -p shipping/installer/linux
mkdir -p shipping/launcher/linux

# Archive the files using the version from version.conf
#TAR_FILE=./shipping/launcher/linux/vortex_launcher_${VERSION}.tar.gz
#tar -cvzf "$TAR_FILE" dist/

# Generate checksum
cd shipping/launcher/linux
sha256sum "vortex_launcher_${VERSION}.tar.gz" > "vortex_launcher_${VERSION}.tar.gz.sha256"
cd ../../../