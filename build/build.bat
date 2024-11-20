@echo off
REM Lire la version depuis version.conf
for /f "tokens=*" %%i in (..\version.conf) do set VERSION=%%i

REM Créer les dossiers de build
mkdir build_spdlog
mkdir build
mkdir dist

REM Compiler spdlog
cd build_spdlog
cmake ..\..\lib\spdlog -G "MinGW Makefiles"
mingw32-make.exe

cd ..\build
cmake ..\.. -G "MinGW Makefiles"
mingw32-make.exe install

cd ..

REM Copier les fichiers nécessaires dans dist/
copy ..\manifest.json dist\
copy ..\LICENSE dist\

REM Construire l'installateur/mise à jour/désinstallateur
cd ..\lib\installer\build
call build.bat
cd ..\..\..\build

REM Intégrer l'installateur et les outils dans Launcher
xcopy /E /Y ..\lib\installer\build\build\bin\ dist\bin\

REM Préparer les dossiers d'expédition
rmdir /S /Q shipping
mkdir shipping\installer\windows
mkdir shipping\launcher\windows

REM Archiver les fichiers en utilisant la version de version.conf
cd dist
tar -cvzf ..\..\shipping\launcher\windows\vortex_launcher_%VERSION%.tar.gz *
cd ..

REM Générer la somme de contrôle
certutil -hashfile shipping\launcher\windows\vortex_launcher_%VERSION%.tar.gz SHA256 > shipping\launcher\windows\vortex_launcher_%VERSION%.tar.gz.sha256
