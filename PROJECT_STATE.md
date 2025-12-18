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
- **Version:** 3.6.3
- **Last Update:** 2025-12-18
- **State:** Active Development
- **Recent Focus:** КРИТИЧЕСКИЙ ФИКС: Полностью переработан порядок запуска (time sync → get_config → Dashboard UI). Теперь credentials.json создается корректно ДО старта основной логики.
- **Build Note:** Windows сборка: `get_config.py` теперь выполняется на 5-м шаге (после time sync, до Dashboard UI). Реализован механизм `softReboot` для сохранения сессий Epic Games перед перезагрузкой.

## 📂 Directory Structure
- `src/`: C++ source code
- `scripts/`: Python helper scripts
- `Bot/`: Visual Studio solution files (Legacy/Generated)
- `VirtAppCMake/`: CMake configuration (Alternative)
- `build_final/`: Current build directory
