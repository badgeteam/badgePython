from machine import Pin
import mch22

SPI_ID = 2                       #ESP32 spi bus connected to fpga
PIN_CS = 27                      #GPIO27 on esp32
pin_cs = Pin(PIN_CS, Pin.OUT)
pin_cs.on()

def reset(state):
    """Reset the fpga

    Parameters:
       state (bool): True reset fpga, False release reset

    Returns:
       None
    """
    mch22.fpga_reset(not state)

def chip_select(state):
    """Set CS of the fpga

    Parameters:
       state (bool): True cs high, False cs low

    Returns:
       None
    """
    pin_cs.value(state)

def done_state():
   """Return the state of the ice40 done pin

   Parameters:
      None

   Returns:
      done (bool): True if done is high, False if done is low
   """
   return (mch22.buttons() & (1 << 5)) == 0