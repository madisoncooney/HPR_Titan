import os
import lxml
import ast
import xml.etree.ElementTree as ET
import pykml
from pykml import parser

kml_file = os.path.join(
    "E:\My Documents", \
    "All Mitch's Stuff", \
    "University", \
    "HPR", \
    "HPR_Titan", \
    "testkml2.kml")


#with open(kml_file) as f:
#    root = parser.parse(f).getroot()
#print(root.Document.Placemark.LineString.coordinates)
    #print(root.Document)
    #print(schema_gx.validate(root))

#coordinates_before = root.Document.Placemark.LineString.coordinates
#coordinates_after = coordinates_before + "1,1,1"
#print(coordinates_after)
#print(coordinates_before)
gps_coords = [str(145.072693), str(-37.866852), str(200)]
myString = ",".join(gps_coords)


ET.register_namespace("","http://www.opengis.net/kml/2.2")
ET.register_namespace("gx","http://www.google.com/kml/ext/2.2")

tree = ET.parse(kml_file)
root = tree.getroot()

#print(root.Document.Placemark.LineString.coordinates.text)

#for Document in root:
 #   for Placemark in Document:
 #       for LineString in Placemark:
#            for coordinates in LineString:

coordinates_text = root[0][5][2][3].text
coordinates_text_after = coordinates_text + myString + "\n"
print(coordinates_text_after)
root[0][5][2][3].text = coordinates_text_after
                #coordinates.text = coordinates_text_after


    #if Document.find('Placemark'):
    #    coordinates = Document.find('Placemark') #.find('LineString').find('coordinates')
     #   print("hi")

tree = ET.ElementTree(root)
tree.write('kmlwritetest.kml', xml_declaration=True, encoding="UTF-8")