# Changelog

All notable changes to this project will be documented in this file.

## [3.6.2] - 2025-02-20

### Fixed
- **Config Loading:** `scripts/get_config.py` now executes *after* console initialization, ensuring logs are visible for debugging.
- **Token Fallback:** Added logic to read `Token` from `config.txt` if the `Token` environment variable is not set.
- **Build System:** Fixed `BUILD_v2.6_SCRIPT.bat` to correctly locate `cmake.exe` in Visual Studio 2022 and use the correct vcpkg triplet (`x64-windows`).

## [3.6.0] - 2024-05-22

### Added
- Интеграция `scripts/get_config.py` для автоматического получения `config.txt` и `credentials.json` с удаленного API при старте.
- Новая команда Telegram `/update_config` для принудительного обновления конфигурации без перезапуска бота.
- Механизм "мягкого ребута" (`softReboot`): бот корректно закрывает Epic Games и выполняет бэкап сессии перед системной перезагрузкой.
- Функция `execPythonWithFallback` в C++ для поддержки вызова Python через `python` или `py`.

### Fixed
- Исправлена критическая ошибка `NameError: name 'Credentials' is not defined` в `scripts/main.py` при автоматической установке зависимостей.
- Синхронизированы версии `main.py` во всех директориях проекта (`scripts/`, `Release/`, корень).
- Улучшена стабильность работы с Google Sheets: данные теперь отправляются корректно после авто-установки библиотек.

## [3.5.3] - 2025-12-10

### Changed
- Версия теперь задается одной переменной сборки (`VIRTAPP_VERSION`) и выводится в формате `3.5.3` без префикса `v`.
- Сборка генерирует основной артефакт `VirtApp-3.5.3.exe` и автоматически клонирует его в совместимый `VirtApp.exe` для скриптов.
- Стартовый баннер/лог `Current version` читают ту же версию из сборки, чтобы не расходиться с именем бинаря.

### Fixed
- Исправлена сборка на Windows: добавлен явный include `<io.h>` для вызова `_access`.
- Убран `forceRelogin = true` в блоках оплаты (phone/flat payment): при неоткрытии телефона бот теперь логирует ошибку и пропускает цикл вместо ребута ПК.

## [3.5.2] - 2025-12-09

### Changed
- Отключен сбор подарков/батл пасса: оставлена только рулетка в основном цикле (`main.cpp`).
- Обновлена строка версии Dashboard до `v3.5.2`.
- Стартовый баннер синхронизирован с версией `v3.5.2`; presents по умолчанию выключены (статус показывает DISABLED).
- Добавлена безопасная инициализация токена (пустой TOKEN не блокирует старт) и лог при отсутствии; автоустановка python/main.py/command_cmd.bat выполняется только при наличии файлов, чтобы избежать зависаний на старте.

## [3.5.1] - 2025-12-08

### Changed
- Dashboard version string bumped to `v3.5.1` in `main.cpp`.
- Presents menu safe-exit now uses `F10` instead of `ESC` to avoid opening the pause menu.

### Fixed
- Removed `forceRelogin` triggers in presents/ruletka flow; bot now логирует и пропускает цикл без ребута.

## [Unreleased] - 2023-10-27

### Added
- **Input Passthrough:** Added dedicated thread in `main.cpp` to forward physical mouse/keyboard input immediately.
  - Solves "frozen mouse" issue when bot is running but not active.
  - Removes dependency on Task Manager for input unblocking.
- **Build System:** Established CMake build workflow in `build_final`.

### Changed
- **Time Sync:** Optimized `scripts/set_local_time.py`.
  - Reduced `MAX_GLOBAL_RETRIES` from 10 to 2.
  - Reduced `RETRY_DELAY` from 3s to 1s.
  - Startup time significantly improved (from ~5-10 mins to <30s).

### Fixed
- **Compilation:** Fixed build environment issues by locating correct CMake path in Visual Studio 2022.
