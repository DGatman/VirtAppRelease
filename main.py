import subprocess
from datetime import datetime
import sys
try:
    import gspread
    from google.oauth2.service_account import Credentials  # Пример библиотеки
except ImportError:
    print("Некоторые библиотеки не установлены. Устанавливаю...")
    subprocess.check_call([sys.executable, "-m", "pip", "install", "-r", "requirements.txt"])

def check_int(s):
    if s[0] in ('-', '+'):
        return s[1:].isdigit()
    return s.isdigit()

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

    # Ищем строку с нужным значением
    if row_name in column_a:
        row_index = column_a.index(row_name) + 1  # gspread использует 1-based индексы
        if sheet.col_values(4)[row_index - 1] != "Не основа":
            current_time = datetime.now().strftime("%d.%m.%Y %H:%M")
            # Записываем значение в столбец
            if mode == "replace":
                sheet.update_cell(row_index, col, value)
                sheet.update_cell(row_index, 12, current_time)
                print(f"{value} placed to row #{row_index}, column #{col}", flush=True)
            elif mode == "plus":
                old_value = sheet.col_values(col)[row_index - 1]
                if check_int(old_value):
                    new_value = old_value + value
                else:
                    new_value = value
                sheet.update_cell(row_index, col, new_value)
                sheet.update_cell(row_index, 12, current_time)
                print(f"{value} was added to row #{row_index} (was - {old_value}, now - {new_value}), column #{col}", flush=True)
            else:
                print("Wrong mode!", flush=True)

    else:
        print("Account wasn't founded", flush=True)


if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Usage: main.py <row_name> <value> <column_number> <mode>", flush=True)
        sys.exit("Usage: python3 main.py <row_name> <value> <column_number> <mode>")

    row_name = sys.argv[1]
    value = int(sys.argv[2])
    col = int(sys.argv[3])
    mode = sys.argv[4]

    update_google_sheet(row_name, value, col, mode)
