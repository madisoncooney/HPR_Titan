from bitstring import BitArray, BitStream
import base64
import datetime

import serial

ser = serial.Serial(port='COM03', baudrate=115200, timeout=1)

# Open port and start writing data
ser.open()

# this needs to be in a loop
packet = ser.read_until(255,6)

output = NF_rawToDatum(packet)

#Testing purposes
print(output.sensor)
print(output.feed)
print(output.time)
print(output.channel)
print(output.data)