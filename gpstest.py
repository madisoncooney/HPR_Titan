import serial

# Check your COM port and baud rate
ser = serial.Serial(port='COM8', baudrate=115200, timeout=1)

while 1:
    line = str(ser.readline())
    if line.find('$GPGGA') != -1:  # filter out incomplete (invalid) lines

        line = line.replace("\\r\\n'", '')  # Delete "!ANG:"
        line = line.replace("b'", '')  # Delete "!ANG:"
        # line = line.replace("$GPGGA", '')  # Delete "!ANG:"
        print(line)
        words_GPGGA = line.split(",")
        # print(words_GPGGA[0])
    if line.find('$GPRMC') != -1:  # filter out incomplete (invalid) lines
        # line2 = line.replace("GPRMC", '')
        line = line.replace("b'", '')  # Delete "!ANG:"
        line = line.replace("\\r\\n'", '')
        words_GPRMC = line.split(",")
        print(line)
