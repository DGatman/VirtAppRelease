@echo off
chcp 65001 > nul
echo ========================================
echo    VirtApp Update Script v3.2
echo ========================================
echo.

echo [1/4] Stopping VirtApp...
taskkill /f /im VirtApp.exe 2>nul
ping 127.0.0.1 -n 3 > nul

echo [2/4] Resetting local changes...
git reset --hard HEAD

echo [3/4] Pulling latest version...
git pull origin master --force

echo [4/4] Done!
echo.
echo ========================================
echo    Update complete! Starting VirtApp...
echo ========================================
ping 127.0.0.1 -n 2 > nul
start VirtApp.exe
