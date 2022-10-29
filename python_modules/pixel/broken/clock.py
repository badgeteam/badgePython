import time, ntp, rgb, wifi, buttons, defines, system, machine
from default_icons import animation_connecting_wifi, icon_no_wifi
import esp32

direction = 0

if not ntp.local_time_available():
    if not wifi.status():
        data, size, frames = animation_connecting_wifi
        rgb.clear()
        rgb.framerate(3)
        rgb.gif(data, (12, 0), size, frames)
        wifi.connect()
        if wifi.wait():
            rgb.clear()
            rgb.framerate(20)
        else:
            print('No wifi')
            rgb.clear()
            rgb.framerate(20)
            data, frames = icon_no_wifi
            rgb.image(data, (12, 0), (8,8))
            time.sleep(3)
            rgb.clear()
            print("Error connecting to wifi")
            system.reboot()

    if not ntp.set_NTP_time():
        print("Error setting time")
        system.reboot()

wifi.disconnect()

## Taken from Bas' PrettyClock.
## Get your timezone from
## https://remotemonitoringsystems.ca/time-zone-abbreviations.php
rtc=machine.RTC()
try:
    nvs = esp32.NVS("system")
    timezoneba = bytearray(50)
    nvs.get_blob("timezone", timezoneba)
    timezone = timezoneba.decode("utf-8")
except Exception as e:
    timezone = 'CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00'
machine.RTC().timezone(timezone)

def input_B(pressed):
    global direction
    direction = defines.BTN_B

buttons.register(defines.BTN_B, input_B)

gifd = [0, 0x00FFFFFF, 0, 0, 0, 0x00FFFFFF, 0, 0, 0, 0, 0, 0, 0, 0]

tmold = 70
rgb.background((0,0,0))
rgb.clear()
rgb.framerate(1)

while direction != defines.BTN_B:
    th = '%02d' % rtc.now()[3]
    tm = '%02d' % rtc.now()[4]
    if tm != tmold:
        rgb.clear()
        rgb.text(th, (255, 255, 255), (3, 0))
        rgb.text(tm, (255, 255, 255), (18, 0))
        rgb.gif(gifd, (15, 0), (1, 7), 2)
        rgb.gif(gifd, (16, 0), (1, 7), 2)
        tmold = tm
    time.sleep(0.2)
system.reboot()