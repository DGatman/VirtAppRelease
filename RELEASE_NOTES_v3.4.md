# 🚀 VirtApp v3.4 — Immortal Cycle v8.0 & Updater v2.2

Update by: 👤 NIFILIM1337
Team: 🎯 Naguib's Team
Date: 📅 07.12.2025

━━━━━━━━━━━━━━━━━━━━━━━━

## 🛡️ IMMORTAL CYCLE v8.0 (Session Guard)
Полная переработка системы сохранения сессий Epic Games. Теперь токены никогда не "протухают".

### ⚙️ Новые механизмы:
1️⃣ **Session Guard (Страж):**
   - Фоновый процесс, который работает параллельно с игрой.
   - Каждые **5 минут** сохраняет свежий токен из Epic Games в бэкап.
   - Защищает от вылетов, отключения света и зависаний.

2️⃣ **Smart Init (Умный старт):**
   - Если при запуске бота игра уже работает — бот **не убивает** её.
   - Вместо этого он копирует токен ИЗ игры к себе (Capture), сохраняя прогресс.

3️⃣ **Exit Backup (Бэкап при выходе):**
   - При закрытии консоли бота (нажатие на крестик) срабатывает аварийное сохранение сессии.

━━━━━━━━━━━━━━━━━━━━━━━━

## 🔄 UPDATER v2.2 (Smart Update)
Глобальное обновление системы обновлений (`update.bat`).

### 🚀 Что нового:
🔹 **Auto-Repair:** Если папка сломана или не является Git-репозиторием, скрипт сам всё починит и скачает файлы.
🔹 **Config Protection:** Теперь сохраняются не только `config.txt`, но и `credentials.json`. Ваши данные не слетят.
🔹 **Encoding Fix:** Полная поддержка всех языков Windows (убраны эмодзи, вызывавшие ошибки).

━━━━━━━━━━━━━━━━━━━━━━━━

## 📦 Build Details
Bot Version:    v3.4 (Exit Handler Added)
Script Version: epic_auth.py v8.0
Updater:        v2.2
Compiler:       MSVC 2022 (Release)
Status:         STABLE / FAULT-TOLERANT

_© NIFILIM1337 | Naguib's Team | GTA5RP Automation_
