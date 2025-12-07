@echo off
chcp 65001 >nul
title VirtApp Updater v2.2

echo ===================================================
echo 🚀 VirtApp Auto-Updater
echo ===================================================

:: 1. Проверка Git
git --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] Git не установлен! Установите Git.
    pause
    exit /b
)

:: 2. Обновление кода
echo [1/3] 📥 Получение обновлений из GitHub...
git pull origin main
if %errorlevel% neq 0 (
    echo [ERROR] Не удалось скачать обновления. Проверьте интернет или конфликты.
    pause
    exit /b
)

:: 3. Сборка (если есть CMake)
cmake --version >nul 2>&1
if %errorlevel% equ 0 (
    echo [2/3] 🛠️ Обнаружен CMake. Пересборка проекта...
    if not exist "build_new" mkdir build_new
    cmake --build build_new --config Release
    
    if exist "build_new\Release\VirtApp.exe" (
        echo [3/3] ✅ Сборка успешна! Копирование файла...
        copy /Y "build_new\Release\VirtApp.exe" "VirtApp.exe" >nul
    ) else (
        echo [WARN] Файл VirtApp.exe не найден после сборки.
    )
) else (
    echo [INFO] CMake не найден. Пропуск этапа сборки.
    echo Используется текущая версия VirtApp.exe.
)

echo.
echo ===================================================
echo ✅ Обновление завершено!
echo ===================================================
timeout /t 5
