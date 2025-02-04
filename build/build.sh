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

mkdir -p build_spdlog
rm -rf ./dist
rm -rf ./build/dist
rm -rf ./build/bin
rm -rf ./shipping
rm -rf ../lib/installer/build/build/bin
rm -rf ../lib/installer/build/build/dist
rm -rf ../lib/installer/ui/installer/assets/builtin
mkdir -p dist
mkdir -p build

cd build_spdlog
cmake ../../lib/spdlog
make -j$(nproc)

cd ../build 
cmake ../.. 
make -j$(nproc) install

cd ..

cp ../manifest.json dist/
cp ../LICENSE dist/

if [ "$NO_INSTALLER" = false ]; then
  rm -rf ../lib/installer/ui/installer/assets/builtin
  cd ../lib/installer/build
  sudo bash build.sh
  cd ../../../build

  cp -r ../lib/installer/build/build/bin/* dist/bin/
  sudo rm -rf dist/bin/builtin
  sudo rm -rf dist/bin/resources

  cp -rn ./build/bin/* dist/bin/
fi

rm -rf shipping
mkdir -p shipping/launcher/linux

if [ "$NO_INSTALLER" = false ]; then
  TAR_FILE=./shipping/launcher/linux/vortex_launcher_${VERSION}.tar.gz
  tar -cvzf "$TAR_FILE" dist

  cd shipping/launcher/linux
  sha256sum "vortex_launcher_${VERSION}.tar.gz" > "vortex_launcher_${VERSION}.tar.gz.sha256"
  cd ../../../

  rm -rf  ../lib/installer/ui/installer/assets/builtin
  mkdir -p  ../lib/installer/ui/installer/assets/builtin

  cp shipping/launcher/linux/vortex_launcher_${VERSION}.tar.gz ../lib/installer/ui/installer/assets/builtin/
  cp shipping/launcher/linux/vortex_launcher_${VERSION}.tar.gz.sha256 ../lib/installer/ui/installer/assets/builtin/

  cp -r ../lib/blank_project ../lib/installer/ui/installer/assets/builtin/

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
fi
