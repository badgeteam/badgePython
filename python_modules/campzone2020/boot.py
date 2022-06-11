import machine, sys, system, time, touchpads
import rtcmem

rtcmem.write(0,0)
rtcmem.write(1,0)

touchpads.on(touchpads.HOME, system.home)

with open('cache/bootreason.txt', 'wa') as file:
	file.write(str(machine.wake_reason()))

#### Application starting ####

# Default app
app = rtcmem.read_string()
if not app:
	#app = machine.nvs_getstr("system", 'default_app')
	#if not app:
    app = 'dashboard.home'

# # Override with special boot mode apps if necessary
# if machine.nvs_getint('system', 'factory_checked') != 2:
# 	# Factory check mode
# 	app = "factory_checks"
# elif machine.nvs_getint('system', 'splash_played') != 1:
# 	# Boot splash screen
# 	app = "bootsplash"

try:
	import sdcard
	sdcard.mountsd()
	del sdcard
except:
	print('Could not mount SD card')
	pass

if app and not app == "shell":
	# try:
	print("Starting app '%s'..." % app)
	system.__current_app__ = app
	if app:
		__import__(app) if not app.endswith('.py') else execfile(app)
	# except Exception as e:
	# 	print(e)
	# 	# if not machine.nvs_get_u8("system", "ignore_crash"):
    #     print("Fatal exception in the running app!")
    #     system.crashedWarning()
    #     time.sleep(3)
    #     system.launcher()