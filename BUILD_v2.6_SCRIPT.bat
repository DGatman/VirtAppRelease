@echo off
REM ====================================================================
REM Script for compiling VirtApp v2.6 with all dependencies
REM Required: Visual Studio 2022 + CMake + vcpkg with all dependencies
REM ====================================================================

setlocal enabledelayedexpansion

set PROJECT_DIR=%~dp0
set PROJECT_DIR=%PROJECT_DIR:~0,-1%
set BUILD_DIR=%PROJECT_DIR%\Build\build_v2.6
set INSTALL_DIR=%PROJECT_DIR%\Release

echo.
echo ========================================
echo    VirtApp v2.6 Build Script
echo ========================================
echo.
echo Project Dir: %PROJECT_DIR%
echo Build Dir: %BUILD_DIR%
echo.

if not exist "%INSTALL_DIR%" (
    mkdir "%INSTALL_DIR%"
)

REM === Step 1: Clean old build ===
echo [1/5] Cleaning old build...
if exist "%BUILD_DIR%" (
    echo Removing %BUILD_DIR%...
    rmdir /s /q "%BUILD_DIR%" 2>nul
    timeout /t 1 /nobreak >nul
)

REM === Step 2: Create build directory ===
echo [2/5] Creating build directory...
mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

REM === Step 3: Configure with CMake ===
set CMAKE_PATH=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe
echo [3/5] Configuring CMake for Release x64...
"%CMAKE_PATH%" -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
    -DVCPKG_TARGET_TRIPLET=x64-windows ^
    "%PROJECT_DIR%"

if errorlevel 1 (
    echo ERROR: CMake configuration failed!
    pause
    exit /b 1
)

REM === Step 4: Build with CMake ===
echo [4/5] Building Release x64 (this may take a few minutes)...
"%CMAKE_PATH%" --build . --config Release --parallel 4

if errorlevel 1 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

REM === Step 5: Copy result ===
echo [5/5] Copying executable...
if exist "%BUILD_DIR%\Release\VirtApp.exe" (
    copy "%BUILD_DIR%\Release\VirtApp.exe" "%INSTALL_DIR%\VirtApp_v2.6.exe"
    echo.
    echo ========================================
    echo    ✓ SUCCESS!
    echo ========================================
    echo.
    echo Executable: %INSTALL_DIR%\VirtApp_v2.6.exe
    echo.
    echo Build completed! Run: %INSTALL_DIR%\VirtApp_v2.6.exe
    echo.
) else (
    echo ERROR: VirtApp.exe not found in build output!
    echo Searching for exe...
    dir /s "%BUILD_DIR%\*.exe" 2>nul
    pause
    exit /b 1
)

pause
endlocal
