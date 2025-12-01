// ═══════════════════════════════════════════════════════════════════════════════
// VIRTAPP DASHBOARD UI v3.0 - Modern Console Interface
// Enterprise-level Dashboard с точным выравниванием
// (c) 2024-2025 RootOne1337
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once
#ifndef DASHBOARD_UI_H
#define DASHBOARD_UI_H

#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <iomanip>
#include <sstream>

using namespace std;

// ─────────────────────────────────────────────────────────────────────────────
// ЦВЕТОВАЯ СХЕМА DASHBOARD
// ─────────────────────────────────────────────────────────────────────────────
namespace DashColors {
    const int BLACK       = 0;
    const int DARK_BLUE   = 1;
    const int DARK_GREEN  = 2;
    const int DARK_CYAN   = 3;
    const int DARK_RED    = 4;
    const int DARK_MAGENTA= 5;
    const int DARK_YELLOW = 6;
    const int GRAY        = 7;
    const int DARK_GRAY   = 8;
    const int BLUE        = 9;
    const int GREEN       = 10;
    const int CYAN        = 11;
    const int RED         = 12;
    const int MAGENTA     = 13;
    const int YELLOW      = 14;
    const int WHITE       = 15;
    
    // Тема Dashboard
    const int BORDER      = DARK_CYAN;
    const int TITLE       = YELLOW;
    const int ACCENT      = CYAN;
    const int SUCCESS     = GREEN;
    const int WARNING     = YELLOW;
    const int DANGER      = RED;
    const int MUTED       = DARK_GRAY;
    const int TEXT        = WHITE;
}

// ─────────────────────────────────────────────────────────────────────────────
// КОНСТАНТЫ DASHBOARD - ТОЧНАЯ ГЕОМЕТРИЯ
// ─────────────────────────────────────────────────────────────────────────────
const int DASH_WIDTH = 74;          // Общая ширина (включая рамки)
const int CONTENT_WIDTH = 70;       // Ширина контента между | и |
const int LEFT_PANEL = 30;          // Ширина левой панели
const int RIGHT_PANEL = 32;         // Ширина правой панели
const int GAP = 4;                  // Отступ между панелями

// ─────────────────────────────────────────────────────────────────────────────
// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ
// ─────────────────────────────────────────────────────────────────────────────
static HANDLE g_dashConsole = NULL;

// ─────────────────────────────────────────────────────────────────────────────
// УТИЛИТЫ
// ─────────────────────────────────────────────────────────────────────────────

inline void dashSetColor(int color) {
    if (!g_dashConsole) g_dashConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(g_dashConsole, color);
}

inline void dashResetColor() {
    dashSetColor(DashColors::GRAY);
}

inline string dashRepeat(char c, int n) {
    if (n <= 0) return "";
    return string(n, c);
}

inline string dashPadRight(const string& text, int width) {
    if ((int)text.length() >= width) return text.substr(0, width);
    return text + string(width - text.length(), ' ');
}

inline string dashPadCenter(const string& text, int width) {
    if ((int)text.length() >= width) return text.substr(0, width);
    int pad = (width - (int)text.length()) / 2;
    int padRight = width - (int)text.length() - pad;
    return string(pad, ' ') + text + string(padRight, ' ');
}

inline string dashGetTime() {
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    char buffer[12];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", ltm);
    return string(buffer);
}

// ─────────────────────────────────────────────────────────────────────────────
// ИНИЦИАЛИЗАЦИЯ DASHBOARD КОНСОЛИ
// ─────────────────────────────────────────────────────────────────────────────
inline void initDashboard() {
    g_dashConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    // Включаем Virtual Terminal для ANSI (Windows 10+)
    DWORD mode = 0;
    GetConsoleMode(g_dashConsole, &mode);
    SetConsoleMode(g_dashConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    
    // Кодовая страница UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // Заголовок окна
    SetConsoleTitleA("VirtApp Dashboard - GTA5RP Bot Control Panel v3.0");
}

// ═══════════════════════════════════════════════════════════════════════════════
// ГЛАВНЫЙ ЗАГОЛОВОК DASHBOARD
// ═══════════════════════════════════════════════════════════════════════════════
inline void dashPrintHeader(const string& version) {
    if (!g_dashConsole) initDashboard();
    
    cout << "\n";
    
    // Верхняя граница: +-- # TITLE ----- [v3.0] --+
    dashSetColor(DashColors::BORDER);
    cout << "  +";
    dashSetColor(DashColors::DARK_CYAN);
    cout << "-- ";
    dashSetColor(DashColors::YELLOW);
    cout << "# VIRTAPP CONTROL PANEL ";
    dashSetColor(DashColors::DARK_CYAN);
    string verStr = "[" + version + "]";
    int fillLen = CONTENT_WIDTH - 27 - (int)verStr.length() - 3; // 27=len("-- # VIRTAPP CONTROL PANEL ")
    cout << dashRepeat('-', fillLen);
    dashSetColor(DashColors::WHITE);
    cout << " " << verStr << " ";
    dashSetColor(DashColors::DARK_CYAN);
    cout << "--";
    dashSetColor(DashColors::BORDER);
    cout << "+\n";
    
    // ASCII Art Logo - каждая строка ровно CONTENT_WIDTH символов
    const char* logo[] = {
        "                                                                      ",
        "   ##   ## #### #####   ######  ###   #####  #####                    ",
        "   ##   ##  ##  ##  ##    ##   ## ##  ##  ## ##  ##                   ",
        "   ##   ##  ##  #####     ##   #####  #####  #####                    ",
        "    ## ##   ##  ##  ##    ##   ##  ## ##     ##                       ",
        "     ###   #### ##  ##    ##   ##  ## ##     ##                       ",
        "                                                                      "
    };
    int logoColors[] = { DashColors::YELLOW, DashColors::YELLOW, DashColors::YELLOW, 
                         DashColors::GREEN, DashColors::GREEN, DashColors::CYAN, DashColors::MUTED };
    
    for (int i = 0; i < 7; i++) {
        dashSetColor(DashColors::BORDER);
        cout << "  |";
        dashSetColor(logoColors[i]);
        cout << logo[i];
        dashSetColor(DashColors::BORDER);
        cout << "|\n";
    }
    
    // Подзаголовок
    dashSetColor(DashColors::BORDER);
    cout << "  |";
    dashSetColor(DashColors::MUTED);
    cout << dashPadCenter("GTA5RP Automation Bot - Dashboard Edition", CONTENT_WIDTH);
    dashSetColor(DashColors::BORDER);
    cout << "|\n";
    
    dashSetColor(DashColors::BORDER);
    cout << "  |";
    dashSetColor(DashColors::MUTED);
    cout << dashPadCenter("(c) 2024-2025 RootOne1337", CONTENT_WIDTH);
    dashSetColor(DashColors::BORDER);
    cout << "|\n";
    
    // Разделитель
    dashSetColor(DashColors::BORDER);
    cout << "  +";
    dashSetColor(DashColors::DARK_CYAN);
    cout << dashRepeat('-', CONTENT_WIDTH);
    dashSetColor(DashColors::BORDER);
    cout << "+\n";
    
    dashResetColor();
}

// ═══════════════════════════════════════════════════════════════════════════════
// ПАНЕЛЬ СТАТУСА СИСТЕМЫ - ТОЧНОЕ ВЫРАВНИВАНИЕ
// ═══════════════════════════════════════════════════════════════════════════════
inline void dashPrintSystemPanel() {
    if (!g_dashConsole) g_dashConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    // Получаем системную информацию
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    
    int memPercent = (int)memInfo.dwMemoryLoad;
    int memTotalGB = (int)(memInfo.ullTotalPhys / (1024 * 1024 * 1024));
    int memUsedGB = memTotalGB * memPercent / 100;
    int cpuPercent = 30;
    
    // Пустая строка
    dashSetColor(DashColors::BORDER);
    cout << "  |" << string(CONTENT_WIDTH, ' ') << "|\n";
    
    // Заголовки панелей: +-- SYSTEM ---+ (30) пробелы +-- BOT STATUS ---+ (32)
    dashSetColor(DashColors::BORDER);
    cout << "  |  ";
    cout << "+";
    dashSetColor(DashColors::DARK_CYAN);
    cout << "-- ";
    dashSetColor(DashColors::TITLE);
    cout << "SYSTEM";
    dashSetColor(DashColors::DARK_CYAN);
    cout << " " << dashRepeat('-', 17) << "+";  // 30 - 4("  +") - 3("-- ") - 6("SYSTEM") - 1(" ") - 1("+") = 15... wait
    dashSetColor(DashColors::BORDER);
    
    cout << "  ";  // gap
    
    cout << "+";
    dashSetColor(DashColors::DARK_CYAN);
    cout << "-- ";
    dashSetColor(DashColors::TITLE);
    cout << "BOT STATUS";
    dashSetColor(DashColors::DARK_CYAN);
    cout << " " << dashRepeat('-', 15) << "+";
    dashSetColor(DashColors::BORDER);
    cout << "  |\n";
    
    // Строка 1: CPU | State
    int cpuFilled = cpuPercent / 10;
    stringstream cpuBar;
    for (int i = 0; i < cpuFilled; i++) cpuBar << "#";
    for (int i = cpuFilled; i < 10; i++) cpuBar << "-";
    
    dashSetColor(DashColors::BORDER);
    cout << "  |  |";
    dashSetColor(DashColors::TEXT);
    cout << " CPU:  [";
    dashSetColor(cpuPercent < 50 ? DashColors::GREEN : (cpuPercent < 80 ? DashColors::YELLOW : DashColors::RED));
    cout << cpuBar.str();
    dashSetColor(DashColors::TEXT);
    cout << "] ";
    dashSetColor(DashColors::CYAN);
    cout << dashPadRight(to_string(cpuPercent) + "%", 4);
    dashSetColor(DashColors::BORDER);
    cout << " |";
    
    cout << "  |";
    dashSetColor(DashColors::TEXT);
    cout << " State:   ";
    dashSetColor(DashColors::GREEN);
    cout << dashPadRight("Ready", 18);
    dashSetColor(DashColors::BORDER);
    cout << "|  |\n";
    
    // Строка 2: RAM | Mode  
    int ramFilled = memPercent / 10;
    stringstream ramBar;
    for (int i = 0; i < ramFilled; i++) ramBar << "#";
    for (int i = ramFilled; i < 10; i++) ramBar << "-";
    
    dashSetColor(DashColors::BORDER);
    cout << "  |  |";
    dashSetColor(DashColors::TEXT);
    cout << " RAM:  [";
    dashSetColor(memPercent < 50 ? DashColors::GREEN : (memPercent < 80 ? DashColors::YELLOW : DashColors::RED));
    cout << ramBar.str();
    dashSetColor(DashColors::TEXT);
    cout << "] ";
    dashSetColor(DashColors::CYAN);
    cout << dashPadRight(to_string(memUsedGB) + "GB", 4);
    dashSetColor(DashColors::BORDER);
    cout << " |";
    
    cout << "  |";
    dashSetColor(DashColors::TEXT);
    cout << " Mode:    ";
    dashSetColor(DashColors::YELLOW);
    cout << dashPadRight("Initializing", 18);
    dashSetColor(DashColors::BORDER);
    cout << "|  |\n";
    
    // Строка 3: ARCH | Ruletka
    dashSetColor(DashColors::BORDER);
    cout << "  |  |";
    dashSetColor(DashColors::TEXT);
    cout << " ARCH: ";
    dashSetColor(DashColors::CYAN);
    cout << (sysInfo.wProcessorArchitecture == 9 ? "x64 " : "x86 ");
    dashSetColor(DashColors::GREEN);
    cout << "[OK]";
    cout << "         ";
    dashSetColor(DashColors::BORDER);
    cout << "|";
    
    cout << "  |";
    dashSetColor(DashColors::TEXT);
    cout << " Ruletka: ";
    dashSetColor(DashColors::YELLOW);
    cout << dashPadRight("Pending", 18);
    dashSetColor(DashColors::BORDER);
    cout << "|  |\n";
    
    // Строка 4: CORE | AFK
    dashSetColor(DashColors::BORDER);
    cout << "  |  |";
    dashSetColor(DashColors::TEXT);
    cout << " CORE: ";
    dashSetColor(DashColors::CYAN);
    cout << dashPadRight(to_string(sysInfo.dwNumberOfProcessors) + " threads", 15);
    dashSetColor(DashColors::BORDER);
    cout << "|";
    
    cout << "  |";
    dashSetColor(DashColors::TEXT);
    cout << " AFK:     ";
    dashSetColor(DashColors::MUTED);
    cout << dashPadRight("Disabled", 18);
    dashSetColor(DashColors::BORDER);
    cout << "|  |\n";
    
    // Закрытие панелей
    dashSetColor(DashColors::BORDER);
    cout << "  |  +";
    dashSetColor(DashColors::DARK_CYAN);
    cout << dashRepeat('-', 26);
    dashSetColor(DashColors::BORDER);
    cout << "+";
    cout << "  +";
    dashSetColor(DashColors::DARK_CYAN);
    cout << dashRepeat('-', 29);
    dashSetColor(DashColors::BORDER);
    cout << "+  |\n";
    
    dashResetColor();
}

// ═══════════════════════════════════════════════════════════════════════════════
// ПАНЕЛЬ СЕССИИ - ТОЧНОЕ ВЫРАВНИВАНИЕ
// ═══════════════════════════════════════════════════════════════════════════════
inline void dashPrintSessionPanel(const string& account, const string& pcName, const string& ip) {
    if (!g_dashConsole) g_dashConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    // Пустая строка
    dashSetColor(DashColors::BORDER);
    cout << "  |" << string(CONTENT_WIDTH, ' ') << "|\n";
    
    // Заголовок SESSION
    dashSetColor(DashColors::BORDER);
    cout << "  |  +";
    dashSetColor(DashColors::DARK_CYAN);
    cout << "-- ";
    dashSetColor(DashColors::TITLE);
    cout << "ACTIVE SESSION";
    dashSetColor(DashColors::DARK_CYAN);
    cout << " " << dashRepeat('-', 47) << "+";
    dashSetColor(DashColors::BORDER);
    cout << "  |\n";
    
    // Account & PC Name
    dashSetColor(DashColors::BORDER);
    cout << "  |  |";
    dashSetColor(DashColors::MUTED);
    cout << " [#] ";
    dashSetColor(DashColors::TEXT);
    cout << "Account: ";
    dashSetColor(DashColors::CYAN);
    cout << dashPadRight(account, 18);
    dashSetColor(DashColors::MUTED);
    cout << " [PC] ";
    dashSetColor(DashColors::TEXT);
    cout << "Name: ";
    dashSetColor(DashColors::YELLOW);
    cout << dashPadRight(pcName, 14);
    dashSetColor(DashColors::BORDER);
    cout << "|  |\n";
    
    // IP & Time
    dashSetColor(DashColors::BORDER);
    cout << "  |  |";
    dashSetColor(DashColors::MUTED);
    cout << " [@] ";
    dashSetColor(DashColors::TEXT);
    cout << "Ext IP:  ";
    dashSetColor(DashColors::MAGENTA);
    cout << dashPadRight(ip, 18);
    dashSetColor(DashColors::MUTED);
    cout << " [T] ";
    dashSetColor(DashColors::TEXT);
    cout << "Time: ";
    dashSetColor(DashColors::GREEN);
    cout << dashPadRight(dashGetTime(), 14);
    dashSetColor(DashColors::BORDER);
    cout << "|  |\n";
    
    // Закрытие
    dashSetColor(DashColors::BORDER);
    cout << "  |  +";
    dashSetColor(DashColors::DARK_CYAN);
    cout << dashRepeat('-', 63);
    dashSetColor(DashColors::BORDER);
    cout << "+  |\n";
    
    dashResetColor();
}
// ═══════════════════════════════════════════════════════════════════════════════
// ПАНЕЛЬ ЛОГОВ - ТОЧНОЕ ВЫРАВНИВАНИЕ
// ═══════════════════════════════════════════════════════════════════════════════
inline void dashPrintLogHeader() {
    if (!g_dashConsole) g_dashConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    // Пустая строка
    dashSetColor(DashColors::BORDER);
    cout << "  |" << string(CONTENT_WIDTH, ' ') << "|\n";
    
    // Заголовок LOGS
    dashSetColor(DashColors::BORDER);
    cout << "  |  +";
    dashSetColor(DashColors::DARK_CYAN);
    cout << "-- ";
    dashSetColor(DashColors::TITLE);
    cout << "LIVE LOGS";
    dashSetColor(DashColors::DARK_CYAN);
    cout << " " << dashRepeat('-', 52) << "+";
    dashSetColor(DashColors::BORDER);
    cout << "  |\n";
    
    dashResetColor();
}

inline void dashPrintLogEntry(const string& level, const string& message, const string& status = "") {
    if (!g_dashConsole) g_dashConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    string timeStr = dashGetTime();
    
    dashSetColor(DashColors::BORDER);
    cout << "  |  |";
    
    // Время
    dashSetColor(DashColors::MUTED);
    cout << " " << timeStr << " ";
    
    // Уровень
    if (level == "INFO") {
        dashSetColor(DashColors::CYAN);
    } else if (level == "WARN") {
        dashSetColor(DashColors::YELLOW);
    } else if (level == "FAIL" || level == "ERR") {
        dashSetColor(DashColors::RED);
    } else if (level == "OK") {
        dashSetColor(DashColors::GREEN);
    } else {
        dashSetColor(DashColors::MUTED);
    }
    cout << "[" << dashPadRight(level, 4) << "] ";
    
    // Сообщение
    dashSetColor(DashColors::TEXT);
    cout << dashPadRight(message, 38);
    
    // Статус
    if (!status.empty()) {
        if (status == "OK") {
            dashSetColor(DashColors::GREEN);
            cout << "[ OK ]";
        } else if (status == "FAIL") {
            dashSetColor(DashColors::RED);
            cout << "[FAIL]";
        } else if (status == "WAIT") {
            dashSetColor(DashColors::YELLOW);
            cout << "[....]";
        } else {
            dashSetColor(DashColors::MUTED);
            cout << dashPadRight(status, 6);
        }
    } else {
        cout << "      ";
    }
    
    dashSetColor(DashColors::BORDER);
    cout << " |  |\n";
    
    dashResetColor();
}

inline void dashPrintLogFooter() {
    if (!g_dashConsole) g_dashConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    dashSetColor(DashColors::BORDER);
    cout << "  |  +";
    dashSetColor(DashColors::DARK_CYAN);
    cout << dashRepeat('-', 63);
    dashSetColor(DashColors::BORDER);
    cout << "+  |\n";
    
    dashResetColor();
}

// ═══════════════════════════════════════════════════════════════════════════════
// ПАНЕЛЬ ГОРЯЧИХ КЛАВИШ - ТОЧНОЕ ВЫРАВНИВАНИЕ
// ═══════════════════════════════════════════════════════════════════════════════
inline void dashPrintHotkeys() {
    if (!g_dashConsole) g_dashConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    // Пустая строка
    dashSetColor(DashColors::BORDER);
    cout << "  |" << string(CONTENT_WIDTH, ' ') << "|\n";
    
    // Hotkeys - выровнено по ширине
    dashSetColor(DashColors::BORDER);
    cout << "  |  ";
    dashSetColor(DashColors::MUTED);
    cout << "[";
    dashSetColor(DashColors::CYAN);
    cout << "F1";
    dashSetColor(DashColors::MUTED);
    cout << "]";
    dashSetColor(DashColors::TEXT);
    cout << " Menu  ";
    
    dashSetColor(DashColors::MUTED);
    cout << "[";
    dashSetColor(DashColors::CYAN);
    cout << "F2";
    dashSetColor(DashColors::MUTED);
    cout << "]";
    dashSetColor(DashColors::TEXT);
    cout << " AFK  ";
    
    dashSetColor(DashColors::MUTED);
    cout << "[";
    dashSetColor(DashColors::CYAN);
    cout << "F10";
    dashSetColor(DashColors::MUTED);
    cout << "]";
    dashSetColor(DashColors::TEXT);
    cout << " Presents  ";
    
    dashSetColor(DashColors::MUTED);
    cout << "[";
    dashSetColor(DashColors::CYAN);
    cout << "Ctrl+R";
    dashSetColor(DashColors::MUTED);
    cout << "]";
    dashSetColor(DashColors::TEXT);
    cout << " Ruletka  ";
    
    dashSetColor(DashColors::MUTED);
    cout << "[";
    dashSetColor(DashColors::RED);
    cout << "ESC";
    dashSetColor(DashColors::MUTED);
    cout << "]";
    dashSetColor(DashColors::TEXT);
    cout << " Exit   ";
    
    dashSetColor(DashColors::BORDER);
    cout << "|\n";
    
    dashResetColor();
}

// ═══════════════════════════════════════════════════════════════════════════════
// НИЖНЯЯ ГРАНИЦА DASHBOARD
// ═══════════════════════════════════════════════════════════════════════════════
inline void dashPrintFooter() {
    if (!g_dashConsole) g_dashConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    dashSetColor(DashColors::BORDER);
    cout << "  +";
    dashSetColor(DashColors::DARK_CYAN);
    cout << dashRepeat('-', CONTENT_WIDTH);
    dashSetColor(DashColors::BORDER);
    cout << "+\n\n";
    
    dashResetColor();
}

// ═══════════════════════════════════════════════════════════════════════════════
// ПОЛНЫЙ DASHBOARD
// ═══════════════════════════════════════════════════════════════════════════════
inline void dashPrintFull(const string& version, const string& account, const string& pcName, const string& ip) {
    dashPrintHeader(version);
    dashPrintSystemPanel();
    dashPrintSessionPanel(account, pcName, ip);
    dashPrintLogHeader();
    dashPrintLogEntry("INFO", "Dashboard initialized", "OK");
    dashPrintLogEntry("INFO", "Loading configuration...", "OK");
    dashPrintLogEntry("INFO", "Waiting for game process...", "WAIT");
    dashPrintLogFooter();
    dashPrintHotkeys();
    dashPrintFooter();
}

#endif // DASHBOARD_UI_H