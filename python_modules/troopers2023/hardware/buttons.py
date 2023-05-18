__button_callback = None

def register_callback(callback):
    """
    Register callback which accepts pin and state as parameter    
    """
    global __button_callback
    __button_callback = callback

def value(button):
    """
    Returns the value of a button
    """
    return 0 # to-do

def __callback(pinstate):
    global __button_callback
    if __button_callback != None:
        pin = pinstate >> 1
        state = pinstate & 1
        __button_callback(pin, state)

#mch22.set_handler(__callback)
