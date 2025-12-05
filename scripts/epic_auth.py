#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
╔══════════════════════════════════════════════════════════════════════════════╗
║                    EPIC GAMES AUTO-AUTH v3.0                                  ║
║                    Отказоустойчивая авторизация                              ║
║                    С использованием win32gui                                 ║
╚══════════════════════════════════════════════════════════════════════════════╝

Автоматическая авторизация в Epic Games Launcher.
Скрипт удерживает окно Epic на переднем плане и кликает по формам авторизации.

ЭТАПЫ:
1. Email - ввод логина и клик "Продолжить"  
2. Password - ввод пароля и клик "Войти"
3. Continue - финальное подтверждение

ИСПОЛЬЗУЕТ:
- win32gui/win32con - для гарантированной активации окна
- pyautogui - для кликов и ввода текста
- opencv - для детекции форм авторизации
"""

import os
import sys
import time
import subprocess
import logging
import ctypes
from pathlib import Path
from typing import Optional, Dict, Tuple
from datetime import datetime

# ══════════════════════════════════════════════════════════════════════════════
#                           НАСТРОЙКИ
# ══════════════════════════════════════════════════════════════════════════════

# Общий таймаут скрипта (в секундах) - 5 минут
SCRIPT_TIMEOUT = 300

# Интервал между проверками (в секундах)
CHECK_INTERVAL = 1

# Задержка после клика (в секундах)
CLICK_DELAY = 0.3

# Задержка после ввода текста (в секундах)
INPUT_DELAY = 0.2

# Задержка после нажатия кнопки (в секундах)
BUTTON_DELAY = 4.0

# Ожидаемое разрешение (все ПК = 1280x720)
SCREEN_WIDTH = 1280
SCREEN_HEIGHT = 720

# Порог совпадения для поиска изображений (0.0 - 1.0)
MATCH_THRESHOLD = 0.75

# Количество подтверждений перед действием
CONFIRM_COUNT = 2

# ─────────────────────────────────────────────────────────────────────────────
#                     ЭТАП 1: ВВОД EMAIL
# ─────────────────────────────────────────────────────────────────────────────

STAGE1_TEMPLATE = "epic_stage1_email.png"
STAGE1_EMAIL_FIELD_X = 640    # Центр поля Email 
STAGE1_EMAIL_FIELD_Y = 390    # Поле ввода Email
STAGE1_CONTINUE_BUTTON_X = 640  # Кнопка Continue по центру
STAGE1_CONTINUE_BUTTON_Y = 480  # Кнопка Continue

# ─────────────────────────────────────────────────────────────────────────────
#                     ЭТАП 2: ВВОД PASSWORD  
# ─────────────────────────────────────────────────────────────────────────────

STAGE2_TEMPLATE = "epic_stage2_password.png"
STAGE2_PASSWORD_FIELD_X = 640   # Поле пароля по центру
STAGE2_PASSWORD_FIELD_Y = 360   # Поле ввода пароля
STAGE2_LOGIN_BUTTON_X = 640     # Кнопка Login по центру
STAGE2_LOGIN_BUTTON_Y = 440     # Кнопка Login

# ─────────────────────────────────────────────────────────────────────────────
#                     ЭТАП 3: ПОДТВЕРЖДЕНИЕ
# ─────────────────────────────────────────────────────────────────────────────

STAGE3_TEMPLATE = "epic_stage3_continue.png"
STAGE3_CONTINUE_BUTTON_X = 640  # По центру
STAGE3_CONTINUE_BUTTON_Y = 500


# ══════════════════════════════════════════════════════════════════════════════
#                    УСТАНОВКА ЗАВИСИМОСТЕЙ
# ══════════════════════════════════════════════════════════════════════════════

def install_package(package: str):
    """Установка пакета через pip."""
    subprocess.check_call([sys.executable, "-m", "pip", "install", package, "-q"],
                          stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

# Импорты с автоустановкой
try:
    import pyautogui
    pyautogui.FAILSAFE = False
    pyautogui.PAUSE = 0.05
except ImportError:
    print("Устанавливаю pyautogui...", flush=True)
    install_package("pyautogui")
    import pyautogui
    pyautogui.FAILSAFE = False
    pyautogui.PAUSE = 0.05

try:
    import cv2
    import numpy as np
except ImportError:
    print("Устанавливаю opencv-python...", flush=True)
    install_package("opencv-python")
    import cv2
    import numpy as np

try:
    from PIL import Image
except ImportError:
    print("Устанавливаю Pillow...", flush=True)
    install_package("Pillow")
    from PIL import Image

try:
    import pyperclip
except ImportError:
    print("Устанавливаю pyperclip...", flush=True)
    install_package("pyperclip")
    import pyperclip

try:
    import win32gui
    import win32con
    import win32api
except ImportError:
    print("Устанавливаю pywin32...", flush=True)
    install_package("pywin32")
    import win32gui
    import win32con
    import win32api


# ══════════════════════════════════════════════════════════════════════════════
#                          ПУТИ К ФАЙЛАМ
# ══════════════════════════════════════════════════════════════════════════════

SCRIPT_DIR = Path(__file__).parent.absolute()
RELEASE_DIR = SCRIPT_DIR.parent
CONFIG_FILE = RELEASE_DIR / "config.txt"
TEMPLATES_DIR = RELEASE_DIR / "data" / "epic"
LOG_FILE = RELEASE_DIR / "epic_auth.log"


# ══════════════════════════════════════════════════════════════════════════════
#                              ЛОГИРОВАНИЕ
# ══════════════════════════════════════════════════════════════════════════════

def setup_logging() -> logging.Logger:
    """Настройка логирования."""
    log_format = "[%(asctime)s] %(levelname)s - %(message)s"
    date_format = "%H:%M:%S"
    
    logger = logging.getLogger("epic_auth")
    logger.setLevel(logging.DEBUG)
    logger.handlers.clear()
    
    try:
        file_handler = logging.FileHandler(LOG_FILE, encoding="utf-8", mode="w")
        file_handler.setLevel(logging.DEBUG)
        file_handler.setFormatter(logging.Formatter(log_format, date_format))
        logger.addHandler(file_handler)
    except Exception as e:
        print(f"Ошибка создания лог-файла: {e}", flush=True)
    
    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setLevel(logging.INFO)
    console_handler.setFormatter(logging.Formatter(log_format, date_format))
    logger.addHandler(console_handler)
    
    return logger

print("=== Epic Auth v3.0 Starting ===", flush=True)
print(f"Log file: {LOG_FILE}", flush=True)
log = setup_logging()


# ══════════════════════════════════════════════════════════════════════════════
#                         ЗАГРУЗКА КОНФИГУРАЦИИ
# ══════════════════════════════════════════════════════════════════════════════

def load_config() -> Dict[str, str]:
    """Загрузка config.txt."""
    config = {}
    if not CONFIG_FILE.exists():
        log.warning(f"config.txt не найден: {CONFIG_FILE}")
        return config
    
    try:
        with open(CONFIG_FILE, "r", encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if "=" in line and not line.startswith("#"):
                    key, value = line.split("=", 1)
                    config[key.strip()] = value.strip()
    except Exception as e:
        log.error(f"Ошибка чтения config: {e}")
    
    return config


def get_credentials() -> Tuple[Optional[str], Optional[str]]:
    """Получение логина и пароля из конфига."""
    config = load_config()
    email = config.get("EpicLogin", "")
    password = config.get("EpicPassword", "")
    
    if not email or not password:
        log.error("EpicLogin или EpicPassword не заданы в config.txt!")
        return None, None
    
    log.info(f"Credentials: {email[:3]}***")
    return email, password


# ══════════════════════════════════════════════════════════════════════════════
#                    РАБОТА С ОКНОМ EPIC (WIN32GUI)
# ══════════════════════════════════════════════════════════════════════════════

EPIC_HWND = None  # Глобальный хэндл окна

def find_epic_hwnd() -> Optional[int]:
    """Поиск окна Epic Games Launcher через win32gui."""
    global EPIC_HWND
    
    result = []
    
    def enum_callback(hwnd, results):
        if win32gui.IsWindowVisible(hwnd):
            title = win32gui.GetWindowText(hwnd)
            title_lower = title.lower()
            if "epic games" in title_lower or "epic launcher" in title_lower:
                results.append((hwnd, title))
    
    try:
        win32gui.EnumWindows(enum_callback, result)
    except Exception as e:
        log.debug(f"EnumWindows error: {e}")
    
    if result:
        EPIC_HWND = result[0][0]
        log.debug(f"Найдено окно: hwnd={EPIC_HWND}, title='{result[0][1]}'")
        return EPIC_HWND
    
    return None


def force_foreground(hwnd: int) -> bool:
    """
    Принудительный вывод окна на передний план.
    Использует несколько методов для гарантии.
    """
    try:
        # Метод 1: Восстановить если свёрнуто
        if win32gui.IsIconic(hwnd):
            win32gui.ShowWindow(hwnd, win32con.SW_RESTORE)
            time.sleep(0.3)
        
        # Метод 2: ShowWindow SHOW
        win32gui.ShowWindow(hwnd, win32con.SW_SHOW)
        time.sleep(0.1)
        
        # Метод 3: BringWindowToTop
        win32gui.BringWindowToTop(hwnd)
        time.sleep(0.1)
        
        # Метод 4: SetForegroundWindow (главный метод)
        # Хитрость: сначала симулируем нажатие Alt для разблокировки
        shell = ctypes.windll.user32
        shell.keybd_event(0x12, 0, 0, 0)  # Alt press
        shell.keybd_event(0x12, 0, 2, 0)  # Alt release
        time.sleep(0.05)
        
        win32gui.SetForegroundWindow(hwnd)
        time.sleep(0.2)
        
        # Метод 5: Максимизировать окно
        win32gui.ShowWindow(hwnd, win32con.SW_MAXIMIZE)
        time.sleep(0.2)
        
        # Проверка
        fg_hwnd = win32gui.GetForegroundWindow()
        if fg_hwnd == hwnd:
            return True
        else:
            log.debug(f"Окно не на переднем плане. FG={fg_hwnd}, Epic={hwnd}")
            return False
            
    except Exception as e:
        log.debug(f"force_foreground error: {e}")
        return False


def activate_epic_window() -> bool:
    """Активация окна Epic Games."""
    global EPIC_HWND
    
    # Ищем окно если нет хэндла
    if EPIC_HWND is None or not win32gui.IsWindow(EPIC_HWND):
        find_epic_hwnd()
    
    if EPIC_HWND is None:
        log.warning("Окно Epic не найдено!")
        return False
    
    return force_foreground(EPIC_HWND)


def get_window_rect() -> Optional[Tuple[int, int, int, int]]:
    """Получение координат окна Epic."""
    global EPIC_HWND
    
    if EPIC_HWND is None:
        find_epic_hwnd()
    
    if EPIC_HWND is None:
        return None
    
    try:
        rect = win32gui.GetWindowRect(EPIC_HWND)
        return rect  # (left, top, right, bottom)
    except:
        return None


# ══════════════════════════════════════════════════════════════════════════════
#                         ДЕТЕКЦИЯ ИЗОБРАЖЕНИЙ
# ══════════════════════════════════════════════════════════════════════════════

def take_screenshot() -> np.ndarray:
    """Скриншот экрана."""
    screenshot = pyautogui.screenshot()
    screenshot_np = np.array(screenshot)
    return cv2.cvtColor(screenshot_np, cv2.COLOR_RGB2BGR)


def load_template(template_name: str) -> Optional[np.ndarray]:
    """Загрузка шаблона."""
    template_path = TEMPLATES_DIR / template_name
    if not template_path.exists():
        log.debug(f"Шаблон не найден: {template_path}")
        return None
    return cv2.imread(str(template_path))


def detect_template(template_name: str) -> Tuple[bool, Optional[Tuple[int, int]]]:
    """
    Детекция шаблона на экране.
    Returns: (found, (center_x, center_y))
    """
    template = load_template(template_name)
    if template is None:
        return False, None
    
    screenshot = take_screenshot()
    
    try:
        result = cv2.matchTemplate(screenshot, template, cv2.TM_CCOEFF_NORMED)
        _, max_val, _, max_loc = cv2.minMaxLoc(result)
        
        if max_val >= MATCH_THRESHOLD:
            h, w = template.shape[:2]
            center_x = max_loc[0] + w // 2
            center_y = max_loc[1] + h // 2
            log.debug(f"Шаблон {template_name}: found at ({center_x}, {center_y}), score={max_val:.2f}")
            return True, (center_x, center_y)
        
        return False, None
        
    except Exception as e:
        log.debug(f"matchTemplate error: {e}")
        return False, None


# ══════════════════════════════════════════════════════════════════════════════
#                              КЛИКИ И ВВОД
# ══════════════════════════════════════════════════════════════════════════════

def click_at(x: int, y: int, description: str = ""):
    """Клик по абсолютным координатам экрана."""
    log.info(f"  Клик: ({x}, {y}) {description}")
    
    # Активируем окно перед кликом
    activate_epic_window()
    time.sleep(0.1)
    
    # Перемещаем мышь и кликаем
    pyautogui.moveTo(x, y, duration=0.1)
    time.sleep(0.05)
    pyautogui.click(x, y)
    time.sleep(CLICK_DELAY)


def clear_and_type(text: str):
    """Очистка поля и ввод текста."""
    # Тройной клик для выделения всего
    pyautogui.click()
    pyautogui.click()
    pyautogui.click()
    time.sleep(0.1)
    
    # Ctrl+A для выделения всего
    pyautogui.hotkey("ctrl", "a")
    time.sleep(0.1)
    
    # Delete для удаления
    pyautogui.press("delete")
    time.sleep(0.1)
    
    # Вставка через буфер обмена
    try:
        pyperclip.copy(text)
        pyautogui.hotkey("ctrl", "v")
    except Exception:
        pyautogui.typewrite(text, interval=0.03)
    
    time.sleep(INPUT_DELAY)


def press_enter():
    """Нажатие Enter."""
    pyautogui.press("enter")
    time.sleep(0.3)


# ══════════════════════════════════════════════════════════════════════════════
#                         ЭТАПЫ АВТОРИЗАЦИИ
# ══════════════════════════════════════════════════════════════════════════════

def execute_stage1(email: str) -> bool:
    """ЭТАП 1: Ввод Email"""
    log.info("")
    log.info("╔════════════════════════════════════════╗")
    log.info("║     ЭТАП 1: ВВОД EMAIL                 ║")
    log.info("╚════════════════════════════════════════╝")
    
    try:
        activate_epic_window()
        time.sleep(0.5)
        
        # Ищем поле email на экране
        found, pos = detect_template(STAGE1_TEMPLATE)
        if found and pos:
            # Кликаем немного ниже найденного элемента (в поле ввода)
            click_at(pos[0], pos[1] + 50, "поле Email")
        else:
            # Fallback на фиксированные координаты
            click_at(STAGE1_EMAIL_FIELD_X, STAGE1_EMAIL_FIELD_Y, "поле Email (fallback)")
        
        # Вводим email
        log.info(f"  Ввод Email: {email[:3]}***")
        clear_and_type(email)
        
        # Нажимаем Enter или Tab + Enter
        time.sleep(0.3)
        pyautogui.press("tab")
        time.sleep(0.2)
        pyautogui.press("enter")
        
        log.info("  Этап 1 выполнен!")
        time.sleep(BUTTON_DELAY)
        return True
        
    except Exception as e:
        log.error(f"  Ошибка этапа 1: {e}")
        return False


def execute_stage2(password: str) -> bool:
    """ЭТАП 2: Ввод Password"""
    log.info("")
    log.info("╔════════════════════════════════════════╗")
    log.info("║     ЭТАП 2: ВВОД PASSWORD              ║")
    log.info("╚════════════════════════════════════════╝")
    
    try:
        activate_epic_window()
        time.sleep(0.5)
        
        # Ищем поле пароля на экране
        found, pos = detect_template(STAGE2_TEMPLATE)
        if found and pos:
            click_at(pos[0], pos[1] + 50, "поле Password")
        else:
            click_at(STAGE2_PASSWORD_FIELD_X, STAGE2_PASSWORD_FIELD_Y, "поле Password (fallback)")
        
        log.info("  Ввод Password: ***")
        clear_and_type(password)
        
        # Enter для отправки формы
        time.sleep(0.3)
        pyautogui.press("enter")
        
        log.info("  Этап 2 выполнен!")
        time.sleep(BUTTON_DELAY)
        return True
        
    except Exception as e:
        log.error(f"  Ошибка этапа 2: {e}")
        return False


def execute_stage3() -> bool:
    """ЭТАП 3: Подтверждение"""
    log.info("")
    log.info("╔════════════════════════════════════════╗")
    log.info("║     ЭТАП 3: ПОДТВЕРЖДЕНИЕ              ║")
    log.info("╚════════════════════════════════════════╝")
    
    try:
        activate_epic_window()
        time.sleep(0.5)
        
        # Ищем кнопку подтверждения
        found, pos = detect_template(STAGE3_TEMPLATE)
        if found and pos:
            click_at(pos[0], pos[1], "кнопка Continue")
        else:
            click_at(STAGE3_CONTINUE_BUTTON_X, STAGE3_CONTINUE_BUTTON_Y, "кнопка Continue (fallback)")
        
        # Также нажмём Enter
        time.sleep(0.3)
        pyautogui.press("enter")
        
        log.info("  Этап 3 выполнен!")
        time.sleep(BUTTON_DELAY)
        return True
        
    except Exception as e:
        log.error(f"  Ошибка этапа 3: {e}")
        return False


# ══════════════════════════════════════════════════════════════════════════════
#                              ГЛАВНЫЙ ЦИКЛ
# ══════════════════════════════════════════════════════════════════════════════

def main():
    """Главная функция."""
    log.info("")
    log.info("╔══════════════════════════════════════════════════╗")
    log.info("║       EPIC GAMES AUTO-AUTH v3.0                  ║")
    log.info("║       С использованием win32gui                  ║")
    log.info("╚══════════════════════════════════════════════════╝")
    log.info("")
    log.info(f"Время: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    log.info(f"Таймаут: {SCRIPT_TIMEOUT} сек ({SCRIPT_TIMEOUT//60} мин)")
    log.info("")
    
    # Проверяем шаблоны
    for template in [STAGE1_TEMPLATE, STAGE2_TEMPLATE, STAGE3_TEMPLATE]:
        path = TEMPLATES_DIR / template
        status = "[OK]" if path.exists() else "[--]"
        log.info(f"  {status} {template}")
    log.info("")
    
    # Загружаем креды
    email, password = get_credentials()
    if not email or not password:
        log.error("Не удалось загрузить креды!")
        return
    
    log.info("")
    log.info("══════════════════════════════════════════════════")
    log.info("МОНИТОРИНГ ЗАПУЩЕН")
    log.info("══════════════════════════════════════════════════")
    log.info("")
    
    start_time = time.time()
    check_number = 0
    stages_completed = {1: False, 2: False, 3: False}
    stage1_attempts = 0
    MAX_STAGE1_ATTEMPTS = 5  # Максимум 5 попыток на этап 1
    
    while True:
        elapsed = time.time() - start_time
        remaining = SCRIPT_TIMEOUT - elapsed
        
        if elapsed >= SCRIPT_TIMEOUT:
            log.info("")
            log.info(f"ТАЙМАУТ ({SCRIPT_TIMEOUT} сек)")
            break
        
        check_number += 1
        
        # Логируем каждые 30 проверок
        if check_number % 30 == 1:
            log.info(f"─── Проверка #{check_number} | Осталось: {remaining:.0f} сек ───")
        
        # ВАЖНО: Активируем окно Epic КАЖДУЮ итерацию
        activate_epic_window()
        
        # ─────────────────────────────────────────────────────────────────────
        # ПРОВЕРКА ЭТАПА 3 (проверяем первым - финальный этап)
        # ─────────────────────────────────────────────────────────────────────
        found3, _ = detect_template(STAGE3_TEMPLATE)
        if found3 and not stages_completed[3]:
            log.info(">>> Обнаружен ЭТАП 3 (Подтверждение)")
            if execute_stage3():
                stages_completed[3] = True
                log.info("")
                log.info("ВСЕ ЭТАПЫ ЗАВЕРШЕНЫ!")
                # Держим окно ещё 30 сек
                for _ in range(30):
                    activate_epic_window()
                    time.sleep(1)
                break
            continue
        
        # ─────────────────────────────────────────────────────────────────────
        # ПРОВЕРКА ЭТАПА 2 (пароль)
        # ─────────────────────────────────────────────────────────────────────
        found2, _ = detect_template(STAGE2_TEMPLATE)
        if found2 and not stages_completed[2]:
            log.info(">>> Обнаружен ЭТАП 2 (Password)")
            if execute_stage2(password):
                stages_completed[2] = True
                stage1_attempts = 0  # Сброс - мы прошли дальше
            continue
        
        # ─────────────────────────────────────────────────────────────────────
        # ПРОВЕРКА ЭТАПА 1 (email) - с лимитом попыток
        # ─────────────────────────────────────────────────────────────────────
        found1, _ = detect_template(STAGE1_TEMPLATE)
        if found1 and not stages_completed[1]:
            stage1_attempts += 1
            log.info(f">>> Обнаружен ЭТАП 1 (Email) - попытка {stage1_attempts}/{MAX_STAGE1_ATTEMPTS}")
            
            if stage1_attempts > MAX_STAGE1_ATTEMPTS:
                log.warning("Превышен лимит попыток на этап 1. Возможно, неверные координаты или креды.")
                log.warning("Пропускаем этап 1, ждём переход...")
                time.sleep(5)
                continue
            
            if execute_stage1(email):
                # Не помечаем как completed - ждём переход на этап 2
                pass
            continue
        
        # Если ничего не обнаружено - ждём
        time.sleep(CHECK_INTERVAL)
    
    # Итоги
    log.info("")
    log.info("══════════════════════════════════════════════════")
    log.info("ИТОГИ:")
    log.info(f"  Этап 1 (Email):        {'OK' if stages_completed[1] else '--'}")
    log.info(f"  Этап 2 (Password):     {'OK' if stages_completed[2] else '--'}")
    log.info(f"  Этап 3 (Подтверждение): {'OK' if stages_completed[3] else '--'}")
    log.info(f"  Время работы: {time.time() - start_time:.1f} сек")
    log.info("══════════════════════════════════════════════════")
    log.info("Скрипт завершён.")


# ══════════════════════════════════════════════════════════════════════════════
#                         ТОЧКА ВХОДА
# ══════════════════════════════════════════════════════════════════════════════

if __name__ == "__main__":
    try:
        main()
        sys.exit(0)
    except KeyboardInterrupt:
        log.info("\nПрервано пользователем")
        sys.exit(0)
    except Exception as e:
        log.exception(f"Критическая ошибка: {e}")
        sys.exit(1)
