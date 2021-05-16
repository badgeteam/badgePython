import machine, system, esp32
import time
import display
import led

display.drawFill(0)
display.flush()
time.sleep(0.4)
display.drawFill(0xFFFFFF)
display.flush()
time.sleep(0.8)
display.drawFill(0)
display.flush()
time.sleep(0.4)

for i in range (0, 3):
    display.drawText(0, 0, '.'*i)
    display.flush()
    time.sleep(0.5)
display.drawText(0, 0, '... starting')
display.flush()

for i in range(0, int(led.NUMEDGE/2)+1):
    # if i > 0:
    #     led1 = (6-(i-1)) % led.NUMEDGE
    #     led2 = 6+(i-1)
    #     led.setEdge(led1, (0, 0, 0))
    #     led.setEdge(led2, (0, 0, 0))
    led1 = (6-i) % led.NUMEDGE
    led2 = 6+i
    led.setEdge(led1, (30, 0, 0))
    led.setEdge(led2, (30, 0, 0))
    time.sleep(0.05)

for i in range(0, led.NUMEDGE):
    led.setEdge(i, (0, 0, 0), False)
led.setEdge(0, (0, 0, 0))
time.sleep(0.4)
for i in range(0, led.NUMEDGE):
    led.setEdge(i, (0, 30, 0), False)
led.setEdge(0, (0, 30, 0))
time.sleep(0.4)
for i in range(0, led.NUMEDGE):
    led.setEdge(i, (0, 0, 0), False)
led.setEdge(0, (0, 0, 0))
time.sleep(0.4)
for i in range(0, led.NUMEDGE):
    led.setEdge(i, (0, 0, 30), False)
led.setEdge(0, (0, 0, 30))
time.sleep(0.4)
for i in range(0, led.NUMEDGE):
    led.setEdge(i, (0, 0, 0), False)
led.setEdge(0, (0, 0, 0))
time.sleep(0.4)

rows = [range(0, 17), range(17, 17+18), range(35, 35+18), range(53, 53+17), range(70, 70+16), range(86, 100)]

for row in rows:
    for i in row:
        led.setKey(i, (30,30,30), flush=False)
    led.keyFlush()
    time.sleep(0.7)
    for i in row:
        led.setKey(i, (0,0,0), flush=False)
    led.keyFlush()

for i in range(0, led.NUMKEYS):
    led.setKey(i, (30,30,30))
time.sleep(0.5)
for i in range(0, led.NUMKEYS):
    led.setKey(i, (0,0,0), flush=False)
led.keyFlush()   
    


text = "Happy\nBirthday"
height = display.getTextHeight(text)
width = display.getTextWidth(text)
offsetx = int((128-width)/2)
offsety = int((64-height)/2)

display.drawFill(0)
display.drawText(offsetx, offsety, text)
display.flush()
time.sleep(5)
##
# Add any hardware checks needed to test the device after initial flashing.
# This runs only once, and disables itself afterwards with the following line:
nvs = esp32.NVS('system')
nvs.set_i32('factory_checked', 3)
nvs.commit()

system.reboot()
