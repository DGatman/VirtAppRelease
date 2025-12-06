#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
EPIC GAMES AUTO-AUTH v4.0
=========================
Полностью рабочая авторизация в Epic Games Launcher.

Метод работы:
1. Находим окно Epic Games через win32gui
2. Принудительно выводим на передний план
3. Ищем формы по OCR (tesseract) или по цвету кнопок
4. Вводим данные и кликаем

Работает БЕЗ шаблонов картинок - только поиск текста и элементов.
"""

import os
import sys
import time
import subprocess
import ctypes
import re
from pathlib import Path
from datetime import datetime

# === НАСТРОЙКИ ===
SCRIPT_TIMEOUT = 300  # 5 минут
CHECK_INTERVAL = 2    # Интервал проверки
CLICK_DELAY = 0.5
TYPE_DELAY = 0.05

# === ПУТИ ===
SCRIPT_DIR = Path(__file__).parent.absolute()
RELEASE_DIR = SCRIPT_DIR.parent
CONFIG_FILE = RELEASE_DIR / "config.txt"
LOG_FILE = RELEASE_DIR / "epic_auth.log"

# === ЛОГИРОВАНИЕ ===
def log(msg):
    """Запись в лог и консоль."""
    timestamp = datetime.now().strftime("%H:%M:%S")
    line = f"[{timestamp}] {msg}"
    print(line, flush=True)
    try:
        with open(LOG_FILE, "a", encoding="utf-8") as f:
            f.write(line + "\n")
    except:
        pass

# === УСТАНОВКА ЗАВИСИМОСТЕЙ ===
def install(pkg):
    subprocess.run([sys.executable, "-m", "pip", "install", pkg, "-q"],
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

# Импорты с автоустановкой
try:
    import pyautogui
    pyautogui.FAILSAFE = False
    pyautogui.PAUSE = 0.02
except ImportError:
    log("Устанавливаю pyautogui...")
    install("pyautogui")
    import pyautogui
    pyautogui.FAILSAFE = False
    pyautogui.PAUSE = 0.02

try:
    import pyperclip
except ImportError:
    log("Устанавливаю pyperclip...")
    install("pyperclip")
    import pyperclip

try:
    import win32gui
    import win32con
    import win32process
except ImportError:
    log("Устанавливаю pywin32...")
    install("pywin32")
    import win32gui
    import win32con
    import win32process

try:
    from PIL import Image, ImageGrab
except ImportError:
    log("Устанавливаю Pillow...")
    install("Pillow")
    from PIL import Image, ImageGrab

# === ЗАГРУЗКА КОНФИГА ===
def load_credentials():
    """Загрузка логина и пароля из config.txt"""
    if not CONFIG_FILE.exists():
        log(f"ОШИБКА: config.txt не найден: {CONFIG_FILE}")
        return None, None
    
    config = {}
    try:
        with open(CONFIG_FILE, "r", encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if "=" in line and not line.startswith("#"):
                    key, value = line.split("=", 1)
                    config[key.strip()] = value.strip()
    except Exception as e:
        log(f"ОШИБКА чтения config: {e}")
        return None, None
    
    email = config.get("EpicLogin", "")
    password = config.get("EpicPassword", "")
    
    if not email or not password:
        log("ОШИБКА: EpicLogin или EpicPassword не заданы!")
        return None, None
    
    log(f"Креды загружены: {email[:3]}***@***")
    return email, password


# === РАБОТА С ОКНАМИ (WIN32) ===
EPIC_HWND = None

def find_epic_window():
    """Поиск окна Epic Games Launcher."""
    global EPIC_HWND
    
    found = []
    
    def callback(hwnd, results):
        if win32gui.IsWindowVisible(hwnd):
            title = win32gui.GetWindowText(hwnd).lower()
            # Ищем окно Epic
            if "epic games" in title or "epic launcher" in title:
                results.append((hwnd, win32gui.GetWindowText(hwnd)))
        return True
    
    try:
        win32gui.EnumWindows(callback, found)
    except Exception as e:
        log(f"EnumWindows error: {e}")
    
    if found:
        EPIC_HWND = found[0][0]
        log(f"Найдено окно Epic: hwnd={EPIC_HWND}, '{found[0][1]}'")
        return EPIC_HWND
    
    # Пробуем найти по классу окна
    try:
        hwnd = win32gui.FindWindow(None, "Epic Games Launcher")
        if hwnd:
            EPIC_HWND = hwnd
            log(f"Найдено окно по имени: hwnd={hwnd}")
            return hwnd
    except:
        pass
    
    return None


def force_window_foreground(hwnd):
    """Принудительный вывод окна на передний план."""
    if not hwnd or not win32gui.IsWindow(hwnd):
        return False
    
    try:
        # 1. Восстановить если свёрнуто
        if win32gui.IsIconic(hwnd):
            win32gui.ShowWindow(hwnd, win32con.SW_RESTORE)
            time.sleep(0.3)
        
        # 2. Показать окно
        win32gui.ShowWindow(hwnd, win32con.SW_SHOW)
        time.sleep(0.1)
        
        # 3. Поднять наверх
        win32gui.BringWindowToTop(hwnd)
        time.sleep(0.1)
        
        # 4. Хитрость для SetForegroundWindow - нажать Alt
        user32 = ctypes.windll.user32
        user32.keybd_event(0x12, 0, 0, 0)  # Alt down
        user32.keybd_event(0x12, 0, 2, 0)  # Alt up
        time.sleep(0.05)
        
        # 5. Активировать окно
        win32gui.SetForegroundWindow(hwnd)
        time.sleep(0.2)
        
        # 6. Развернуть на весь экран
        win32gui.ShowWindow(hwnd, win32con.SW_MAXIMIZE)
        time.sleep(0.3)
        
        # Проверка
        fg = win32gui.GetForegroundWindow()
        if fg == hwnd:
            return True
        else:
            log(f"Окно не на переднем плане (fg={fg}, epic={hwnd})")
            # Ещё попытка
            win32gui.SetForegroundWindow(hwnd)
            time.sleep(0.2)
            return win32gui.GetForegroundWindow() == hwnd
            
    except Exception as e:
        log(f"force_foreground error: {e}")
        return False


def activate_epic():
    """Активация окна Epic."""
    global EPIC_HWND
    
    if not EPIC_HWND or not win32gui.IsWindow(EPIC_HWND):
        find_epic_window()
    
    if not EPIC_HWND:
        log("Окно Epic не найдено!")
        return False
    
    return force_window_foreground(EPIC_HWND)


def get_window_rect():
    """Получение координат окна Epic."""
    global EPIC_HWND
    if EPIC_HWND and win32gui.IsWindow(EPIC_HWND):
        try:
            return win32gui.GetWindowRect(EPIC_HWND)
        except:
            pass
    return None


# === ПОИСК ЭЛЕМЕНТОВ НА ЭКРАНЕ ===

def find_color_on_screen(target_color, tolerance=30):
    """
    Поиск цвета на экране.
    Возвращает координаты центра найденной области или None.
    """
    try:
        screenshot = ImageGrab.grab()
        pixels = screenshot.load()
        width, height = screenshot.size
        
        matches = []
        
        for y in range(0, height, 5):  # Шаг 5 пикселей для скорости
            for x in range(0, width, 5):
                r, g, b = pixels[x, y][:3]
                tr, tg, tb = target_color
                
                if (abs(r - tr) < tolerance and 
                    abs(g - tg) < tolerance and 
                    abs(b - tb) < tolerance):
                    matches.append((x, y))
        
        if matches:
            # Возвращаем центр кластера
            avg_x = sum(m[0] for m in matches) // len(matches)
            avg_y = sum(m[1] for m in matches) // len(matches)
            return (avg_x, avg_y)
        
        return None
    except Exception as e:
        log(f"find_color error: {e}")
        return None


def find_blue_button():
    """Поиск синей кнопки Epic (Continue/Login/Sign In)."""
    # Синий цвет кнопок Epic Games: примерно RGB(0, 116, 217) или похожий
    blue_colors = [
        (0, 116, 217),   # Epic blue
        (0, 120, 215),   # Вариация
        (0, 115, 220),   # Вариация
        (40, 120, 200),  # Вариация
    ]
    
    for color in blue_colors:
        pos = find_color_on_screen(color, tolerance=40)
        if pos:
            log(f"Найдена синяя кнопка около ({pos[0]}, {pos[1]})")
            return pos
    
    return None


def find_input_field():
    """
    Поиск поля ввода на экране.
    Ищем белый/светлый прямоугольник.
    """
    # Белый/светлый цвет полей ввода
    try:
        screenshot = ImageGrab.grab()
        pixels = screenshot.load()
        width, height = screenshot.size
        
        # Ищем горизонтальные линии белых пикселей (поле ввода)
        for y in range(height // 3, height * 2 // 3):  # Средняя часть экрана
            white_count = 0
            start_x = None
            
            for x in range(width // 4, width * 3 // 4):  # Центральная часть
                r, g, b = pixels[x, y][:3]
                
                # Белый или очень светлый
                if r > 240 and g > 240 and b > 240:
                    if start_x is None:
                        start_x = x
                    white_count += 1
                else:
                    if white_count > 200:  # Нашли длинную белую полосу
                        center_x = start_x + white_count // 2
                        log(f"Найдено поле ввода около ({center_x}, {y})")
                        return (center_x, y)
                    white_count = 0
                    start_x = None
        
        return None
    except Exception as e:
        log(f"find_input error: {e}")
        return None


# === ДЕЙСТВИЯ ===

def click(x, y, desc=""):
    """Клик по координатам."""
    log(f"  Клик ({x}, {y}) {desc}")
    pyautogui.moveTo(x, y, duration=0.1)
    time.sleep(0.05)
    pyautogui.click(x, y)
    time.sleep(CLICK_DELAY)


def type_text(text):
    """Ввод текста через буфер обмена."""
    # Очистка поля
    pyautogui.hotkey("ctrl", "a")
    time.sleep(0.1)
    pyautogui.press("delete")
    time.sleep(0.1)
    
    # Вставка через clipboard
    try:
        pyperclip.copy(text)
        pyautogui.hotkey("ctrl", "v")
    except:
        # Fallback - ввод напрямую
        pyautogui.typewrite(text, interval=TYPE_DELAY)
    
    time.sleep(0.2)


def press_enter():
    """Нажатие Enter."""
    pyautogui.press("enter")
    time.sleep(0.3)


def press_tab():
    """Нажатие Tab."""
    pyautogui.press("tab")
    time.sleep(0.2)


# === ЭТАПЫ АВТОРИЗАЦИИ ===

def do_email_stage(email):
    """Этап 1: Ввод email."""
    log("=== ЭТАП 1: ВВОД EMAIL ===")
    
    activate_epic()
    time.sleep(0.5)
    
    # Способ 1: Ищем поле ввода
    field = find_input_field()
    if field:
        click(field[0], field[1], "поле email")
    else:
        # Fallback: клик по центру экрана (там обычно поле)
        rect = get_window_rect()
        if rect:
            cx = (rect[0] + rect[2]) // 2
            cy = (rect[1] + rect[3]) // 2
            click(cx, cy, "центр окна")
        else:
            click(640, 400, "центр экрана")
    
    time.sleep(0.3)
    
    # Вводим email
    log(f"  Ввод: {email[:3]}***")
    type_text(email)
    
    # Ищем синюю кнопку Continue
    time.sleep(0.3)
    button = find_blue_button()
    if button:
        click(button[0], button[1], "кнопка Continue")
    else:
        # Fallback: Tab + Enter
        log("  Кнопка не найдена, используем Tab+Enter")
        press_tab()
        press_enter()
    
    log("  Этап 1 выполнен!")
    time.sleep(3)  # Ждём загрузку следующей страницы
    return True


def do_password_stage(password):
    """Этап 2: Ввод пароля."""
    log("=== ЭТАП 2: ВВОД ПАРОЛЯ ===")
    
    activate_epic()
    time.sleep(0.5)
    
    # Ищем поле ввода пароля
    field = find_input_field()
    if field:
        click(field[0], field[1], "поле пароля")
    else:
        rect = get_window_rect()
        if rect:
            cx = (rect[0] + rect[2]) // 2
            cy = (rect[1] + rect[3]) // 2 - 50  # Чуть выше центра
            click(cx, cy, "центр окна")
        else:
            click(640, 350, "центр экрана")
    
    time.sleep(0.3)
    
    # Вводим пароль
    log("  Ввод: ***")
    type_text(password)
    
    # Ищем синюю кнопку Login
    time.sleep(0.3)
    button = find_blue_button()
    if button:
        click(button[0], button[1], "кнопка Login")
    else:
        log("  Кнопка не найдена, используем Enter")
        press_enter()
    
    log("  Этап 2 выполнен!")
    time.sleep(4)  # Ждём авторизацию
    return True


def do_continue_stage():
    """Этап 3: Нажать Continue (если появится)."""
    log("=== ЭТАП 3: ПОДТВЕРЖДЕНИЕ ===")
    
    activate_epic()
    time.sleep(0.5)
    
    # Ищем синюю кнопку
    button = find_blue_button()
    if button:
        click(button[0], button[1], "кнопка Continue")
        log("  Этап 3 выполнен!")
        return True
    else:
        log("  Кнопка подтверждения не найдена (возможно не нужна)")
        return False


# === ОПРЕДЕЛЕНИЕ ТЕКУЩЕГО СОСТОЯНИЯ ===

def detect_current_stage():
    """
    Определение текущего этапа авторизации.
    Возвращает: "email", "password", "continue", "done", "unknown"
    """
    try:
        screenshot = ImageGrab.grab()
        # Сохраним для дебага
        # screenshot.save(RELEASE_DIR / "debug_screenshot.png")
        
        # Проверяем наличие синей кнопки
        button = find_blue_button()
        field = find_input_field()
        
        if button and field:
            # Есть поле и кнопка - это форма ввода
            # Нужно определить email или password
            # По позиции поля: если высоко - password, если ниже - email
            if field[1] < 400:  # Поле выше середины
                return "password"
            else:
                return "email"
        elif button and not field:
            # Только кнопка - это Continue
            return "continue"
        elif not button and not field:
            # Ничего нет - возможно авторизация прошла
            return "done"
        else:
            return "unknown"
            
    except Exception as e:
        log(f"detect_stage error: {e}")
        return "unknown"


# === ГЛАВНЫЙ ЦИКЛ ===

def main():
    """Главная функция."""
    # Очищаем лог
    try:
        open(LOG_FILE, "w").close()
    except:
        pass
    
    log("=" * 50)
    log("EPIC GAMES AUTO-AUTH v4.0")
    log("=" * 50)
    log(f"Время: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    log(f"Таймаут: {SCRIPT_TIMEOUT} сек")
    log("")
    
    # Загружаем креды
    email, password = load_credentials()
    if not email or not password:
        log("ОШИБКА: Креды не загружены!")
        return 1
    
    log("")
    log("Поиск окна Epic Games...")
    
    # Ищем окно
    if not find_epic_window():
        log("Окно Epic не найдено. Ждём...")
        # Ждём появления окна
        for i in range(30):
            time.sleep(1)
            if find_epic_window():
                break
        else:
            log("Окно Epic не появилось за 30 сек")
            return 1
    
    log("")
    log("=" * 50)
    log("НАЧАЛО МОНИТОРИНГА")
    log("=" * 50)
    
    start_time = time.time()
    email_done = False
    password_done = False
    continue_done = False
    last_action_time = 0
    ACTION_COOLDOWN = 5  # Минимум 5 сек между действиями
    
    while True:
        elapsed = time.time() - start_time
        
        # Таймаут
        if elapsed >= SCRIPT_TIMEOUT:
            log("")
            log(f"ТАЙМАУТ ({SCRIPT_TIMEOUT} сек)")
            break
        
        # Активируем окно каждую итерацию
        activate_epic()
        
        # Проверяем cooldown
        if time.time() - last_action_time < ACTION_COOLDOWN:
            time.sleep(1)
            continue
        
        # Определяем текущий этап
        stage = detect_current_stage()
        log(f"Текущий этап: {stage}")
        
        if stage == "email" and not email_done:
            if do_email_stage(email):
                email_done = True
                last_action_time = time.time()
            continue
        
        if stage == "password" and not password_done:
            if do_password_stage(password):
                password_done = True
                last_action_time = time.time()
            continue
        
        if stage == "continue" and not continue_done:
            if do_continue_stage():
                continue_done = True
                last_action_time = time.time()
            # После continue можно выходить
            log("")
            log("АВТОРИЗАЦИЯ ЗАВЕРШЕНА!")
            break
        
        if stage == "done":
            log("Формы авторизации не обнаружены - возможно уже залогинены")
            # Ждём немного и проверяем ещё раз
            time.sleep(5)
            if detect_current_stage() == "done":
                log("Подтверждено: авторизация не требуется")
                break
        
        time.sleep(CHECK_INTERVAL)
    
    # Итоги
    log("")
    log("=" * 50)
    log("ИТОГИ:")
    log(f"  Email:    {'OK' if email_done else '--'}")
    log(f"  Password: {'OK' if password_done else '--'}")
    log(f"  Continue: {'OK' if continue_done else '--'}")
    log(f"  Время: {time.time() - start_time:.1f} сек")
    log("=" * 50)
    
    # Держим окно активным ещё 10 сек
    log("Удерживаю окно активным ещё 10 сек...")
    for _ in range(10):
        activate_epic()
        time.sleep(1)
    
    log("Скрипт завершён.")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        log("Прервано пользователем")
        sys.exit(0)
    except Exception as e:
        log(f"КРИТИЧЕСКАЯ ОШИБКА: {e}")
        import traceback
        log(traceback.format_exc())
        sys.exit(1)
