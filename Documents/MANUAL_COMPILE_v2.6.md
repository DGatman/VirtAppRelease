# 📋 Инструкция: Ручная компиляция VirtApp v2.6

## ✅ Что нужно сделать в Visual Studio 2022

### Шаг 1: Открыть Visual Studio 2022

1. Запущена в фоне (используй Ctrl+Alt+Delete и ищи devenv.exe)
2. Или открой вручную: `C:\Program Files\Microsoft Visual Studio\18\Community\devenv.exe`

### Шаг 2: Создать новый проект из существующего кода

```
File → New → Project from Existing Code
```

**Заполни параметры:**

- **Project file location**: `C:\Users\Nifilim-Server#1\Documents\GTA5rpVirt`
- **Project name**: `VirtApp`
- **Project type**: `Visual C++`

### Шаг 3: Добавить исходные файлы

В Solution Explorer (справа):
1. Правый клик на проект → **Add** → **Existing Item**
2. Добавить из папки `src/`:
   - `main.cpp`
   - `ScreenScaner.cpp`
   - `ScreenScaner.h`
   - `utils.c`
   - `utils.h`
   - `VirtApp.h`

### Шаг 4: Добавить подключаемые библиотеки

**Project** → **Properties** → **VC++ Directories**:

- **Include Directories**: Добавь пути к библиотекам:
  ```
  C:\boost\include
  C:\leptonica\include
  C:\tesseract\include
  C:\curl\include
  ```

- **Library Directories**: Добавь пути к lib файлам:
  ```
  C:\boost\lib
  C:\leptonica\lib
  C:\tesseract\lib
  C:\curl\lib
  ```

### Шаг 5: Проверить зависимости

В коде есть подключения к:
- **Tesseract OCR** (для распознавания текста)
- **Boost** (утилиты)
- **libcurl** (HTTP запросы)
- **TgBot** (Telegram Bot API)
- **SQLite3** (база данных)
- **Interception** (захват ввода)

⚠️ **ВАЖНО**: Если библиотеки не установлены, это вызовет ошибки линкера!

### Шаг 6: Скомпилировать

1. **Build → Clean Solution** (очистить)
2. **Build → Rebuild Solution** (пересобрать)
3. **Выбрать Configuration**: `Release | x64`

### Шаг 7: Найти exe

После успешной компиляции exe будет в:
```
C:\Users\Nifilim-Server#1\Documents\GTA5rpVirt\Release\VirtApp.exe
```

---

## ⚠️ Если компиляция не работает

### Ошибка: "Не найден заголовочный файл"
- Проверить пути в **VC++ Directories**
- Убедиться, что библиотеки установлены

### Ошибка: "Не найдена библиотека при линковке"
- Проверить **.lib** файлы в **Library Directories**
- Может быть нужно собрать библиотеки из исходников

### Ошибка: "Требуется Windows SDK"
- **Project → Properties → Platform Toolset**: выбрать установленный
- Обычно: `Visual Studio 2022 (v143)`

---

## 📝 Альтернатива: Использовать командную строку

Если VS GUI не работает, используй **Developer Command Prompt for VS 2022**:

```cmd
cd C:\Users\Nifilim-Server#1\Documents\GTA5rpVirt
msbuild VirtApp.sln /p:Configuration=Release /p:Platform=x64
```

(Требуется чтобы `.sln` и `.vcxproj` файлы уже существовали)

---

## ✨ После успешной компиляции

1. Скопировать `.exe` из папки `Release` в `Release_Build`
2. Переименовать в `VirtApp_v2.6.exe`
3. Проверить размер файла (должен быть > 1 MB, не 0.5 MB!)
4. Запустить и тестировать

**Готово!** 🎉
