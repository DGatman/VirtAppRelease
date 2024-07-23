@echo off
:loop

tasklist /fi “ImageName eq VirtApp.exe” /fo csv 2>NUL | find /I “VLC.exe”>NUL

if “%ERRORLEVEL%”==”0”  (echo Process / Application is running) else (cd C:\Users\%USERNAME%\VirtAppRelease\ && git pull && VirtApp.exe)
goto loop