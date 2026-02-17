#!/bin/sh

NO_INSTALLER=false
while [[ "$#" -gt 0 ]]; do
  case $1 in
    -ni|--no_installer)
      NO_INSTALLER=true
      shift
      ;;
    *)
      echo "Unknown option : $1"
      exit 1
      ;;
  esac
done

VERSION=$(cat ../version.conf)

rm -rf ./dist
rm -rf ./build/dist
rm -rf ./build/bin
rm -rf ./shipping
rm -rf ../lib/installer/build/build/bin
rm -rf ../lib/installer/build/build/dist
rm -rf ../lib/installer/src/ui/assets/builtin
mkdir -p dist
mkdir -p build

# -----------------------------
# Build
# -----------------------------
cd ./build
cmake -DCMAKE_BUILD_TYPE=Release ../..
make -j$(nproc) install

cd ..

cp ../manifest.json dist/
cp ../LICENSE dist/

# -----------------------------
# Installer
# -----------------------------
if [ "$NO_INSTALLER" = false ]; then
  rm -rf ../lib/installer/src/ui/assets/builtin
  cd ../lib/installer/build
  sudo bash build.sh
  cd ../../../build

  cp -r ../lib/installer/build/build/bin/* dist/bin/
  sudo rm -rf dist/bin/builtin
  sudo rm -rf dist/bin/resources

  cp -rn ./build/bin/* dist/bin/
fi

# -----------------------------
# Manifest
# -----------------------------
rm -rf shipping
mkdir -p shipping/launcher/linux

sudo rm dist/bin/vxuninstall
sudo rm dist/bin/vxinstaller
sudo rm dist/bin/vortex_installer
sudo rm dist/bin/vortex_uninstall
sudo rm dist/bin/vortex_update

if [ "$NO_INSTALLER" = false ]; then
  TAR_FILE=./shipping/launcher/linux/vortex_launcher_${VERSION}.tar.gz
  tar -cvzf "$TAR_FILE" dist

  cd shipping/launcher/linux
  sha256sum "vortex_launcher_${VERSION}.tar.gz" > "vortex_launcher_${VERSION}.tar.gz.sha256"
  cd ../../../

  rm -rf  ../lib/installer/src/ui/assets/builtin
  mkdir -p  ../lib/installer/src/ui/assets/builtin

  cp shipping/launcher/linux/vortex_launcher_${VERSION}.tar.gz ../lib/installer/src/ui/assets/builtin/
  cp shipping/launcher/linux/vortex_launcher_${VERSION}.tar.gz.sha256 ../lib/installer/src/ui/assets/builtin/
  cp -r ../lib/blank_project ../lib/installer/src/ui/assets/builtin/

  MANIFEST_PATH="../lib/installer/src/ui/assets/builtin/manifest.json"
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
fi
