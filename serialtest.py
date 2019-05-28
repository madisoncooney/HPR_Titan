import serial
ser = serial.Serial(port='COM5', baudrate=115200, timeout=1)

while 1:
    line = ser.readline()
    print(line)

