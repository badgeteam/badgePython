import machine, sys, system, time
import _device as device
import rtcmem
import esp32

rtcmem.write(0,0)
rtcmem.write(1,0)

device.prepareForWakeup()

app = rtcmem.read_string()
if not app:
	app = rtcmem.read_string()
	if not app:
		app = 'dashboard.launcher'

if app and not app == "shell":
	try:
		print("Starting app '%s'..." % app)
		system.__current_app__ = app
		if app:
			__import__(app)
	except KeyboardInterrupt:
		system.launcher()
	except BaseException as e:
		sys.print_exception(e)
		ignore = 0
		try:
			ignore = nvs.get_i32("ignore_crash")
		except Exception as e:
			ignore = 0
		if not ignore:
			print("Fatal exception in the running app!")
			system.crashedWarning()
			time.sleep(3)
			system.launcher()
