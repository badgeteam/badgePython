from machine import Pin
import sdcard

PIN_SDPOWER = 19
PIN_NEOPIXEL =  5

power_pin = Pin(PIN_SDPOWER, Pin.OUT)
led_pin = Pin(PIN_NEOPIXEL, Pin.OUT)

def enable_power():
    global power_pin
    power_pin.on()

def disable_power():
    global power_pin
    power_pin.off()

def mountsd():
    enable_power()
    sdcard.mountsd()