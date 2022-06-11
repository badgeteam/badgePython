import mch22

BTN_HOME=0
BTN_MENU=1
BTN_START=2
BTN_ACCEPT=3
BTN_A=BTN_ACCEPT
BTN_BACK=4
BTN_B=BTN_BACK
BTN_LEFT=8
BTN_PRESS=9
BTN_DOWN=10
BTN_UP=11
BTN_RIGHT=12

__orientation = 0

_callbacks = dict()

def _button_cb(pinstate):	
	global _callbacks
	pin = pinstate >> 1
	state = pinstate & 1
	#print(str(pin) + " "+str(state))
	if pin in _callbacks:
		callback = _callbacks[pin]
		if callable(callback):
			callback(state)
	
def attach(button, callback):
	global _callbacks
	_callbacks[button] = callback
	return True
	
def detach(button):
	_callbacks[button] = None

def detachAll():
	_callbacks[0] = None
	_callbacks[1] = None
	_callbacks[2] = None
	_callbacks[3] = None
	_callbacks[4] = None
	_callbacks[8] = None
	_callbacks[9] = None
	_callbacks[10] = None
	_callbacks[11] = None
	_callbacks[12] = None


def value(gpio):
	return (mch22.buttons() & gpio) > 0

def getCallback(button):
	global _callbacks
	return _callbacks[button]

def __init():
	mch22.set_handler(_button_cb)

__init()

def rotate(value):
    '''
    Change the orientation of the arrow keys (0, 90, 180, 270)
    '''
    global __orientation
    __orientation = value
