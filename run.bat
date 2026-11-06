@echo off
setlocal
cd /d "%~dp0"

cmake --preset qt-mingw
if errorlevel 1 (
    echo CMake configure failed.
    pause
    exit /b 1
)

cmake --build --preset qt-mingw-build --target HelloWorld
if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)

set "PATH=C:\Qt\6.12.0\mingw_64\bin;C:\Qt\Tools\mingw1310_64\bin;%PATH%"
set "QT_PLUGIN_PATH=C:\Qt\6.12.0\mingw_64\plugins"
set "QT_QPA_PLATFORM_PLUGIN_PATH=C:\Qt\6.12.0\mingw_64\plugins\platforms"

if not exist "%~dp0build\BarCodeX\HelloWorld.exe" (
    echo Application executable was not created.
    pause
    exit /b 1
)

"%~dp0build\BarCodeX\HelloWorld.exe"
