import serial
import time

PORT = 'COM6'
BAUD = 115200

s = serial.Serial(PORT, BAUD, timeout=0.9)
s.setDTR(False)
while(True):
    response = s.read(255).decode("ascii")
    if(response != ""):
        print(response)