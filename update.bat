@echo off
chcp 65001 >nul
title VirtApp Updater v2.4

echo ===================================================
echo 🚀 VirtApp Auto-Updater (Git Version)
echo ===================================================

:: 1. Закрываем бота
echo [1/3] 🛑 Закрытие процессов...
taskkill /F /IM VirtApp.exe >nul 2>&1
timeout /t 1 >nul

:: 2. Настройка Git и получение данных
echo [2/3] 🔄 Синхронизация с GitHub...
:: На всякий случай обновляем URL репозитория
git remote set-url origin https://github.com/RootOne1337/GTA5rpVirt.git
git fetch origin

:: Проверяем, какая ветка на сервере (main или master)
git show-ref --verify --quiet refs/remotes/origin/main
if %errorlevel% equ 0 (
    set BRANCH=main
) else (
    set BRANCH=master
)

echo Обнаружена ветка: %BRANCH%

:: 3. Жесткое обновление (Force Update)
echo [3/3] 📥 Применение обновлений...
git reset --hard origin/%BRANCH%

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Ошибка обновления!
    echo Попробуйте удалить папку и скачать заново.
    pause
    exit /b
)

:: Переключаемся на нужную ветку, если мы не на ней
git checkout %BRANCH% >nul 2>&1

echo.
echo ===================================================
echo ✅ Обновление успешно завершено!
echo ===================================================
echo Теперь можно запускать VirtApp.exe вручную.
pause
