@echo off
REM === VirtApp Updater (Silent) ===
if exist config.txt copy config.txt config.backup.txt >nul 2>&1
git pull origin master >nul 2>&1
if exist config.backup.txt (copy config.backup.txt config.txt >nul 2>&1 & del config.backup.txt >nul 2>&1)
