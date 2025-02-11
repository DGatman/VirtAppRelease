import json
import subprocess
from datetime import datetime
import sys
import time
try:
    import gspread
    import requests
    from google.oauth2.service_account import Credentials  # Пример библиотеки
except ImportError:
    print("Некоторые библиотеки не установлены. Устанавливаю...", flush=True)
    subprocess.check_call([sys.executable, "-m", "pip", "install", "-r", "requirements.txt"])


def check_int(s):
    if s != "" and s is not None:
        if s[0] in ('-', '+'):
            return s[1:].isdigit()
        return s.isdigit()
    else:
        return False


def send_to_google_sheet(row_name, value, col, mode):
    # Настройка доступа
    SCOPE = ["https://spreadsheets.google.com/feeds",
             "https://www.googleapis.com/auth/drive"]
    creds = Credentials.from_service_account_file("credentials.json", scopes=SCOPE)
    client = gspread.authorize(creds)

    # Открываем таблицу и лист "Общая"
    sheet = client.open("Учет виртов").worksheet("Total")

    # Получаем все данные из столбца А и состояния
    column_a = sheet.col_values(1)  # PC NAME
    status_column = sheet.col_values(4)  # Состояние (основа / не основа)

    # Проходим по всем совпадениям
    for row_index, name in enumerate(column_a, start=1):
        if name == row_name and status_column[row_index - 1] != "Не основа":
            current_time = datetime.now().strftime("%d.%m.%Y %H:%M")

            if mode == "replace":
                sheet.update_cell(row_index, col, value)
                sheet.update_cell(row_index, 12, current_time)
                print(f"{value} placed to row #{row_index}, column #{col}", flush=True)
                return  # После обновления выходим из функции

            elif mode == "plus":
                old_value = sheet.cell(row_index, col).value
                if check_int(old_value):
                    new_value = int(old_value) + value
                else:
                    new_value = value
                sheet.update_cell(row_index, col, new_value)
                sheet.update_cell(row_index, 12, current_time)
                print(f"{value} was added to row #{row_index} (was - {old_value}, now - {new_value}), column #{col}",
                      flush=True)
                return  # После обновления выходим из функции

    print("Account wasn't founded or all matches are 'Не основа'", flush=True)

def get_profiles(login):
    profiles = ["jopa", "jorik"]

    # Login
    url = "https://gta5rp.com/api/V2/users/auth/login"

    payload = "{\"2fa_code\": \"593519\", \"login\": \"DGatman\", \"password\": \"Leik5te5\", \"remember\": \"0\"}"
    headers = {
        'content-type': "application/json"
    }

    response = requests.request("POST", url, data=payload, headers=headers)
    account = json.loads(response.text)
    token = account["token"]


    #Getting profiles
    url = "https://gta5rp.com/api/V2/users/chars/13"

    headers = {
        'x-access-token': token
    }

    response = requests.request("GET", url,headers=headers)
    print(response.text)
    profiles = json.loads(response.text)
    return profiles

if __name__ == "__main__":
    if len(sys.argv) == 5:
        row_name = sys.argv[1]
        value = int(sys.argv[2])
        col = int(sys.argv[3])
        mode = sys.argv[4]
        send_to_google_sheet(row_name, value, col, mode)

    elif len(sys.argv) == 2:
        login = sys.argv[1]
        profiles = get_profiles(login)
        print("Profiles:\t")
        for i in profiles:
            print("Name:\t" + i["name"])
            print("LVL:\t" + str(i["lvl"]))
            print("Cash:\t" + str(i["cash"]))
            print("Bank:\t" + str(i["bank"]))
            print("House:\tYES" if i["house"] is not None else "House:\tNO")
            print("Apartment:\tYES" if i["apartment"] is not None else "Apartment:\tNO")
            print("VEHICLES:")
            for v in i["vehicles"]:
                print("\t" + v["title"])
            print("Vip type:\t" + i["vip_name"])
            print("Vip duration:\t" + str(round((i["vip_expire_at"] - time.time())/86400)) + " days")

    else:
        print("Usage: main.py <row_name> <value> <column_number> <mode>", flush=True)
        sys.exit("Usage: python3 main.py <row_name> <value> <column_number> <mode>")


