@echo off

for /f "tokens=*" %%i in (..\version.conf) do set VERSION=%%i

mkdir build_spdlog
mkdir build
rmdir /S /Q dist
rmdir /S /Q ..\lib\installer\build\build\bin
rmdir /S /Q ..\lib\installer\ui\installer\assets\builtin
mkdir dist

cd build_spdlog
cmake ..\..\lib\spdlog -G "MinGW Makefiles"
mingw32-make.exe

cd ..\build
cmake ..\.. -G "MinGW Makefiles"
mingw32-make.exe install

cd ..

copy ..\manifest.json dist\
copy ..\LICENSE dist\

cd ..\lib\installer\build
call build.bat
cd ..\..\..\build

robocopy ..\lib\installer\build\build\bin dist\bin /E /XD ressources
xcopy /E /Y .\build\bin\ dist\bin\

rmdir /S /Q shipping
mkdir shipping\launcher\windows

tar -cvzf shipping\launcher\windows\vortex_launcher_%VERSION%.tar.gz dist

certutil -hashfile shipping\launcher\windows\vortex_launcher_%VERSION%.tar.gz SHA256 > shipping\launcher\windows\vortex_launcher_%VERSION%.tar.gz.sha256

rmdir /S /Q ..\lib\installer\ui\installer\assets\builtin
mkdir ..\lib\installer\ui\installer\assets\builtin

copy shipping\launcher\windows\vortex_launcher_%VERSION%.tar.gz ..\lib\installer\ui\installer\assets\builtin\ 
copy shipping\launcher\windows\vortex_launcher_%VERSION%.tar.gz.sha256 ..\lib\installer\ui\installer\assets\builtin\

set MANIFEST_PATH=..\lib\installer\ui\installer\assets\builtin\manifest.json
set VERSION_FILE=..\version.conf
set ARCHITECTURE=x86_64
set PLATFORM=windows

for /f "tokens=*" %%V in (%VERSION_FILE%) do set VERSION=%%V

if exist %MANIFEST_PATH% copy %MANIFEST_PATH% %MANIFEST_PATH%.bak

(
    echo {
    echo    "version": "%VERSION%",
    echo    "arch": "%ARCHITECTURE%",
    echo    "platform": "%PLATFORM%",
    echo    "tarball": "vortex_launcher_%VERSION%.tar.gz",
    echo    "sum": "vortex_launcher_%VERSION%.tar.gz.sha256"
    echo }
) > %MANIFEST_PATH%


cd ..\lib\installer\build
call build.bat
cd ..\..\..\build

copy ..\lib\installer\build\build\bin\VortexInstaller.exe shipping\launcher\windows\
