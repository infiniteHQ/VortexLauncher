#!/bin/sh

VERSION=$(cat ../version.conf)

mkdir -p build_spdlog
mkdir -p build
mkdir -p dist

cd build_spdlog
cmake ../../lib/spdlog
make -j$(nproc)

cd ../build 
cmake ../.. 
make -j$(nproc) install

cd ..

cp ../manifest.json dist/
cp ../LICENSE dist/

cd ../lib/installer/build
sudo bash build.sh
cd ../../../build

cp -rn ../lib/installer/build/build/bin/* dist/bin/
cp -rn ./build/bin/* dist/bin/

rm -rf shipping
mkdir -p shipping/launcher/linux

TAR_FILE=./shipping/launcher/linux/vortex_launcher_${VERSION}.tar.gz
tar -cvzf "$TAR_FILE" dist/

cd shipping/launcher/linux
sha256sum "vortex_launcher_${VERSION}.tar.gz" > "vortex_launcher_${VERSION}.tar.gz.sha256"
cd ../../../

rm -rf  ../lib/installer/ui/installer/assets/builtin
mkdir -p  ../lib/installer/ui/installer/assets/builtin

cp "shipping/launcher/linux/vortex_launcher_${VERSION}.tar.gz" "../lib/installer/ui/installer/assets/builtin/"
cp "shipping/launcher/linux/vortex_launcher_${VERSION}.tar.gz.sha256" "../lib/installer/ui/installer/assets/builtin/"

cp -r ../lib/blank_project ../lib/installer/ui/installer/assets/builtin/

# Variables
MANIFEST_PATH="../lib/installer/ui/installer/assets/builtin/manifest.json"
VERSION_FILE="../version.conf"
ARCHITECTURE="x86_64"
PLATFORM="linux"

if [[ -f "$VERSION_FILE" ]]; then
  VERSION=$(<"$VERSION_FILE")
else
  echo "Erreur : Fichier version.conf introuvable."
  exit 1
fi

if [[ -f "$MANIFEST_PATH" ]]; then
  cp "$MANIFEST_PATH" "${MANIFEST_PATH}.bak"
fi

cat > "$MANIFEST_PATH" <<EOL
{
    "version": "${VERSION}",
    "arch": "${ARCHITECTURE}",
    "platform": "${PLATFORM}",
    "tarball": "vortex_launcher_${VERSION}.tar.gz",
    "sum": "vortex_launcher_${VERSION}.tar.gz.sha256"
}
EOL

cd ../lib/installer/build
sudo bash build.sh
cd ../../../build

cp ../lib/installer/build/build/bin/VortexInstaller shipping/launcher/linux/
