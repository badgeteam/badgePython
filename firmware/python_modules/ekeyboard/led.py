import neopixel
import led_keys

led_keys = [0]*100*3
led_edge = [0]*83*3

NUMEDGE = 83
NUMKEYS = 100

def setKey(key, color, flush=True):
    led_keys[key*3] = color[1]
    led_keys[key*3+1] = color[0]
    led_keys[key*3+2] = color[2]
    if flush:
        neopixel.enable(13)
        neopixel.send(bytes(led_keys))
        neopixel.disable()

def keyFlush():
    neopixel.enable(13)
    neopixel.send(bytes(led_keys))
    neopixel.disable()

def setEdge(pos, color, flush=True):
    led_edge[pos*3] = color[1]
    led_edge[pos*3+1] = color[0]
    led_edge[pos*3+2] = color[2]
    if flush:
        neopixel.enable(12)
        neopixel.send(bytes(led_edge))
        neopixel.disable()

def edgeFlush():
    neopixel.enable(12)
    neopixel.send(bytes(led_edge))
    neopixel.disable()