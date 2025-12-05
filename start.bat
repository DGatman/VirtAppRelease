@echo off
:loop
tasklist /fi "ImageName eq VirtApp.exe" /fo csv 2>NUL | find /I "VirtApp.exe">NUL
if "%ERRORLEVEL%"=="0" (
    REM VirtApp is running
) else (
    cd /d %~dp0
    call update.bat
    start VirtApp.exe
)
timeout /t 30 /nobreak >nul
goto loop
