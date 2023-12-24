@echo off
C:\Windows\System32\taskkill /f /im VirtApp.exe
C:\Windows\System32\ping 127.0.0.1 -n 5 > nul
start.bat
