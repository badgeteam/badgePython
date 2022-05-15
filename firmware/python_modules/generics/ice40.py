import hardware.ice40 as hwice40
from machine import SPI
from time import sleep
from io import FileIO

TRANSFER_SIZE=256
TRANSFER_RATE=10000000

def load_bitstream(bitstream):
    filestream = None
    if type(bitstream) == str:
        filestream = open(bitstream, "rb")
    elif type(bitstream) == FileIO:
        filestream = bitstream
    else:
        raise ValueError("bitstream must be filename name or file handle")
    
    spi = SPI(hwice40.SPI_ID, TRANSFER_RATE)
    
    hwice40.chip_select(False)
    hwice40.reset(True)
    sleep(0.1)
    hwice40.reset(False)
    hwice40.chip_select(True)
    spi.write(b"\x00")
    hwice40.chip_select(False)    
    sleep(0.1)
    if hwice40.done_state():
        print("Error resetting fpga")
        return False
    while True:
        bitstreampart = filestream.read(TRANSFER_SIZE)  #TODO: change to readinto to resuse single buffer
        if not bitstreampart:
            break
        spi.write(bitstreampart)
    hwice40.chip_select(True)
    spi.write(bytes(10*[0]))
    spi.deinit()
    del(spi)
    return hwice40.done_state()