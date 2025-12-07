#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
EPIC GAMES AUTO-AUTH v7.0 - GOLDEN SESSION (ANTI-CRASH)
=======================================================
СИСТЕМА "ЗОЛОТОЙ СЛЕПОК" (STATE RESTORATION)

1. ПРИНЦИП РАБОТЫ:
   - Мы не надеемся на то, что Эпик "запомнил" нас с прошлого раза.
   - Мы КАЖДЫЙ РАЗ при запуске стираем его память и подкладываем "Идеальный Сейв".
   - Это гарантирует вход даже если ПК выключили из розетки.

2. РЕЖИМЫ:
   - [DEFAULT]: Восстановление сессии -> Запуск Эпика -> (Если надо) Ввод пароля.
   - [--backup]: Создание "Золотого слепка" из текущей сессии (для ТГ бота).

3. ПУТИ:
   - Бэкап: Документы/EpicSessionBackup
   - Рабочая папка: AppData/Local/EpicGamesLauncher/Saved
"""

import os
import sys
import time
import shutil
import ctypes
import argparse
import subprocess
from pathlib import Path
from datetime import datetime

# ═══════════════════════════════════════════════════════════════════════════════
#                              НАСТРОЙКИ (SETTINGS)
# ═══════════════════════════════════════════════════════════════════════════════

# Пути к папкам (используем переменные окружения Windows)
# ВАЖНО: Бэкап ищем в Documents\Saved (как вы указали)
BACKUP_DIR = os.path.expandvars(r'%USERPROFILE%\Documents\Saved')
TARGET_DIR = os.path.expandvars(r'%LOCALAPPDATA%\EpicGamesLauncher\Saved')

# Путь к EXE Эпика (Стандартный)
EPIC_EXE_PATH = r"C:\Program Files (x86)\Epic Games\Launcher\Portal\Binaries\Win32\EpicGamesLauncher.exe"

# Процессы, которые нужно убить перед операциями (Только Epic)
PROCESSES_TO_KILL = [
    "EpicGamesLauncher.exe",
    "EpicWebHelper.exe",
    "UnrealCEFSubProcess.exe"
]

# Настройки скрипта
SCRIPT_TIMEOUT = 300      # 5 минут (на случай если придется вводить пароль)
CHECK_INTERVAL = 2        # Интервал проверки
SCREEN_WIDTH = 1280
SCREEN_HEIGHT = 720

# Пути к файлам скрипта
SCRIPT_DIR = Path(__file__).parent.absolute()
RELEASE_DIR = SCRIPT_DIR.parent
IMAGES_DIR = RELEASE_DIR / "data" / "epic"
CONFIG_FILE = RELEASE_DIR / "config.txt"
LOG_FILE = RELEASE_DIR / "epic_auth.log"

# ═══════════════════════════════════════════════════════════════════════════════
#                              ЛОГИРОВАНИЕ
# ═══════════════════════════════════════════════════════════════════════════════

LOG_HANDLE = None

def log(msg):
    """Запись в лог и консоль."""
    global LOG_HANDLE
    timestamp = datetime.now().strftime("%H:%M:%S")
    line = f"[{timestamp}] {msg}"
    print(line, flush=True)
    try:
        if LOG_HANDLE is None:
            LOG_HANDLE = open(LOG_FILE, "w", encoding="utf-8")
        LOG_HANDLE.write(line + "\n")
        LOG_HANDLE.flush()
    except: pass

# ═══════════════════════════════════════════════════════════════════════════════
#                           СИСТЕМНЫЕ ФУНКЦИИ
# ═══════════════════════════════════════════════════════════════════════════════

def is_game_running():
    """Проверка: Запущена ли игра (GTA5 или RageMP)."""
    try:
        # Используем tasklist для проверки процессов
        output = subprocess.check_output("tasklist", shell=True).decode('cp866', errors='ignore')
        for proc in ["GTA5.exe", "ragemp_v.exe", "playgtav.exe"]:
            if proc.lower() in output.lower():
                return True
    except Exception as e:
        log(f"WARNING: Could not check processes: {e}")
    return False

def kill_processes():
    """
    Жесткое убийство всех процессов, связанных с Epic/GTA.
    Гарантирует, что файлы не заняты.
    """
    log(">>> KILLING PROCESSES...")
    for proc in PROCESSES_TO_KILL:
        # /F - force, /IM - image name, /T - tree (kill childs)
        cmd = f"taskkill /F /IM {proc} /T >nul 2>&1"
        os.system(cmd)
    
    # Ждем, пока система освободит хендлы файлов
    time.sleep(3)
    log("  Processes terminated.")

# ═══════════════════════════════════════════════════════════════════════════════
#                           РАБОТА С ФАЙЛАМИ (CORE)
# ═══════════════════════════════════════════════════════════════════════════════

def create_backup():
    """
    [РУЧНОЙ РЕЖИМ] Создание "Золотого слепка".
    Копирует текущую папку Saved в Документы.
    """
    log("=" * 60)
    log("CREATING GOLDEN SESSION BACKUP")
    log("=" * 60)
    
    if not os.path.exists(TARGET_DIR):
        log(f"ERROR: Source folder not found: {TARGET_DIR}")
        log("Please login to Epic Games first!")
        return False

    # 1. Убиваем процессы (чтобы скопировать все файлы)
    kill_processes()
    
    # 2. Удаляем старый бэкап если есть
    if os.path.exists(BACKUP_DIR):
        log(f"Removing old backup: {BACKUP_DIR}")
        try:
            shutil.rmtree(BACKUP_DIR)
        except Exception as e:
            log(f"ERROR removing old backup: {e}")
            return False
            
    # 3. Копируем
    log(f"Copying {TARGET_DIR} -> {BACKUP_DIR}")
    try:
        shutil.copytree(TARGET_DIR, BACKUP_DIR)
        log("SUCCESS: Backup created!")
        return True
    except Exception as e:
        log(f"ERROR copying backup: {e}")
        return False

def restore_session():
    """
    [АВТО РЕЖИМ] Восстановление сессии.
    Удаляет текущую папку и ставит бэкап.
    """
    log(">>> RESTORING SESSION...")
    
    if not os.path.exists(BACKUP_DIR):
        log("  No backup found. Skipping restore.")
        return False
        
    # 1. Удаляем текущую папку (WIPE)
    if os.path.exists(TARGET_DIR):
        log(f"  Wiping current AppData folder...")
        try:
            shutil.rmtree(TARGET_DIR)
        except Exception as e:
            log(f"  WARNING: Could not delete folder: {e}")
            # Пробуем еще раз через паузу
            time.sleep(2)
            try: shutil.rmtree(TARGET_DIR)
            except: pass
            
    # 2. Копируем бэкап (RESTORE)
    log(f"  Restoring from backup...")
    try:
        shutil.copytree(BACKUP_DIR, TARGET_DIR)
        log("  Session restored successfully!")
        return True
    except Exception as e:
        log(f"  ERROR restoring session: {e}")
        return False

def launch_epic():
    """Запуск Epic Games Launcher."""
    log(">>> LAUNCHING EPIC GAMES...")
    if os.path.exists(EPIC_EXE_PATH):
        subprocess.Popen(EPIC_EXE_PATH)
        log("  Launcher started.")
        return True
    else:
        log(f"ERROR: Epic EXE not found at {EPIC_EXE_PATH}")
        return False

# ═══════════════════════════════════════════════════════════════════════════════
#                              MAIN ENTRY POINT
# ═══════════════════════════════════════════════════════════════════════════════

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--backup", action="store_true", help="Create Golden Session Backup")
    args = parser.parse_args()

    log("=" * 60)
    log("EPIC GAMES MANAGER v7.1 - IMMORTAL CYCLE")
    log("=" * 60)
    log(f"Time: {datetime.now()}")

    # 1. РЕЖИМ БЭКАПА (Вызывается из ТГ бота вручную)
    if args.backup:
        if create_backup():
            print("BACKUP_OK") # Маркер для бота
            sys.exit(0)
        else:
            print("BACKUP_FAIL")
            sys.exit(1)

    # 2. РЕЖИМ ЗАПУСКА (Автоматический)
    
    # Шаг 0: Проверка на запущенную игру
    if is_game_running():
        log("GAME IS RUNNING (GTA5/RageMP). Skipping Auth & Restore.")
        log("Assuming session is active. Exiting.")
        sys.exit(0)

    # Шаг 1: Убиваем всё лишнее перед началом
    kill_processes()
    
    # Шаг 2: Восстанавливаем сессию (из Документов в AppData)
    # Если восстановление прошло успешно, запускаем цикл обновления токена
    if restore_session():
        # === ЦИКЛ БЕССМЕРТИЯ (ROLLING TOKEN) ===
        log(">>> [IMMORTAL CYCLE] Starting Token Refresh...")
        
        # А. Запускаем Эпик, чтобы он обновил токены
        launch_epic()
        
        # Б. Ждем 50 секунд (время на обмен токена) - увеличил для надежности
        log("  Waiting 50s for Epic to refresh internal tokens...")
        time.sleep(50)
        
        # В. Убиваем Эпик, чтобы забрать файлы
        kill_processes()
        
        # Г. Сохраняем обновленные файлы обратно в Документы
        log("  Updating Golden Session (Overwriting Backup)...")
        create_backup()
        
        log(">>> [IMMORTAL CYCLE] Token Refreshed Successfully!")
    
    # Шаг 3: Финальный запуск Эпика (уже для игры)
    launch_epic()
    
    log("Script finished.")
    if LOG_HANDLE: LOG_HANDLE.close()

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        log(f"CRITICAL: {e}")
        sys.exit(1)
