import os
import sys
import shutil
import time
import subprocess
import ctypes
from datetime import datetime

# ==================================================================================
# CONFIGURATION
# ==================================================================================
LOCAL_APP_DATA = os.environ.get('LOCALAPPDATA')
USER_PROFILE = os.environ.get('USERPROFILE')

# Пути к папкам
EPIC_SOURCE_DIR = os.path.join(LOCAL_APP_DATA, r"EpicGamesLauncher\Saved")
BACKUP_DIR = os.path.join(USER_PROFILE, r"Documents\GTA5rpVirt_EpicBackup")

# Путь к экзешнику Epic Games
EPIC_EXE_PATH = r"C:\Program Files (x86)\Epic Games\Launcher\Portal\Binaries\Win32\EpicGamesLauncher.exe"

# Интервал сохранения (в секундах) для Guard режима
GUARD_INTERVAL = 300  # 5 минут

# ==================================================================================
# SYSTEM FUNCTIONS
# ==================================================================================

def log(message):
    """Логирование с меткой времени"""
    timestamp = datetime.now().strftime("%H:%M:%S")
    print(f"[{timestamp}] {message}")

def is_process_running(process_name):
    """Проверяет, запущен ли процесс, через tasklist (без лишних библиотек)"""
    try:
        # Фильтр по имени процесса, вывод в csv без заголовков
        cmd = f'tasklist /FI "IMAGENAME eq {process_name}" /FO CSV /NH'
        output = subprocess.check_output(cmd, shell=True).decode('cp866', errors='ignore')
        return process_name.lower() in output.lower()
    except Exception:
        return False

def kill_process(process_name):
    """Принудительное завершение процесса"""
    log(f"Killing {process_name}...")
    os.system(f"taskkill /f /im {process_name} >nul 2>&1")

def copy_tree_recursive(src, dst):
    """Рекурсивное копирование папки с заменой"""
    if not os.path.exists(src):
        log(f"Warning: Source not found {src}")
        return

    if os.path.exists(dst):
        shutil.rmtree(dst)
    
    try:
        shutil.copytree(src, dst, dirs_exist_ok=True, ignore_dangling_symlinks=True)
        log(f"Synced: {src} -> {dst}")
    except Exception as e:
        log(f"Copy error (non-critical): {e}")

# ==================================================================================
# CORE LOGIC
# ==================================================================================

def perform_backup():
    """CAPTURE: Копирует текущую сессию из Epic в Documents"""
    log("Performing BACKUP (Epic -> Docs)...")
    
    # 1. Config (Настройки пользователя)
    copy_tree_recursive(
        os.path.join(EPIC_SOURCE_DIR, "Config"),
        os.path.join(BACKUP_DIR, "Config")
    )
    
    # 2. Data/Webcache (Токены авторизации)
    # Epic часто держит файлы webcache заблокированными.
    # Мы используем ignore_errors=True при копировании, чтобы взять то, что доступно.
    src_web = os.path.join(EPIC_SOURCE_DIR, "webcache")
    dst_web = os.path.join(BACKUP_DIR, "webcache")
    
    if os.path.exists(src_web):
        if os.path.exists(dst_web):
            try:
                shutil.rmtree(dst_web)
            except:
                pass # Если не удалилось, перезапишем поверх
        
        try:
            shutil.copytree(src_web, dst_web, dirs_exist_ok=True)
        except Exception:
            pass # Игнорируем ошибки занятых файлов, главное скопировать Cookies
            
    log("Backup completed.")

def perform_restore():
    """RESTORE: Восстанавливает сессию из Documents в Epic"""
    log("Performing RESTORE (Docs -> Epic)...")
    if not os.path.exists(BACKUP_DIR):
        log("No backup found. Skipping restore.")
        return

    # Создаем папку назначения если нет
    if not os.path.exists(EPIC_SOURCE_DIR):
        os.makedirs(EPIC_SOURCE_DIR)

    # Восстанавливаем
    copy_tree_recursive(BACKUP_DIR, EPIC_SOURCE_DIR)
    log("Restore completed.")

def launch_epic():
    """Запуск Epic Games Launcher"""
    log("Launching Epic Games Launcher...")
    if os.path.exists(EPIC_EXE_PATH):
        subprocess.Popen([EPIC_EXE_PATH, "-silent", "-hidetray"], close_fds=True)
    else:
        log(f"Error: Epic EXE not found at {EPIC_EXE_PATH}")

# ==================================================================================
# MODES
# ==================================================================================

def guard_mode():
    """
    Режим СТРАЖА.
    Висит в фоне и каждые 5 минут обновляет бэкап, пока Эпик запущен.
    """
    # Ждем немного перед первым запуском, чтобы основной процесс успел выйти
    time.sleep(10)
    
    while True:
        # Проверяем, жива ли игра или лаунчер
        epic_alive = is_process_running("EpicGamesLauncher.exe")
        gta_alive = is_process_running("GTA5.exe") or is_process_running("ragemp_v.exe")
        
        if epic_alive or gta_alive:
            # Если игра запущена, токен точно свежий. Бэкапим.
            perform_backup()
        else:
            # Если всё закрыто - страж больше не нужен, выходим.
            break
            
        time.sleep(GUARD_INTERVAL)

def main():
    # Проверка аргументов запуска
    if len(sys.argv) > 1:
        if sys.argv[1] == "--guard":
            guard_mode()
            return
        elif sys.argv[1] == "--backup-only":
            log("--- EMERGENCY BACKUP REQUESTED ---")
            perform_backup()
            return

    log("--- IMMORTAL CYCLE v8.0 START ---")

    # 1. SMART INIT: Проверяем, запущен ли уже Эпик
    if is_process_running("EpicGamesLauncher.exe"):
        log("Status: Epic Games is ALREADY RUNNING.")
        log("Action: Skipping kill/restore. Updating backup from current session.")
        perform_backup()
    else:
        log("Status: Epic Games is NOT running.")
        log("Action: Restoring backup and launching.")
        perform_restore()
        launch_epic()
        
        # Ждем инициализации
        log("Waiting for Epic to initialize (20s)...")
        time.sleep(20)
        
        # Делаем первичный бэкап
        perform_backup()

    # 2. ЗАПУСК СТРАЖА (GUARD)
    # Запускаем этот же скрипт в отдельном процессе с флагом --guard
    # creationflags=0x08000000 (CREATE_NO_WINDOW) чтобы не было окна консоли
    log("Spawning Session Guard process...")
    subprocess.Popen(
        [sys.executable, os.path.abspath(__file__), "--guard"],
        creationflags=0x08000000,
        close_fds=True
    )
    log("Guard spawned. Main script exiting.")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        log(f"Critical Error: {e}")
        time.sleep(5)
