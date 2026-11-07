@echo off
setlocal
cd /d "%~dp0"

set "QT_ROOT=%QT_ROOT%"
if not defined QT_ROOT set "QT_ROOT=C:\Qt"

if not defined QT_PREFIX_PATH if defined CMAKE_PREFIX_PATH set "QT_PREFIX_PATH=%CMAKE_PREFIX_PATH%"
if not defined QT_PREFIX_PATH if defined Qt6_DIR (
    for %%I in ("%Qt6_DIR%\..\..\..") do set "QT_PREFIX_PATH=%%~fI"
)
if not defined QT_PREFIX_PATH (
    for /d %%V in ("%QT_ROOT%\6*") do (
        if exist "%%~fV\mingw_64\lib\cmake\Qt6\Qt6Config.cmake" set "QT_PREFIX_PATH=%%~fV\mingw_64"
    )
)
if not defined QT_PREFIX_PATH (
    echo Qt 6 was not found. Install a Qt 6 MinGW 64-bit kit, or set QT_PREFIX_PATH to its mingw_64 folder.
    pause
    exit /b 1
)

if not defined CXX_COMPILER if defined CMAKE_CXX_COMPILER set "CXX_COMPILER=%CMAKE_CXX_COMPILER%"
if not defined CXX_COMPILER if defined MINGW_ROOT if exist "%MINGW_ROOT%\bin\g++.exe" set "CXX_COMPILER=%MINGW_ROOT%\bin\g++.exe"
if not defined CXX_COMPILER if exist "%QT_ROOT%\Tools\mingw1310_64\bin\g++.exe" set "CXX_COMPILER=%QT_ROOT%\Tools\mingw1310_64\bin\g++.exe"
if not defined CXX_COMPILER (
    for /d %%M in ("%QT_ROOT%\Tools\mingw*_64") do (
        if not defined CXX_COMPILER if exist "%%~fM\bin\g++.exe" set "CXX_COMPILER=%%~fM\bin\g++.exe"
    )
)
if not defined CXX_COMPILER (
    for /f "delims=" %%G in ('where g++ 2^>nul') do if not defined CXX_COMPILER set "CXX_COMPILER=%%G"
)
if not defined CXX_COMPILER (
    echo MinGW g++ was not found. Install the compiler matching your Qt kit, or set MINGW_ROOT.
    pause
    exit /b 1
)

for %%I in ("%CXX_COMPILER%") do set "MINGW_BIN=%%~dpI"
set "PATH=%QT_PREFIX_PATH%\bin;%MINGW_BIN%;%PATH%"

set "CMAKE_EXE="
if exist "%QT_ROOT%\Tools\CMake_64\bin\cmake.exe" set "CMAKE_EXE=%QT_ROOT%\Tools\CMake_64\bin\cmake.exe"
if not defined CMAKE_EXE (
    for /f "delims=" %%C in ('where cmake 2^>nul') do if not defined CMAKE_EXE set "CMAKE_EXE=%%C"
)
if not defined CMAKE_EXE (
    echo CMake was not found. Install CMake and add it to PATH.
    pause
    exit /b 1
)

set "NINJA_EXE="
if exist "%QT_ROOT%\Tools\Ninja\ninja.exe" set "NINJA_EXE=%QT_ROOT%\Tools\Ninja\ninja.exe"
if not defined NINJA_EXE (
    for /f "delims=" %%N in ('where ninja 2^>nul') do if not defined NINJA_EXE set "NINJA_EXE=%%N"
)
if not defined NINJA_EXE (
    echo Ninja was not found. Install Ninja and add it to PATH.
    pause
    exit /b 1
)

echo Configuring BarCodeX...
"%CMAKE_EXE%" -S "%~dp0." -B "%~dp0build\BarCodeX" -G Ninja -DCMAKE_MAKE_PROGRAM="%NINJA_EXE%" -DCMAKE_CXX_COMPILER="%CXX_COMPILER%" -DCMAKE_PREFIX_PATH="%QT_PREFIX_PATH%"
if errorlevel 1 (
    echo CMake configure failed. Check that the MinGW compiler matches the installed Qt kit.
    pause
    exit /b 1
)

echo Building BarCodeX...
"%CMAKE_EXE%" --build "%~dp0build\BarCodeX" --target BarCodeX
if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)

set "APP_EXE=%~dp0build\BarCodeX\BarCodeX.exe"
if not exist "%APP_EXE%" (
    echo Application executable was not created.
    pause
    exit /b 1
)

if exist "%QT_PREFIX_PATH%\bin\windeployqt.exe" (
    "%QT_PREFIX_PATH%\bin\windeployqt.exe" --no-translations --compiler-runtime "%APP_EXE%"
    if errorlevel 1 (
        echo Qt runtime deployment failed.
        pause
        exit /b 1
    )
) else (
    set "QT_PLUGIN_PATH=%QT_PREFIX_PATH%\plugins"
    set "QT_QPA_PLATFORM_PLUGIN_PATH=%QT_PREFIX_PATH%\plugins\platforms"
)

"%APP_EXE%"
if errorlevel 1 (
    echo The application exited with an error.
    pause
    exit /b 1
)