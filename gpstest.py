import serial

# Check your COM port and baud rate
#ser = serial.Serial(port='COM8', baudrate=115200, timeout=1)

# while 1:
#     line = str(ser.readline())
#     if line.find('$GPGGA') != -1:  # filter out incomplete (invalid) lines
#
#         line = line.replace("\\r\\n'", '')  # Delete "!ANG:"
#         line = line.replace("b'", '')  # Delete "!ANG:"
#         # line = line.replace("$GPGGA", '')  # Delete "!ANG:"
#         print(line)
#         words_GPGGA = line.split(",")
#         # print(words_GPGGA[0])
#     if line.find('$GPRMC') != -1:  # filter out incomplete (invalid) lines
#         # line2 = line.replace("GPRMC", '')
#         line = line.replace("b'", '')  # Delete "!ANG:"
#         line = line.replace("\\r\\n'", '')
#         words_GPRMC = line.split(",")
#         print(line)

import time
import xml.etree.ElementTree as ET
import kmlparser

curr_longitude = 0
curr_latitude = 0
curr_altitude = 0
ET.register_namespace("","http://www.opengis.net/kml/2.2")
timestr = time.strftime("%d%m%Y-%H%M")
f = open("Serial-" + timestr + ".txt", 'w')
f.write("DMY-HMS,altitude(cm),latitude 10sig, longitude 10sig, speed (mkn), heading 100ths deg\n")
print("DMY-HMS,altitude(cm),latitude 10sig, longitude 10sig, speed (mkn), heading 100ths deg")
# lon, lat, alt

ser = serial.Serial(port='COM12', baudrate=115200, timeout=1)

while curr_latitude == 0:
    line = str(ser.readline())
    if line.find('GPS:') != -1:
        line = line.replace("b'GPS:", '')
        # line = line.replace(",\n'", '')
        f.write(line + '\n')  # Write to the output log file

        words_gps = line.split(",")  # Fields split
        if len(words_gps) == 6:
            if int(words_gps[2]) != 0:
                print(line)
                curr_latitude = int(words_gps[2])/10000000  # lat
                curr_longitude = int(words_gps[3])/10000000  # long
                curr_altitude = int(words_gps[1])/100  # altitude
                break

gps_coords = [str(curr_longitude), str(curr_latitude), str(curr_altitude)]
gps_string = ",".join(gps_coords) # convert whole thing to string


kmlstring = "<?xml version='1.0' encoding='UTF-8'?>" \
             '<kml xmlns="http://www.opengis.net/kml/2.2">' \
             '<Document>' \
             '<name>Flight Path</name>' \
             '<snippet>' \
             'Created with the MCW HPR KML Creator v1.00 at 8/05/2019 12:17:06 AM' \
             '</snippet>' \
             '<Style id="multitrack">' \
             '<IconStyle><Icon /></IconStyle>' \
             '<LabelStyle><scale>0</scale></LabelStyle>' \
             '</Style>' \
             '<Style id="fs_1_Split">' \
             '<LineStyle>' \
             '<color>FFFFFFFF</color>' \
             '<width>4</width>' \
             '</LineStyle>' \
             '<PolyStyle>' \
             '<color>FF0000FF</color>' \
             '</PolyStyle>' \
             '</Style>' \
             '<LookAt>' \
             '<longitude>' + gps_coords[0] + '</longitude>' \
             '<latitude>' + gps_coords[1] + '</latitude>' \
             '<altitude>' + gps_coords[2] + '</altitude>' \
             '<range>1000</range>' \
             '<tilt>45</tilt>' \
             '</LookAt>' \
             '<Placemark>' \
             '<name>CurrentLoc</name>' \
             '<styleUrl>#fs_1_Split</styleUrl>' \
             '<LineString>' \
             '<extrude>1</extrude>' \
             '<tessellate>1</tessellate>' \
             '<altitudeMode>absolute</altitudeMode>' \
             '<coordinates>' \
             + gps_string + \
             '</coordinates>' \
             '</LineString>' \
             '</Placemark>' \
             '</Document>' \
             '</kml>'

tree = ET.ElementTree(ET.fromstring(kmlstring))
tree.write(timestr+'.kml', xml_declaration=True, encoding="UTF-8")
tree.write('NetWork_Current.kml', xml_declaration=True, encoding="UTF-8")

# for i in range(10):


while 1:
    line = str(ser.readline())

    if line.find('GPS:') != -1:
        line = line.replace("b'GPS:", '')
        # line = line.replace(",\n'", '')
        f.write(line + '\n')  # Write to the output log file
        print(line)
        words_gps = line.split(",")  # Fields split
        if len(words_gps) == 6:
            curr_latitude = int(words_gps[2])/10000000  # lat
            curr_longitude = int(words_gps[3])/10000000  # long
            curr_altitude = int(words_gps[1])/100  # altitude
            gps_coords = [str(curr_longitude), str(curr_latitude), str(curr_altitude)]
            kmlparser.parsekml(timestr, gps_coords, tree)
