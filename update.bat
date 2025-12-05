@echo off
chcp 65001 >nul
echo ══════════════════════════════════════════════════════════
echo              VirtApp Updater
echo ══════════════════════════════════════════════════════════
echo.

REM Сохраняем конфиг перед обновлением
if exist config.txt (
    echo [*] Сохраняю config.txt...
    copy config.txt config.backup.txt >nul
)

echo [*] Обновляю из GitHub...
git pull origin main

REM Восстанавливаем конфиг после обновления  
if exist config.backup.txt (
    echo [*] Восстанавливаю config.txt...
    copy config.backup.txt config.txt >nul
    del config.backup.txt
)

echo.
echo ══════════════════════════════════════════════════════════
echo              Обновление завершено!
echo ══════════════════════════════════════════════════════════
echo.
pause
