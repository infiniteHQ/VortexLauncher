@echo off

for /f "tokens=*" %%i in (..\version.conf) do set VERSION=%%i

mkdir build_spdlog
mkdir build
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

xcopy /E /Y ..\lib\installer\build\build\bin\ dist\bin\

rmdir /S /Q shipping
mkdir shipping\installer\windows
mkdir shipping\launcher\windows

cd dist
tar -cvzf ..\..\shipping\launcher\windows\vortex_launcher_%VERSION%.tar.gz *
cd ..

certutil -hashfile shipping\launcher\windows\vortex_launcher_%VERSION%.tar.gz SHA256 > shipping\launcher\windows\vortex_launcher_%VERSION%.tar.gz.sha256
