##From read_until sequence from serialcapture.py

#!/usr/bin/env python3
from bitstring import BitArray
import datetime
import serial

class Datum:
    def __init__(self,sensorN,feedN,channelN,valueN,terminatorN):

        self.sensor = sensorN
        self.feed = feedN
        self.time = datetime.datetime.now()

        if channelN % 2 == 1:
            self.data = valueN * -1
        else:
            self.data = valueN

        self.channel = (channelN - 9) - (channelN % 2)
        self.terminator = terminatorN


def NF_rawToDatum(packet):
    ###############################################################
    # separate into nibbles

    BITOFFSET = 0

    # For Base32
    convTable = (
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V')

    packetArray = packet.bin
    
    # CONTROL
    nib_sensor = packetArray[0:5]
    nib_feed = packetArray[5:8]

    # 1
    nib_A1 = packetArray[8:12]
    nib_A2 = packetArray[12:16]

    # 2
    nib_B1 = packetArray[16:20]
    nib_B2 = packetArray[20:24]

    # 3
    nib_C1 = packetArray[24:28]
    nib_C2 = packetArray[28:32]

    # 4
    nib_D1 = packetArray[32:36]
    nib_D2 = packetArray[36:40]

    # TERMINATE
    byt_terminator = packetArray[40:48]

    ###############################################################
    # convert nibbles to plain values

    plain_sensor = convTable[int(nib_sensor, 2)]
    plain_feed = int(nib_feed, 2)

    plain_A1 = int(nib_A1, 2)
    plain_A2 = (int(nib_A2, 2)) - 5

    plain_B1 = int(nib_B1, 2)
    plain_B2 = int(nib_B2, 2)

    plain_C1 = int(nib_C1, 2)
    plain_C2 = int(nib_C2, 2)

    plain_D1 = int(nib_D1, 2)
    plain_D2 = int(nib_D2, 2)

    plain_terminator = int(byt_terminator, 2)

    ###############################################################
    # convert plain values to sensible data

    #see sensible data

    value = int(str(plain_B1) + str(plain_B2) + str(plain_C1) + str(plain_C2) + str(plain_D1) + str(plain_D2)) * 10 ** (plain_A2)

    output = Datum(plain_sensor,plain_feed,plain_A1,value,plain_terminator)

    return output


def NF_bufferToRaw(buffer):

    tempbuffer = buffer
    packetFound = 0
    i = 0

    while packetFound != 1:

        readbyte = (BitArray(bytes=buffer, length=8, offset=i)).uint

        i += 1

        if readbyte == 255:

            packetFound = 1
            output = BitArray(bytes=tempbuffer, length=48, offset=i+7)

    return output

ser = serial.Serial(port='COM3', baudrate=9600)

# this needs to be in a loop
buffer = ser.read(200)
print(str(buffer))

packetproxy = [72,176,18,52,86,255]

packets = NF_bufferToRaw(buffer)

output = NF_rawToDatum(packets)

#Testing purposes
print(output.sensor)
print(output.feed)
print(output.time)
print(output.channel)
print(output.data)

#0x016a2468adfe