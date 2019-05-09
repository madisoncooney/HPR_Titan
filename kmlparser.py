import os
import xml.etree.ElementTree as ET


def parsekml(timestring, current_gps, tree):
    # register that this XML tree uses the google KML namespace
    ET.register_namespace("","http://www.opengis.net/kml/2.2")
    gps_string = ",".join(current_gps)  # convert whole thing to string

    # access the root of the XML tree
    root = tree.getroot()
    # navigate indices of root to find coordinates
    coordinates_text = root[0][5][2][3].text
    # change coordinates text, adding the current GPS location
    coordinates_text_after = coordinates_text + "\n" + gps_string # + "\n"
    root[0][5][2][3].text = coordinates_text_after
    # accesses the look at section of the KML and tells the camera to look at the most recent GPS coord
    root[0][4][0].text = current_gps[0]
    root[0][4][1].text = current_gps[1]
    root[0][4][2].text = str(float(current_gps[2]) + 200)

    # write new root to tree
    tree = ET.ElementTree(root)

    # write new tree to file
    tree.write(timestring+'.kml', xml_declaration=True, encoding="UTF-8")
    tree.write('NetWork_Current.kml', xml_declaration=True, encoding="UTF-8")

    return
