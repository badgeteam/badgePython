import system, keyboard, nvs

def callback(value):
    if value:
        nvs.nvs_setstr("owner", "name", value)
    system.home()

nickname = nvs.nvs_getstr("owner", "name") or ""
keyboard.show("Nickname", nickname, callback)
