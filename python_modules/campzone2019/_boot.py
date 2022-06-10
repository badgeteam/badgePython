import uos, gc, sys, machine

folders = ['lib', 'apps', 'cache', 'cache/woezel', 'config']
print("Running _boot.py")
for folder in folders:
    try:
        uos.mkdir(folder)
    except Exception as error:
        pass

import system, virtualtimers
# This doesn't work in micropython/main.c because micropython can't handle
# slash characters before single characters that are also HTML elements,
# like <a> or <s> (e.g. /apps or /sdcard won't work.)mi
sys.path.append('/apps')
sys.path.append('/lib')

# Hijack the system start function to fix some CZ19 screen issues
orig_start = system.start
def hijacked_start(app, status=True):
    import rgb, time
    rgb.clear()
    time.sleep(0.1)
    orig_start(app, status)
system.start = hijacked_start

## Make badge sleep in undervoltage conditions
virtualtimers.activate(1000) # low resolution needed
def _vcc_callback():
    try:
        vcc = system.get_vcc_bat()
        if vcc != None:
            if vcc < 3300:
                __import__('deepsleep')
                deepsleep.vcc_low()
    finally:
        # Return 10000 to start again in 10 seconds
        gc.collect()
        return 10000

virtualtimers.new(10000, _vcc_callback, hfpm=True)

del folders, uos
gc.collect(); gc.mem_free()
