import display, hardware

# Device specific system functions

def configureWakeupSource():
    return False # Not supported

def prepareForSleep():
    pass # Not supported

def prepareForWakeup():
    display.drawFill(0x0000FF)
    display.drawText(10, 10, "Starting BadgePython...", 0xFFFFFF)
    display.flush()

    hardware.enable_power()

    try:
        hardware.mountsd()
    except:
        pass

def showLoadingScreen(app=""):
    display.drawFill(0x0000FF)
    display.drawText(10, 10, "Starting{}...".format("" if app == "" else " " + app), 0xFFFFFF)
    display.flush()

def showMessage(message="", icon=None):
    display.drawFill(0x0000FF)
    display.drawText(10, 10, message, 0xFFFFFF)
    display.flush()
