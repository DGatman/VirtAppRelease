# Project State: GTA5rpVirt

## ℹ️ About
Automation bot for GTA 5 RP servers using computer vision (OCR) and input simulation.
Handles authentication (Epic Games), queue management, and basic in-game actions.

## 🛠 Tech Stack
- **Core:** C++17 (Visual Studio 2022)
- **Build System:** CMake
- **Input:** Interception Driver (Kernel level mouse/keyboard)
- **Vision:** Tesseract OCR (v5), OpenCV (implied)
- **Network:** cURL, mailio (Email handling)
- **Scripting:** Python 3 (Time sync, Auth logic)
- **Libraries:** Boost, OpenSSL, SQLite

## 📦 Modules
- **VirtApp.exe:** Main bot executable (C++)
- **ScreenScaner:** Screen capture and OCR processing
- **scripts/set_local_time.py:** NTP/HTTP time synchronization
- **scripts/epic_auth.py:** Epic Games authentication automation
- **scripts/get_config.py:** Remote configuration sync (API client)
- **scripts/main.py:** Python entry point (Google Sheets integration)

## 🔄 Current Status
- **Version:** 3.6.2
- **Last Update:** 2025-12-18
- **State:** Active Development
- **Recent Focus:** Исправлен порядок инициализации (конфиг грузится до UI); токен читается из конфига; версия 3.6.2.
- **Build Note:** Windows сборка: добавлен `get_config.py` в автозапуск. Реализован механизм `softReboot` для сохранения сессий Epic Games перед перезагрузкой. Исправлена проблема с `Credentials` в Python скриптах.

## 📂 Directory Structure
- `src/`: C++ source code
- `scripts/`: Python helper scripts
- `Bot/`: Visual Studio solution files (Legacy/Generated)
- `VirtAppCMake/`: CMake configuration (Alternative)
- `build_final/`: Current build directory
