@echo off
chcp 65001 >nul
title VirtApp Updater

echo ===================================================
echo 🚀 VirtApp Auto-Updater (Direct Download)
echo ===================================================

:: 1. Закрываем бота
echo [1/3] 🛑 Закрытие VirtApp...
taskkill /F /IM VirtApp.exe >nul 2>&1
timeout /t 1 >nul

:: 2. Скачиваем файл напрямую (без Git)
echo [2/3] 📥 Скачивание новой версии...
powershell -Command "Invoke-WebRequest -Uri 'https://github.com/DGatman/VirtAppRelease/raw/master/VirtApp.exe' -OutFile 'VirtApp.exe'"

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Ошибка скачивания!
    echo Проверьте интернет или доступность GitHub.
    pause
    exit /b
)

:: 3. Запуск
echo [3/3] ✅ Успешно! Запуск...
start VirtApp.exe
exit
