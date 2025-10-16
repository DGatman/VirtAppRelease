import http.client
import json
import re
import ctypes

# Структура SYSTEMTIME для WinAPI
class SYSTEMTIME(ctypes.Structure):
    _fields_ = [
        ("wYear", ctypes.c_ushort),
        ("wMonth", ctypes.c_ushort),
        ("wDayOfWeek", ctypes.c_ushort),
        ("wDay", ctypes.c_ushort),
        ("wHour", ctypes.c_ushort),
        ("wMinute", ctypes.c_ushort),
        ("wSecond", ctypes.c_ushort),
        ("wMilliseconds", ctypes.c_ushort),
    ]

def get_moscow_time():
    conn = http.client.HTTPConnection("worldtimeapi.org", 80, timeout=10)
    conn.request("GET", "/api/timezone/Europe/Moscow")
    response = conn.getresponse()
    if response.status != 200:
        raise Exception(f"HTTP Error {response.status}")
    data = json.loads(response.read().decode())
    conn.close()

    datetime_str = data["datetime"]  # Пример: "2025-06-19T14:30:00.123456+03:00"
    match = re.match(r"(\d+)-(\d+)-(\d+)T(\d+):(\d+):(\d+)", datetime_str)
    if not match:
        raise ValueError("Не удалось распарсить дату и время")
    return tuple(map(int, match.groups()))

def set_system_time(year, month, day, hour, minute, second):
    st = SYSTEMTIME()
    st.wYear = year
    st.wMonth = month
    st.wDay = day
    st.wHour = hour
    st.wMinute = minute
    st.wSecond = second
    st.wMilliseconds = 0
    st.wDayOfWeek = 0  # Windows сам определяет

    if not ctypes.windll.kernel32.SetLocalTime(ctypes.byref(st)):
        raise ctypes.WinError()

def main():
    try:
        year, month, day, hour, minute, second = get_moscow_time()
        print(f"Устанавливаю системное время: {day:02}.{month:02}.{year} {hour:02}:{minute:02}:{second:02}")
        set_system_time(year, month, day, hour, minute, second)
        print("✅ Системное время успешно обновлено.")
    except Exception as e:
        print("❌ Ошибка:", e)

if __name__ == "__main__":
    main()
