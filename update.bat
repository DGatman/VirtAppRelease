@echo off
chcp 65001 >nul
title VirtApp Updater v2.3

echo ===================================================
echo 🚀 VirtApp Auto-Updater (Git Version)
echo ===================================================

:: 1. Закрываем бота, если он запущен
echo [1/2] 🛑 Закрытие VirtApp...
taskkill /F /IM VirtApp.exe >nul 2>&1
timeout /t 2 >nul

:: 2. Скачиваем файлы (включая новый VirtApp.exe)
echo [2/2] 📥 Скачивание обновления с GitHub...
git pull origin main

if %errorlevel% neq 0 (
    echo [ERROR] Ошибка обновления! Проверьте интернет.
    pause
    exit /b
)

echo.
echo ✅ Обновление завершено!
echo Сейчас запустится новая версия...
timeout /t 3
start VirtApp.exe
exit
