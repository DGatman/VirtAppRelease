import subprocess
from datetime import datetime
import sys
try:
    import gspread
    from google.oauth2.service_account import Credentials  # Пример библиотеки
except ImportError:
    print("Некоторые библиотеки не установлены. Устанавливаю...")
    subprocess.check_call([sys.executable, "-m", "pip", "install", "-r", "requirements.txt"])

def update_google_sheet(row_name, value, col, mode):
    # Настройка доступа
    SCOPE = ["https://spreadsheets.google.com/feeds",
             "https://www.googleapis.com/auth/drive"]
    creds = Credentials.from_service_account_file("credentials.json", scopes=SCOPE)
    client = gspread.authorize(creds)

    # Открываем таблицу и лист "Общая"
    sheet = client.open("Учет виртов").worksheet("Total")

    # Получаем все данные из столбца А
    column_a = sheet.col_values(1)  # PC NAME
    column_b = sheet.col_values(2)  # Server
    column_c = sheet.col_values(3)  # Name
    column_d = sheet.col_values(4)  # Vip
    column_e = sheet.col_values(5)  # VipDuration
    column_g = sheet.col_values(7)  # Money
    column_h = sheet.col_values(8)  # Tips
    column_i = sheet.col_values(9)  # BP
    column_j = sheet.col_values(10)  # DP
    column_k = sheet.col_values(11)  # Flat
    column_l = sheet.col_values(12)  # Date

    # Ищем строку с нужным значением
    if row_name in column_a:
        row_index = column_a.index(row_name) + 1  # gspread использует 1-based индексы
        current_time = datetime.now().strftime("%d.%m.%Y %H:%M")
        # Записываем значение в столбец I (9-й столбец)
        if mode == "replace":
            sheet.update_cell(row_index, col, value)
            sheet.update_cell(row_index, 12, current_time)
            print(f"Значение {value} записано в строку {row_index}, столбец {col}")
        elif mode == "plus":
            new_value = int(sheet.col_values(col)[row_index - 1]) + value
            sheet.update_cell(row_index, col, new_value)
            sheet.update_cell(row_index, 12, current_time)
            print(f"Значение {value} добавлено к строке {row_index} (итог {new_value}), столбец {col}")
        else :
            print("Неизвестный режим!")

    else:
        print("Аккаунт не найден в таблице")


if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Использование: python update_google_sheets.py <row_name> <value> <column_number> <mode>")
        sys.exit(1)

    row_name = sys.argv[1]
    value = int(sys.argv[2])
    col = int(sys.argv[3])
    mode = sys.argv[4]

    update_google_sheet(row_name, value, col, mode)
