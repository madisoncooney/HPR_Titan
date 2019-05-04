from bitstring import BitArray, BitStream
import base64
import binascii

import serial


ser = serial.Serial(port='COM10', baudrate=115200, timeout=1)

## Open port and start writing data
ser.open()

## this needs to be in a loop
packet = ser.read_until("/n",6)
