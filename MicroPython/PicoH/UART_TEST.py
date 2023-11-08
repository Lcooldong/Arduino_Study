import time
from machine import Pin, UART

uart0 = UART(0, 115200, bits=8, parity=None, stop=1)
# uart = UART(0, baudrate=115200, tx=Pin(0), rx=Pin(1))
# uart.init(bits=8, parity=None, stop=2)
led = Pin("LED", Pin.OUT)
time.sleep(0.5)

rxData = bytes()
while uart0.any() > 0:
    rxData += uart0.read(1)

print(rxData)
#        if data==b'u':
#            led.toggle()
#    time.sleep(1)
    