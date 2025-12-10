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
- **scripts/main.py:** Python entry point (legacy/wrapper)

## 🔄 Current Status
- **Version:** 3.5.3
- **Last Update:** 2025-12-10
- **State:** Active Development
- **Recent Focus:** Версия выводится в формате 0.0.0 и прокинутa из сборки; основной артефакт теперь `VirtApp-<версия>.exe` с совместимым `VirtApp.exe`; рулетка-only по умолчанию, безопасный выход через F10, защита старта от пустого TOKEN/скриптов сохранена; сборка Windows чинит вызов `_access` через явный include `<io.h>`; **КРИТИЧНО: убран forceRelogin при провалах открытия телефона/рулетки — теперь просто скипит цикл без ребута ПК**
- **Build Note:** Windows сборка починена: явный include `<io.h>` для вызова `_access`. Блоки оплаты (phone/flat payment) больше не ребутят ПК при сбоях — логируют и пропускают.

## 📂 Directory Structure
- `src/`: C++ source code
- `scripts/`: Python helper scripts
- `Bot/`: Visual Studio solution files (Legacy/Generated)
- `VirtAppCMake/`: CMake configuration (Alternative)
- `build_final/`: Current build directory
