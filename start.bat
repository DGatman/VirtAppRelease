@echo off
title VirtApp Watchdog
chcp 65001 >nul
cd /d "%~dp0"

:: ==============================================================
:: VirtApp Watchdog & Auto-Updater
:: Checks if VirtApp.exe is running. If not, updates and restarts.
:: ==============================================================

:loop
:: Check if process is running
tasklist /fi "ImageName eq VirtApp.exe" /fo csv 2>NUL | find /I "VirtApp.exe" >NUL

if "%ERRORLEVEL%"=="0" (
    :: App is running. Wait 60 seconds and check again.
    echo [Watchdog] VirtApp is running...
    timeout /t 60 >nul
    goto loop
) else (
    echo.
    echo [Watchdog] ⚠️ VirtApp is NOT running! Initiating restart sequence...
    
    :: 2. Update via Git
    echo [Update] ⬇️ Pulling updates from GitHub...
    git remote set-url origin https://github.com/DGatman/VirtAppRelease.git
    git pull
    
    :: 4. Start VirtApp
    echo [Start] 🚀 Launching VirtApp...
    start "" VirtApp.exe
    
    :: Wait for initialization
    timeout /t 10 >nul
    goto loop
)