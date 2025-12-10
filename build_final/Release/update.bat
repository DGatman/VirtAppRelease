@echo off
chcp 65001 >nul
echo ==========================================================
echo              VirtApp Updater v2.2
echo ==========================================================
echo.

REM 1. Backup config and credentials
if exist config.txt (
    echo [*] Saving config.txt...
    copy /Y config.txt config.txt.bak >nul
)
if exist credentials.json (
    echo [*] Saving credentials.json...
    copy /Y credentials.json credentials.json.bak >nul
)

REM 2. Check/Repair Git
if not exist .git (
    echo [!] Git repository not found. Repairing...
    git init
    git remote add origin https://github.com/DGatman/VirtAppRelease
    echo [*] Fetching latest version...
    git fetch origin
    echo [*] Resetting to latest version...
    git reset --hard origin/master
) else (
    echo [*] Cleaning local changes...
    git reset --hard HEAD >nul
    echo [*] Downloading updates...
    git pull origin master
)

REM 3. Restore config and credentials
if exist config.txt.bak (
    echo [*] Restoring config.txt...
    copy /Y config.txt.bak config.txt >nul
    del config.txt.bak
)
if exist credentials.json.bak (
    echo [*] Restoring credentials.json...
    copy /Y credentials.json.bak credentials.json >nul
    del credentials.json.bak
)

echo.
echo ==========================================================
echo              Update Completed Successfully!
echo ==========================================================
echo.
pause