import platform
import led
import led_keys

def ledstatus(status):
    if status & 0x01:
        led.setKey(led_keys.NUMLOCK, (0x10, 0x10, 0x10))
    else:
        led.setKey(led_keys.NUMLOCK, (0, 0, 0))
    if status & 0x02:
        led.setKey(led_keys.CAPSLOCK, (0x10, 0x10, 0x10))
    else:
        led.setKey(led_keys.CAPSLOCK, (0,0,0))

def shutdown_keyboard_manager():
    platform.remove_statusled_handler(ledstatus)

def start_keyboard_manager():
    platform.add_statusled_handler(ledstatus)

platform.add_statusled_handler(ledstatus)
platform.backlit_power()
