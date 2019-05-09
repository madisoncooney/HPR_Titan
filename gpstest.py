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
import os
import time
import xml.etree.ElementTree as ET
import kmlparser
import random
ET.register_namespace("","http://www.opengis.net/kml/2.2")

timestr = time.strftime("%d%m%Y-%H%M")

# lon, lat, alt
curr_longitude = 145.8347914
curr_latitude = -38.207827
curr_altitude = 200
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

for i in range(10):
    gps_coords = [str(curr_longitude + random.random()/100), str(curr_latitude + random.random()/100), str(curr_altitude + random.random()*100)]
    gps_string = ",".join(gps_coords)  # convert whole thing to string
    kmlparser.parsekml(timestr, gps_coords, tree)
    time.sleep(1)
