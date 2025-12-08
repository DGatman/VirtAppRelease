@echo off
chcp 65001 >nul
echo ========================================
echo  VirtApp v2.6 - ПЕРВЫЙ ЗАПУСК
echo ========================================
echo.

echo [1/2] Установка Interception драйвера...
echo (Требуются права администратора)
echo.

REM Проверяем права администратора
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo ❌ ОШИБКА: Запустите этот файл от имени администратора!
    echo Правый клик → Запуск от имени администратора
    echo.
    pause
    exit /b 1
)

REM Устанавливаем Interception драйвер
if exist "install-interception.exe" (
    install-interception.exe /install
    echo.
    echo ✅ Interception драйвер установлен!
) else (
    echo ⚠️ install-interception.exe не найден
)

echo.
echo [2/2] Установка Python зависимостей...
if exist "python-3.10.0-amd64.exe" (
    echo Запускаю установщик Python...
    start /wait python-3.10.0-amd64.exe /passive InstallAllUsers=1 PrependPath=1
) else (
    echo ⚠️ Python installer не найден - установите Python 3.10+ вручную
)

echo.
echo ========================================
echo ✅ УСТАНОВКА ЗАВЕРШЕНА!
echo ========================================
echo.
echo ВАЖНО: Перезагрузите компьютер для активации Interception драйвера!
echo После перезагрузки запустите start.bat
echo.
pause
