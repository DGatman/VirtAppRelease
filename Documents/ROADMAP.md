# Roadmap

## ✅ Completed
- [x] Epic Games Authentication (Immortal Cycle v7.1)
- [x] Time Synchronization Optimization
- [x] Mouse Input Passthrough (Interception Driver Fix)
- [x] C++ Project Compilation (Release Build)
- [x] Убран авт ребут в блоке presents/рулетка (лог + пропуск цикла)
- [x] Safe exit меню подарков переведен на F10 вместо ESC
- [x] Сбор подарков/BP отключен, оставлена рулетка-only логика
- [x] Старт: защита от пустого TOKEN и зависаний на отсутствующих авто-скриптах (python/main.py/command_cmd.bat)
- [x] Интеграция API Config Client (`get_config.py`)
- [x] Реализация Soft Reboot с сохранением сессий Epic
- [x] Исправление багов Google Sheets (Python import fix)

## 🚧 In Progress
- [ ] Проверка на стенде: отсутствие ребута при сбоях открытия телефона/рулетки
- [ ] Verifying input behavior in-game
- [ ] Тестирование `/update_config` в реальных условиях

## 📅 Planned
- [ ] Refactor `main.cpp` to separate concerns (Input vs Logic)
- [ ] Add configuration file for Time Sync parameters
- [ ] Implement auto-update mechanism for the binary
