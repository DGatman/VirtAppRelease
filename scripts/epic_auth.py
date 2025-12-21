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
import msvcrt

REQUIRED_MODULES = [
    "cv2",
    "mss",
    "numpy",
    "win32api",
    "win32con",
    "win32gui",
    "win32process",
    "win32clipboard",
    "pywinauto.keyboard",
]


def ensure_dependencies():
    """Auto-install required packages if missing (headless fleet safe)."""
    missing = []
    for mod in REQUIRED_MODULES:
        try:
            __import__(mod)
        except Exception:
            missing.append(mod)

    if not missing:
        return

    root = Path(__file__).parent.parent
    req = root / "requirements.txt"
    print(f"[INFO] Installing missing deps: {', '.join(missing)}", flush=True)
    cmd = [sys.executable, "-m", "pip", "install", "-r", str(req)]
    try:
        subprocess.check_call(cmd)
    except Exception as e:
        print(f"[ERROR] Failed to install dependencies: {e}", flush=True)
        sys.exit(1)


# Third-party libraries (loaded after bootstrap)
ensure_dependencies()
import cv2
import mss
import numpy as np
import win32api
import win32con
import win32gui
import win32process
import win32clipboard
from pywinauto.keyboard import send_keys

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

LOG_PATH = Path(__file__).parent.parent / "epic_auth.log"
GUARD_LOCK_PATH = Path(__file__).parent.parent / "epic_auth_guard.lock"

DEFAULT_WINDOW_CLASS = "UnrealWindow"
GUARD_INTERVAL = 300  # 5 minutes

# ==================================================================================
# UTILS
# ==================================================================================

def log(message):
    timestamp = datetime.now().strftime("%H:%M:%S")
    line = f"[{timestamp}] {message}"
    print(line, flush=True)
    try:
        LOG_PATH.parent.mkdir(parents=True, exist_ok=True)
        with LOG_PATH.open("a", encoding="utf-8", errors="ignore") as f:
            f.write(line + "\n")
    except Exception:
        pass


def _debug_dump_window(hwnd: int, tag: str):
    """Write a screenshot of the target window to scripts/debug for diagnosis."""
    try:
        DEBUG_DIR.mkdir(parents=True, exist_ok=True)
        bgr = capture_window_bgr(hwnd)
        ts = datetime.now().strftime("%Y%m%d_%H%M%S")
        out = DEBUG_DIR / f"epic_{tag}_{ts}.png"
        cv2.imwrite(str(out), bgr)
        log(f"Debug dump saved: {out}")
    except Exception as e:
        log(f"Debug dump failed: {e}")

def is_process_running(process_name):
    try:
        cmd = f'tasklist /FI "IMAGENAME eq {process_name}" /FO CSV /NH'
        output = subprocess.check_output(cmd, shell=True).decode('cp866', errors='ignore')
        return process_name.lower() in output.lower()
    except Exception:
        return False


def get_pids_by_image(process_name: str) -> list[int]:
    """Return list of PIDs for a process name using tasklist (no extra deps)."""
    try:
        cmd = f'tasklist /FI "IMAGENAME eq {process_name}" /FO CSV /NH'
        out = subprocess.check_output(cmd, shell=True).decode("cp866", errors="ignore").strip()
        if not out or "INFO:" in out:
            return []

        # CSV line: "Image Name","PID","Session Name","Session#","Mem Usage"
        import csv
        rows = list(csv.reader(out.splitlines()))
        pids: list[int] = []
        for row in rows:
            if len(row) >= 2 and row[0].lower() == process_name.lower():
                try:
                    pids.append(int(row[1]))
                except Exception:
                    pass
        return pids
    except Exception:
        return []


def _is_any_process_running(names: Iterable[str]) -> bool:
    for n in names:
        if is_process_running(n):
            return True
    return False


def spawn_guard(profile: str):
    """Spawn background guard process (safe to call repeatedly).

    Guard uses a lock file to ensure only one instance runs.
    """
    try:
        subprocess.Popen(
            [sys.executable, __file__, "--guard", "--profile", profile],
            creationflags=0x08000000,  # CREATE_NO_WINDOW
            close_fds=True,
        )
        log("Guard spawned.")
    except Exception as e:
        log(f"Failed to spawn guard: {e}")


def _enum_windows():
    windows: list[int] = []

    def enum_handler(h, _):
        if not win32gui.IsWindow(h):
            return
        windows.append(h)

    win32gui.EnumWindows(enum_handler, None)
    return windows


def _window_info(hwnd: int) -> str:
    try:
        title = win32gui.GetWindowText(hwnd) or ""
    except Exception:
        title = ""
    try:
        cls = win32gui.GetClassName(hwnd) or ""
    except Exception:
        cls = ""
    try:
        _, pid = win32process.GetWindowThreadProcessId(hwnd)
    except Exception:
        pid = 0
    try:
        l, t, r, b = win32gui.GetWindowRect(hwnd)
        w, h = max(0, r - l), max(0, b - t)
    except Exception:
        w, h = 0, 0
    vis = False
    try:
        vis = bool(win32gui.IsWindowVisible(hwnd))
    except Exception:
        pass
    return f"hwnd=0x{hwnd:08X} pid={pid} vis={vis} {w}x{h} class='{cls}' title='{title}'"

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


def _parse_bool(value: Optional[str], default: bool) -> bool:
    if value is None:
        return default
    s = str(value).strip().lower()
    if s == "":
        return default
    if s in {"1", "true", "yes", "y", "on", "enable", "enabled"}:
        return True
    if s in {"0", "false", "no", "n", "off", "disable", "disabled"}:
        return False
    return default


def _parse_float(value: Optional[str], default: float) -> float:
    if value is None:
        return default
    try:
        return float(str(value).strip().replace(",", "."))
    except Exception:
        return default


def get_epic_auth_mode(cfg: dict) -> str:
    """Return one of: off | detect | blind | auto.

    Config keys supported:
    - EpicAuth=ON/OFF
    - EpicAuthEnabled=1/0 (legacy; treated same as EpicAuth)
    - EpicAuthMode=auto|detect|blind|off
    - EpicDetect=ON/OFF (legacy toggle)
    - EpicBlind=ON/OFF (legacy toggle)
    """
    epic_auth_flag = cfg.get("EpicAuth")
    if epic_auth_flag is None:
        epic_auth_flag = cfg.get("EpicAuthEnabled")

    if not _parse_bool(epic_auth_flag, True):
        return "off"

    mode = str(cfg.get("EpicAuthMode", "auto")).strip().lower()
    if mode in {"off", "0", "false"}:
        return "off"
    if mode in {"detect", "template", "templates"}:
        return "detect"
    if mode in {"blind", "coords", "coordinate", "coordinates"}:
        return "blind"

    # Auto mode with optional toggles
    detect_on = _parse_bool(cfg.get("EpicDetect"), True)
    blind_on = _parse_bool(cfg.get("EpicBlind"), True)
    if detect_on and blind_on:
        return "auto"
    if detect_on and not blind_on:
        return "detect"
    if (not detect_on) and blind_on:
        return "blind"
    return "off"

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
    """Capture only the Epic window. Try PrintWindow (works even if partially covered), fallback to mss on the hwnd rect."""
    left, top, right, bottom = get_window_rect(hwnd)
    width = right - left
    height = bottom - top

    # Try PrintWindow for occluded windows
    try:
        import win32ui
        import win32print

        hwndDC = win32gui.GetWindowDC(hwnd)
        mfcDC = win32ui.CreateDCFromHandle(hwndDC)
        saveDC = mfcDC.CreateCompatibleDC()
        saveBitMap = win32ui.CreateBitmap()
        saveBitMap.CreateCompatibleBitmap(mfcDC, width, height)
        saveDC.SelectObject(saveBitMap)

        result = win32gui.PrintWindow(hwnd, saveDC.GetSafeHdc(), 0)
        if result == 1:
            bmpinfo = saveBitMap.GetInfo()
            bmpstr = saveBitMap.GetBitmapBits(True)
            img = np.frombuffer(bmpstr, dtype='uint8')
            img.shape = (bmpinfo['bmHeight'], bmpinfo['bmWidth'], 4)
            bgr = img[:, :, :3].copy()
            win32gui.DeleteObject(saveBitMap.GetHandle())
            saveDC.DeleteDC()
            mfcDC.DeleteDC()
            win32gui.ReleaseDC(hwnd, hwndDC)
            return bgr
    except Exception:
        # Fallback to mss below
        pass

    with mss.mss() as sct:
        monitor = {"left": left, "top": top, "width": width, "height": height}
        shot = sct.grab(monitor)
        img = np.array(shot)
        return img[:, :, :3]  # BGR

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

def wait_template(
    hwnd,
    template_name,
    threshold=0.85,
    timeout=30,
    scales=[0.9, 1.0, 1.1],
    dump_on_fail: bool = True,
):
    template_path = TEMPLATES_DIR / template_name
    if not template_path.exists():
        log(f"Template not found: {template_name}")
        return None
    
    template_gray = cv2.imread(str(template_path), cv2.IMREAD_GRAYSCALE)
    deadline = time.time() + timeout
    
    best_seen: Optional[Match] = None
    while time.time() < deadline:
        try:
            bgr = capture_window_bgr(hwnd)
            gray = cv2.cvtColor(bgr, cv2.COLOR_BGR2GRAY)
            match = match_template_multiscale(gray, template_gray, scales)
            
            if match and match.score >= threshold:
                return match

            if match and (best_seen is None or match.score > best_seen.score):
                best_seen = match
        except Exception as e:
            log(f"Vision error: {e}")
        
        time.sleep(0.5)

    if best_seen is not None:
        log(
            f"Template '{template_name}' not found. Best score={best_seen.score:.3f} "
            f"scale={best_seen.scale:.2f} at ({best_seen.x},{best_seen.y})"
        )
    else:
        log(f"Template '{template_name}' not found. No matches computed.")
    if dump_on_fail:
        _debug_dump_window(hwnd, f"no_{Path(template_name).stem}")
    return None


def probe_template(hwnd, template_name, threshold=0.85, timeout=2, scales=[0.9, 1.0, 1.1]):
    """Short probe without dumping screenshots on failure."""
    return wait_template(
        hwnd,
        template_name,
        threshold=threshold,
        timeout=timeout,
        scales=scales,
        dump_on_fail=False,
    )


def find_epic_window():
    """Find Epic window.

    Priority:
    1) Match windows by PID(s) of EpicGamesLauncher.exe (most reliable)
    2) Match by known class
    3) Match by title substring
    """
    pids = set(get_pids_by_image("EpicGamesLauncher.exe"))

    candidates: list[int] = []
    for h in _enum_windows():
        try:
            _, pid = win32process.GetWindowThreadProcessId(h)
        except Exception:
            continue
        if pids and pid in pids:
            candidates.append(h)

    def score_window(h: int) -> int:
        try:
            l, t, r, b = win32gui.GetWindowRect(h)
            area = max(0, r - l) * max(0, b - t)
        except Exception:
            area = 0
        try:
            title = (win32gui.GetWindowText(h) or "").strip()
        except Exception:
            title = ""
        try:
            vis = bool(win32gui.IsWindowVisible(h))
        except Exception:
            vis = False
        try:
            cls = (win32gui.GetClassName(h) or "").strip()
        except Exception:
            cls = ""
        # Prefer visible, titled, large windows
        class_bonus = 200000 if cls == "UnrealWindow" else 0
        return (1000000 if vis else 0) + class_bonus + (100000 if title else 0) + area

    if candidates:
        # Prefer visible/real windows; avoid hidden Chromium widgets when possible.
        def big_enough(h: int) -> bool:
            try:
                l, t, r, b = win32gui.GetWindowRect(h)
                return (r - l) >= 500 and (b - t) >= 350
            except Exception:
                return False

        vis_candidates = []
        for h in candidates:
            try:
                if win32gui.IsWindowVisible(h) and big_enough(h):
                    vis_candidates.append(h)
            except Exception:
                pass

        best_pool = vis_candidates if vis_candidates else candidates
        best = max(best_pool, key=score_window)
        log(f"Epic window (by PID) selected: {_window_info(best)}")
        return best

    hwnd = win32gui.FindWindow(DEFAULT_WINDOW_CLASS, None)
    if hwnd and win32gui.IsWindow(hwnd):
        log(f"Epic window (by class) selected: {_window_info(hwnd)}")
        return hwnd

    target_titles = ["epic games launcher", "epic games"]
    title_candidates: list[int] = []
    for h in _enum_windows():
        try:
            if not win32gui.IsWindowVisible(h):
                continue
            title = win32gui.GetWindowText(h) or ""
        except Exception:
            continue
        low = title.lower()
        if any(t in low for t in target_titles):
            title_candidates.append(h)

    if title_candidates:
        best = max(title_candidates, key=score_window)
        log(f"Epic window (by title) selected: {_window_info(best)}")
        return best

    # As a last resort, print some diagnostics
    try:
        log("Epic window not found. Top windows snapshot:")
        for h in _enum_windows()[:20]:
            log("  " + _window_info(h))
    except Exception:
        pass
    return None


def get_foreground_window() -> Optional[int]:
    try:
        h = win32gui.GetForegroundWindow()
        return int(h) if h else None
    except Exception:
        return None


def try_restore_foreground(prev_hwnd: Optional[int]):
    if not prev_hwnd:
        return
    try:
        if win32gui.IsWindow(prev_hwnd) and win32gui.IsWindowVisible(prev_hwnd):
            win32gui.SetForegroundWindow(prev_hwnd)
    except Exception:
        pass

def bring_to_front(hwnd):
    """Bring Epic to foreground briefly.

    IMPORTANT: Do not call this continuously; only when click/typing is required.
    Returns previous foreground window (best-effort).
    """
    prev = get_foreground_window()
    try:
        if win32gui.IsIconic(hwnd):
            win32gui.ShowWindow(hwnd, win32con.SW_RESTORE)

        win32gui.ShowWindow(hwnd, win32con.SW_SHOW)
        win32gui.SetWindowPos(
            hwnd,
            win32con.HWND_TOPMOST,
            0,
            0,
            0,
            0,
            win32con.SWP_NOMOVE | win32con.SWP_NOSIZE | win32con.SWP_SHOWWINDOW,
        )
        time.sleep(0.15)
        win32gui.SetForegroundWindow(hwnd)
        time.sleep(0.15)
    except Exception:
        pass
    finally:
        try:
            win32gui.SetWindowPos(
                hwnd,
                win32con.HWND_NOTOPMOST,
                0,
                0,
                0,
                0,
                win32con.SWP_NOMOVE | win32con.SWP_NOSIZE | win32con.SWP_SHOWWINDOW,
            )
        except Exception:
            pass
    return prev


def focus_epic_for_action(hwnd: int, action):
    prev = bring_to_front(hwnd)
    try:
        return action()
    finally:
        try_restore_foreground(prev)


def _open_epic_via_uri(uri: str = "com.epicgames.launcher://store") -> bool:
    """Ask Windows to open Epic Launcher via URI.

    Useful when Epic is running but its main window is hidden in tray.
    """
    try:
        subprocess.Popen(
            ["cmd", "/c", "start", "", uri],
            creationflags=0x08000000,  # CREATE_NO_WINDOW
            close_fds=True,
        )
        return True
    except Exception:
        return False


def ensure_epic_window_visible(hwnd: Optional[int], cfg: dict, *, allow_uri: bool = True) -> Optional[int]:
    """Best-effort: make sure Epic window is shown (not just tray).

    - If hwnd exists but is hidden/minimized: restore/show it without keeping focus.
    - If hwnd is None: optionally trigger Epic via URI and re-find.
    """
    force_show = _parse_bool(cfg.get("EpicForceShowWindow"), True)
    if not force_show:
        return hwnd

    def _show_no_focus(h: int):
        try:
            if win32gui.IsIconic(h):
                win32gui.ShowWindow(h, win32con.SW_RESTORE)
            win32gui.ShowWindow(h, win32con.SW_SHOW)
            # Nudge window so it becomes visible without forcing foreground.
            win32gui.SetWindowPos(
                h,
                win32con.HWND_NOTOPMOST,
                0,
                0,
                0,
                0,
                win32con.SWP_NOMOVE | win32con.SWP_NOSIZE | win32con.SWP_SHOWWINDOW,
            )
        except Exception:
            pass

    if hwnd and win32gui.IsWindow(hwnd):
        try:
            if not win32gui.IsWindowVisible(hwnd) or win32gui.IsIconic(hwnd):
                log("Epic window is hidden/minimized; restoring (no focus)...")
                _show_no_focus(hwnd)
                time.sleep(0.5)
        except Exception:
            pass
        return hwnd

    if allow_uri and _parse_bool(cfg.get("EpicForceOpenUri"), True):
        if _open_epic_via_uri(str(cfg.get("EpicOpenUri", "com.epicgames.launcher://store"))):
            time.sleep(1.0)
            h2 = find_epic_window()
            if h2:
                _show_no_focus(h2)
                return h2

    return hwnd


def _coerce_xy(value: float, size: int) -> int:
    """If value <= 1 treat as ratio, else as pixels."""
    if value <= 1.0:
        return int(max(0, min(size - 1, round(size * value))))
    return int(max(0, min(size - 1, round(value))))


def blind_login(hwnd: int, login: str, password: str, cfg: dict) -> bool:
    """Best-effort login without templates (DPI/template mismatch fallback).

    Uses configurable coordinates (ratio or px) and a TAB workflow.
    Defaults are approximate and may need per-fleet calibration.
    """
    try:
        left, top, right, bottom = get_window_rect(hwnd)
        width = max(1, right - left)
        height = max(1, bottom - top)
    except Exception:
        width, height = 1000, 600

    email_x = _parse_float(cfg.get("EpicBlindEmailX"), 0.50)
    email_y = _parse_float(cfg.get("EpicBlindEmailY"), 0.55)
    pass_x = _parse_float(cfg.get("EpicBlindPasswordX"), 0.50)
    pass_y = _parse_float(cfg.get("EpicBlindPasswordY"), 0.63)
    use_tab = _parse_bool(cfg.get("EpicBlindUseTab"), True)

    x_email = _coerce_xy(email_x, width)
    y_email = _coerce_xy(email_y, height)
    x_pass = _coerce_xy(pass_x, width)
    y_pass = _coerce_xy(pass_y, height)

    log(f"Blind login: clicking email @ ({x_email},{y_email}), password @ ({x_pass},{y_pass}), tab={use_tab}")

    def _do():
        switch_to_english()

        # Email
        click_at(hwnd, x_email, y_email)
        time.sleep(0.25)
        send_keys("^a{DELETE}", pause=0.05)
        time.sleep(0.1)
        set_clipboard(login)
        send_keys("^v", pause=0.05)
        time.sleep(0.2)

        # Password
        if use_tab:
            send_keys("{TAB}")
            time.sleep(0.15)
        else:
            click_at(hwnd, x_pass, y_pass)
            time.sleep(0.15)

        send_keys("^a{DELETE}", pause=0.05)
        time.sleep(0.1)
        set_clipboard(password)
        send_keys("^v", pause=0.05)
        time.sleep(0.2)

        send_keys("{ENTER}")

    focus_epic_for_action(hwnd, _do)

    # Optional best-effort confirmation
    time.sleep(2.0)
    if probe_template(hwnd, "success.png", threshold=0.75, timeout=2):
        log("Blind login: success template detected.")
        return True

    log("Blind login: submitted (success not confirmed by template).")
    return False

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

    # IMPORTANT: Do NOT keep Epic window foreground.
    # We do template matching using PrintWindow/mss capture without stealing focus.
    match = wait_template(hwnd, "login_field.png", timeout=20)
    if not match:
        log("Login field not found. Maybe already logged in?")
        return False

    def enter_login():
        switch_to_english()
        click_at(hwnd, match.x + match.w // 2, match.y + match.h // 2)
        time.sleep(0.3)
        send_keys("^a{DELETE}", pause=0.1)
        time.sleep(0.15)
        set_clipboard(login)
        send_keys("^v", pause=0.1)
        time.sleep(0.25)
        send_keys("{ENTER}")

    focus_epic_for_action(hwnd, enter_login)

    match = wait_template(hwnd, "password_field.png", timeout=15)
    if not match:
        log("Password field not found after login enter.")
        return False

    def enter_password_and_submit():
        switch_to_english()
        click_at(hwnd, match.x + match.w // 2, match.y + match.h // 2)
        time.sleep(0.3)
        send_keys("^a{DELETE}", pause=0.1)
        set_clipboard(password)
        send_keys("^v", pause=0.1)
        time.sleep(0.25)
        # Final sequence: Enter -> Wait -> TAB -> Enter (to ensure button click)
        send_keys("{ENTER}")
        time.sleep(1.0)
        send_keys("{TAB}")
        time.sleep(0.2)
        send_keys("{ENTER}")

    focus_epic_for_action(hwnd, enter_password_and_submit)

    log("Waiting for success confirmation...")
    if wait_template(hwnd, "success.png", timeout=30):
        log("Login SUCCESSFUL!")
        return True

    log("Login verification TIMEOUT.")
    return False

# ==================================================================================
# MODES
# ==================================================================================

def guard_mode(profile_name="default"):
    """Background guard.

    Every GUARD_INTERVAL:
    - If Epic/GTA is running: backup session.
    - If Epic is running and we are NOT logged in: try auto-login (if creds exist).

    This makes it useful even when the app starts epic_auth in --guard mode only.
    """
    # Single-instance guard lock
    try:
        GUARD_LOCK_PATH.parent.mkdir(parents=True, exist_ok=True)
        lock_f = GUARD_LOCK_PATH.open("a+")
        try:
            msvcrt.locking(lock_f.fileno(), msvcrt.LK_NBLCK, 1)
        except OSError:
            log("Guard already running (lock held). Exiting this guard instance.")
            return
    except Exception as e:
        log(f"Guard lock init failed (continuing without lock): {e}")
        lock_f = None

    log(f"Guard mode active for [{profile_name}].")
    time.sleep(5)

    poll_interval = 10  # seconds (how often we check Epic)
    backup_interval = GUARD_INTERVAL  # keep existing 5 min default
    login_attempt_cooldown = 30  # seconds (avoid spamming)
    blind_attempt_cooldown = 180  # seconds (blind mode is intrusive; keep it rarer)
    config_reload_interval = 60

    last_backup = 0.0
    last_login_attempt = 0.0
    last_config_reload = 0.0
    last_unknown_dump = 0.0

    cfg: dict = {}
    mode = "auto"
    login = None
    password = None

    try:
        while True:
            now = time.time()
            epic_alive = is_process_running("EpicGamesLauncher.exe")
            gta_alive = _is_any_process_running(["GTA5.exe", "ragemp_v.exe"])  # support both

            if now - last_config_reload >= config_reload_interval:
                cfg = parse_kv_config(CONFIG_PATH)
                login = cfg.get("EpicLogin")
                password = cfg.get("EpicPassword")
                mode = get_epic_auth_mode(cfg)
                last_config_reload = now

            if epic_alive:
                hwnd = find_epic_window()
                hwnd = ensure_epic_window_visible(hwnd, cfg)
                if hwnd:
                    # If we accidentally grabbed a hidden helper window, wait briefly for a visible one.
                    try:
                        if not win32gui.IsWindowVisible(hwnd):
                            for _ in range(10):
                                time.sleep(0.3)
                                h2 = find_epic_window()
                                if h2 and win32gui.IsWindowVisible(h2):
                                    hwnd = h2
                                    break
                    except Exception:
                        pass

                    success = probe_template(hwnd, "success.png", threshold=0.90, timeout=2)
                    if success:
                        if now - last_backup >= backup_interval:
                            perform_backup(profile_name)
                            last_backup = now
                    else:
                        if mode == "off":
                            # Do nothing; keep backing up occasionally.
                            pass
                        elif mode == "blind":
                            if login and password and (now - last_login_attempt >= blind_attempt_cooldown):
                                log("Guard: blind login attempt...")
                                last_login_attempt = now
                                blind_login(hwnd, login, password, cfg)
                        else:
                            # detect/auto path
                            login_field = probe_template(hwnd, "login_field.png", threshold=0.80, timeout=2)
                            password_field = probe_template(hwnd, "password_field.png", threshold=0.80, timeout=2)

                            if (login_field or password_field):
                                if login and password and (now - last_login_attempt >= login_attempt_cooldown):
                                    log("Guard: Epic not logged in. Attempting auto-login...")
                                    last_login_attempt = now
                                    auto_login(hwnd, login, password)
                            else:
                                # Unknown UI (most often DPI/template mismatch)
                                if mode == "auto" and login and password and (now - last_login_attempt >= blind_attempt_cooldown):
                                    log("Guard: templates mismatch; falling back to blind login...")
                                    last_login_attempt = now
                                    blind_login(hwnd, login, password, cfg)
                                elif now - last_unknown_dump >= backup_interval:
                                    log("Guard: Epic UI state unknown (no success/login templates). Saving debug screenshot.")
                                    _debug_dump_window(hwnd, "guard_unknown_ui")
                                    last_unknown_dump = now
                else:
                    # Epic process running but no window
                    if now - last_unknown_dump >= backup_interval:
                        log("Guard: Epic process running but window not found.")
                        last_unknown_dump = now

            # If game is running but Epic isn't, still keep looping.
            if gta_alive and (time.time() - last_backup >= backup_interval) and epic_alive:
                perform_backup(profile_name)
                last_backup = time.time()

            time.sleep(poll_interval)
    finally:
        try:
            if lock_f is not None:
                try:
                    msvcrt.locking(lock_f.fileno(), msvcrt.LK_UNLCK, 1)
                except Exception:
                    pass
                lock_f.close()
        except Exception:
            pass

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
    
    log(f"--- EPIC AUTH MODERN v9.4 START [Profile: {args.profile}] ---")
    log(f"Python: {sys.executable}")
    log(f"Args: {sys.argv}")

    if args.guard:
        guard_mode(args.profile)
        return

    if args.backup or args.backup_only:
        perform_backup(args.profile)
        return

    mode = get_epic_auth_mode(config)
    login = config.get("EpicLogin")
    password = config.get("EpicPassword")
    log(f"Mode: {mode} (EpicDetect/EpicBlind/EpicAuthMode)")

    hwnd = None
    exit_code = 0

    try:
        if is_process_running("EpicGamesLauncher.exe"):
            log("Epic is already running. Checking login state...")
            hwnd = find_epic_window()
        else:
            perform_restore(args.profile)
            log("Launching Epic...")
            subprocess.Popen([DEFAULT_EPIC_EXE, "-silent"], close_fds=True)
            for _ in range(60):
                hwnd = find_epic_window()
                if hwnd and win32gui.IsWindowVisible(hwnd):
                    break
                time.sleep(1)

        # If Epic starts to tray / hidden, try to force-show the window.
        hwnd = ensure_epic_window_visible(hwnd, config)

        # If we got a hidden helper window, wait briefly for a visible one.
        try:
            if hwnd and (not win32gui.IsWindowVisible(hwnd)):
                for _ in range(10):
                    time.sleep(0.3)
                    h2 = find_epic_window()
                    if h2 and win32gui.IsWindowVisible(h2):
                        hwnd = h2
                        break
        except Exception:
            pass

        if not hwnd:
            log("Failed to find Epic window.")
            exit_code = 2
            return

        log("Epic window found. Probing UI state (no focus steal)...")
        _debug_dump_window(hwnd, "start")

        if mode == "off":
            log("EpicAuth disabled (mode=off).")
            exit_code = 0
            return

        if mode == "blind":
            if not (login and password):
                log("EpicLogin/EpicPassword missing in config.txt")
                _debug_dump_window(hwnd, "missing_creds")
                exit_code = 3
                return
            ok = blind_login(hwnd, login, password, config)
            if ok:
                perform_backup(args.profile)
            exit_code = 0
            return

        # detect/auto: probe templates first
        login_field = probe_template(hwnd, "login_field.png", threshold=0.80, timeout=2)
        password_field = probe_template(hwnd, "password_field.png", threshold=0.80, timeout=2)
        success = probe_template(hwnd, "success.png", threshold=0.90, timeout=2)

        if success and not (login_field or password_field):
            log("Epic appears logged in (success template found).")
            perform_backup(args.profile)
            exit_code = 0
            return

        if (login_field or password_field):
            log("Epic appears NOT logged in (login fields detected).")
            if not (login and password):
                log("EpicLogin/EpicPassword missing in config.txt")
                _debug_dump_window(hwnd, "missing_creds")
                exit_code = 3
                return

            log("Attempting auto-login (template mode)...")
            ok = auto_login(hwnd, login, password)
            if ok:
                perform_backup(args.profile)
                exit_code = 0
                return

            log("Auto-login failed.")
            _debug_dump_window(hwnd, "login_failed")
            exit_code = 4
            return

        # Unknown UI
        if mode == "auto" and (login and password):
            log("Templates mismatch; falling back to blind login...")
            blind_login(hwnd, login, password, config)
            exit_code = 0
            return

        log("Epic UI state UNKNOWN: neither success nor login fields matched. Templates likely mismatch.")
        _debug_dump_window(hwnd, "unknown_ui")
        exit_code = 5
        return
    finally:
        # Always spawn guard so it keeps searching continuously.
        if mode != "off":
            spawn_guard(args.profile)
        if exit_code != 0:
            log(f"Exiting with code {exit_code} (guard continues in background).")
        sys.exit(exit_code)

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        log(f"CRITICAL ERROR: {e}")
        time.sleep(10)
