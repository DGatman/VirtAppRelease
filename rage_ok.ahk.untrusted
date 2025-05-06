; Файл: rage_ok.ahk
#Persistent
SetTitleMatchMode, 2   ; частичное совпадение заголовка

; Бесконечный цикл, чтобы скрипт работал постоянно
Loop
{
    ; Ждём появления окна с «RAGE Multiplayer»
    if WinExist("RAGE Multiplayer")
    {
        ; Активируем окно
        WinActivate
        ; Даем полсекунды на отрисовку
        Sleep, 500
        ; Отправляем нажатие Enter (аналог клика по OK)
        Send, {Enter}
        ; Опционально: дать паузу перед следующим срабатыванием
        Sleep, 2000
    }
    ; Проверяем раз в 200 мс
    Sleep, 5000
}
