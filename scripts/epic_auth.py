import os
import sys
import time
import json
import shutil
import subprocess
import argparse
from datetime import datetime
from pathlib import Path
from typing import Iterable, Optional

# Third-party libraries
try:
    import cv2
    import mss
    import numpy as np
    import win32api
    import win32con
    import win32gui
    import win32clipboard
    from pywinauto.keyboard import send_keys
except ImportError:
    print("[ERROR] Missing dependencies. Please run: pip install -r requirements.txt")

# ==================================================================================
# CONFIGURATION & PATHS
# ==================================================================================
LOCAL_APP_DATA = os.environ.get('LOCALAPPDATA')
USER_PROFILE = os.environ.get('USERPROFILE')

EPIC_SOURCE_DIR = os.path.join(LOCAL_APP_DATA, r"EpicGamesLauncher\Saved")
BACKUP_BASE_DIR = os.path.join(USER_PROFILE, r"Documents\GTA5rpVirt_EpicBackup")
DEFAULT_EPIC_EXE = r"C:\Program Files (x86)\Epic Games\Launcher\Portal\Binaries\Win32\EpicGamesLauncher.exe"
if not os.path.exists(DEFAULT_EPIC_EXE):
    DEFAULT_EPIC_EXE = r"C:\Program Files\Epic Games\Launcher\Portal\Binaries\Win64\EpicGamesLauncher.exe"

TEMPLATES_DIR = Path(__file__).parent / "templates"
CONFIG_PATH = Path(__file__).parent.parent / "config.txt"
DEBUG_DIR = Path(__file__).parent / "debug"

DEFAULT_WINDOW_CLASS = "UnrealWindow"
GUARD_INTERVAL = 300  # 5 minutes

# ==================================================================================
# UTILS
# ==================================================================================

def log(message):
    timestamp = datetime.now().strftime("%H:%M:%S")
    print(f"[{timestamp}] {message}", flush=True)

def is_process_running(process_name):
    try:
        cmd = f'tasklist /FI "IMAGENAME eq {process_name}" /FO CSV /NH'
        output = subprocess.check_output(cmd, shell=True).decode('cp866', errors='ignore')
        return process_name.lower() in output.lower()
    except Exception:
        return False

def kill_process(process_name):
    log(f"Killing {process_name}...")
    os.system(f"taskkill /f /im {process_name} >nul 2>&1")

def parse_kv_config(path: Path) -> dict:
    data = {}
    if not path.exists():
        return data
    try:
        raw = path.read_text(encoding="utf-8", errors="ignore").splitlines()
        for line in raw:
            s = line.strip()
            if not s or s.startswith("#"):
                continue
            if s.endswith(";"):
                s = s[:-1]
            if "=" not in s:
                continue
            k, v = s.split("=", 1)
            data[k.strip()] = v.strip()
    except Exception as e:
        log(f"Config parse error: {e}")
    return data

# ==================================================================================
# VISION & WINDOW CONTROL
# ==================================================================================

class Match:
    def __init__(self, score, x, y, w, h, scale):
        self.score = score
        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.scale = scale

def get_window_rect(hwnd):
    left, top, right, bottom = win32gui.GetWindowRect(hwnd)
    return left, top, right, bottom

def capture_window_bgr(hwnd):
    left, top, right, bottom = get_window_rect(hwnd)
    width = right - left
    height = bottom - top
    with mss.mss() as sct:
        monitor = {"left": left, "top": top, "width": width, "height": height}
        shot = sct.grab(monitor)
        img = np.array(shot)
        return img[:, :, :3] # BGR

def match_template_multiscale(screen_gray, template_gray, scales):
    best = None
    for scale in scales:
        if scale <= 0: continue
        tw = int(template_gray.shape[1] * scale)
        th = int(template_gray.shape[0] * scale)
        if tw < 10 or th < 10 or tw > screen_gray.shape[1] or th > screen_gray.shape[0]:
            continue
        
        t = cv2.resize(template_gray, (tw, th), interpolation=cv2.INTER_AREA) if scale != 1.0 else template_gray
        res = cv2.matchTemplate(screen_gray, t, cv2.TM_CCOEFF_NORMED)
        _, max_val, _, max_loc = cv2.minMaxLoc(res)
        
        if best is None or max_val > best.score:
            best = Match(float(max_val), int(max_loc[0]), int(max_loc[1]), int(t.shape[1]), int(t.shape[0]), float(scale))
    return best

def wait_template(hwnd, template_name, threshold=0.85, timeout=30, scales=[0.9, 1.0, 1.1]):
    template_path = TEMPLATES_DIR / template_name
    if not template_path.exists():
        log(f"Template not found: {template_name}")
        return None
    
    template_gray = cv2.imread(str(template_path), cv2.IMREAD_GRAYSCALE)
    deadline = time.time() + timeout
    
    while time.time() < deadline:
        try:
            bgr = capture_window_bgr(hwnd)
            gray = cv2.cvtColor(bgr, cv2.COLOR_BGR2GRAY)
            match = match_template_multiscale(gray, template_gray, scales)
            
            if match and match.score >= threshold:
                return match
        except Exception as e:
            log(f"Vision error: {e}")
        
        time.sleep(0.5)
    return None

def bring_to_front(hwnd):
    if win32gui.IsIconic(hwnd):
        win32gui.ShowWindow(hwnd, win32con.SW_RESTORE)
    win32gui.SetForegroundWindow(hwnd)

def click_at(hwnd, x, y):
    left, top, _, _ = get_window_rect(hwnd)
    abs_x, abs_y = left + x, top + y
    win32api.SetCursorPos((abs_x, abs_y))
    time.sleep(0.1)
    win32api.mouse_event(win32con.MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0)
    time.sleep(0.1)
    win32api.mouse_event(win32con.MOUSEEVENTF_LEFTUP, 0, 0, 0, 0)

# ==================================================================================
# CORE LOGIC
# ==================================================================================

def perform_backup(profile_name="default"):
    log(f"Backing up session [{profile_name}]...")
    dst_dir = os.path.join(BACKUP_BASE_DIR, profile_name)
    os.makedirs(dst_dir, exist_ok=True)
    
    try:
        subprocess.run(['robocopy', os.path.join(EPIC_SOURCE_DIR, "Config"), os.path.join(dst_dir, "Config"), '/E', '/MT', '/R:0', '/W:0', '/NFL', '/NDL', '/NJH', '/NJS'], capture_output=True)
        subprocess.run(['robocopy', os.path.join(EPIC_SOURCE_DIR, "webcache"), os.path.join(dst_dir, "webcache"), '/E', '/MT', '/R:0', '/W:0', '/NFL', '/NDL', '/NJH', '/NJS'], capture_output=True)
        log("Backup completed.")
    except Exception as e:
        log(f"Backup error: {e}")

def perform_restore(profile_name="default"):
    src_dir = os.path.join(BACKUP_BASE_DIR, profile_name)
    if not os.path.exists(src_dir):
        log(f"No backup found for {profile_name}. Skipping restore.")
        return False
    
    log(f"Restoring session [{profile_name}]...")
    os.makedirs(EPIC_SOURCE_DIR, exist_ok=True)
    try:
        subprocess.run(['robocopy', src_dir, EPIC_SOURCE_DIR, '/E', '/MT', '/R:1', '/W:1', '/NFL', '/NDL', '/NJH', '/NJS'], capture_output=True)
        log("Restore completed.")
        return True
    except Exception as e:
        log(f"Restore error: {e}")
        return False

def set_clipboard(text):
    """Safely set clipboard text with retries"""
    for i in range(5):
        try:
            win32clipboard.OpenClipboard()
            win32clipboard.EmptyClipboard()
            win32clipboard.SetClipboardData(win32con.CF_UNICODETEXT, text)
            win32clipboard.CloseClipboard()
            return True
        except Exception:
            time.sleep(0.1)
    log("Failed to set clipboard")
    return False

def switch_to_english():
    """Force switch keyboard layout to English (US)"""
    try:
        # Load English (US) layout
        hwnd = win32gui.GetForegroundWindow()
        win32api.PostMessage(hwnd, win32con.WM_INPUTLANGCHANGEREQUEST, 0, 0x04090409)
        time.sleep(0.2)
    except Exception as e:
        log(f"Layout switch error: {e}")

def auto_login(hwnd, login, password):
    log("Starting Vision-based Auto-Login...")
    bring_to_front(hwnd)
    switch_to_english()
    
    match = wait_template(hwnd, "login_field.png", timeout=20)
    if not match:
        log("Login field not found. Maybe already logged in?")
        return False
    
    click_at(hwnd, match.x + match.w // 2, match.y + match.h // 2)
    time.sleep(0.5)
    send_keys("^a{DELETE}", pause=0.1)
    time.sleep(0.2)
    
    set_clipboard(login)
    send_keys("^v", pause=0.1)
    time.sleep(0.5)
    send_keys("{ENTER}")
    
    match = wait_template(hwnd, "password_field.png", timeout=15)
    if not match:
        log("Password field not found after login enter.")
        return False
    
    click_at(hwnd, match.x + match.w // 2, match.y + match.h // 2)
    time.sleep(0.5)
    send_keys("^a{DELETE}", pause=0.1)
    
    set_clipboard(password)
    send_keys("^v", pause=0.1)
    time.sleep(0.5)
    
    # Final sequence: Enter -> Wait -> TAB -> Enter (to ensure button click)
    send_keys("{ENTER}")
    time.sleep(1.0)
    send_keys("{TAB}")
    time.sleep(0.2)
    send_keys("{ENTER}")
    
    log("Waiting for success confirmation...")
    if wait_template(hwnd, "success.png", timeout=30):
        log("Login SUCCESSFUL!")
        return True
    else:
        log("Login verification TIMEOUT.")
        return False

# ==================================================================================
# MODES
# ==================================================================================

def guard_mode(profile_name="default"):
    log(f"Guard mode active for [{profile_name}].")
    time.sleep(10)
    while True:
        if is_process_running("EpicGamesLauncher.exe") or is_process_running("GTA5.exe"):
            perform_backup(profile_name)
        else:
            log("Processes closed. Guard exiting.")
            break
        time.sleep(GUARD_INTERVAL)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--guard", action="store_true")
    parser.add_argument("--backup", action="store_true")
    parser.add_argument("--backup-only", action="store_true", help="Alias for --backup")
    parser.add_argument("--restore", action="store_true")
    parser.add_argument("--profile", default=None)
    args = parser.parse_args()

    # Config loading
    config = parse_kv_config(CONFIG_PATH)
    
    # Determine Profile
    if args.profile is None:
        # Try to get from config "Active Character"
        active_char = config.get("Active Character", "1")
        args.profile = f"profile_{active_char}"
    
    log(f"--- EPIC AUTH MODERN v9.2 START [Profile: {args.profile}] ---")

    if args.guard:
        guard_mode(args.profile)
        return

    if args.backup or args.backup_only:
        perform_backup(args.profile)
        return

    login = config.get("EpicLogin")
    password = config.get("EpicPassword")

    if is_process_running("EpicGamesLauncher.exe"):
        log("Epic is already running. Updating backup.")
        perform_backup(args.profile)
    else:
        perform_restore(args.profile)
        log("Launching Epic...")
        subprocess.Popen([DEFAULT_EPIC_EXE, "-silent"], close_fds=True)
        
        hwnd = 0
        for _ in range(60):
            hwnd = win32gui.FindWindow(DEFAULT_WINDOW_CLASS, None)
            if hwnd and win32gui.IsWindowVisible(hwnd):
                break
            time.sleep(1)
        
        if hwnd:
            log("Epic window found. Checking if login is needed...")
            if wait_template(hwnd, "success.png", timeout=10):
                log("Session restored successfully via files.")
            elif login and password:
                log("Session expired. Attempting auto-login...")
                if auto_login(hwnd, login, password):
                    perform_backup(args.profile)
            else:
                log("Login needed but credentials missing in config.txt")
        else:
            log("Failed to find Epic window after launch.")

    subprocess.Popen([sys.executable, __file__, "--guard", "--profile", args.profile], creationflags=0x08000000)
    log("Guard spawned. Done.")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        log(f"CRITICAL ERROR: {e}")
        time.sleep(10)
