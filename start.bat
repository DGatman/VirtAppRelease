@echo off
:loop

tasklist /fi "ImageName eq VirtApp.exe" /fo csv 2>NUL | find /I "VirtApp.exe">NUL
if "%ERRORLEVEL%"=="0" (
    echo VirtApp is running...
) else (
    echo VirtApp not running, updating and starting...
    cd /d C:\Users\%USERNAME%\VirtAppRelease\
    git reset --hard HEAD
    git pull origin master --force
    start VirtApp.exe
)

timeout /t 30 /nobreak >nul
goto loop
