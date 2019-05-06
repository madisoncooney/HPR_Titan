# MinIMU9ArduinoAHRS
# Pololu MinIMU-9 + Arduino AHRS (Attitude and Heading Reference System)

# Copyright (c) 2011 Pololu Corporation.
# http://www.pololu.com/

# MinIMU9ArduinoAHRS is based on sf9domahrs by Doug Weibel and Jose Julio:
# http://code.google.com/p/sf9domahrs/

# sf9domahrs is based on ArduIMU v1.5 by Jordi Munoz and William Premerlani, Jose
# Julio and Doug Weibel:
# http://code.google.com/p/ardu-imu/

# MinIMU9ArduinoAHRS is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by the
# Free Software Foundation, either version 3 of the License, or (at your option)
# any later version.

# MinIMU9ArduinoAHRS is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
# more details.

# You should have received a copy of the GNU Lesser General Public License along
# with MinIMU9ArduinoAHRS. If not, see <http://www.gnu.org/licenses/>.

################################################################################

# This is a test/3D visualization program for the Pololu MinIMU-9 + Arduino
# AHRS, based on "Test for Razor 9DOF IMU" by Jose Julio, copyright 2009.

# This script needs VPython, pyserial and pywin modules

# First Install Python 2.6.4 (Python 2.7 also works)
# Install pywin from http://sourceforge.net/projects/pywin32/
# Install pyserial from http://sourceforge.net/projects/pyserial/files/
# Install VPython from http://vpython.org/contents/download_windows.html

from vpython import *
import serial
import string
import math
# import datetime as dt
# from kivy.app import App
# from kivy.uix.widget import Widget
# import matplotlib
# import matplotlib.pyplot as plt
# import matplotlib.animation as animation

from time import time

grad2rad = 3.141592 / 180.0

# Check your COM port and baud rate
ser = serial.Serial(port='COM8', baudrate=115200, timeout=1)

# Main scene
scene = canvas(title="Pololu MinIMU-9 + Arduino AHRS", resizable=False)
scene.width = 500
scene.y = 200

# scene.range = (1.2, 1.2, 1.2)
# # scene.forward = (0,-1,-0.25)
# scene.forward = vector(1, 0, -0.25)
# scene.up = vector(0, 0, 1)

# Second scene (Roll, Pitch, Yaw)
scene2 = canvas(title='Pololu MinIMU-9 + Arduino AHRS', x=0, y=0, width=600, height=200, center=vector(5, 0, 0), background= color.black, resizable=False)
#scene2.range = (1, 1, 1)

scene2.select()
# Roll, Pitch, Yaw
cil_roll = cylinder(pos=vec(0, 0, 0), axis=vector(0.2, 0, 0), radius=0.1, color=color.red)
cil_roll2 = cylinder(pos=vec(0, 0, 0), axis=vector(-0.2, 0, 0), radius=0.1, color=color.red)
cil_pitch = cylinder(pos=vec(3, 0, 0), axis=vector(0.2, 0, 0), radius=0.1, color=color.green)
cil_pitch2 = cylinder(pos=vec(3, 0, 0), axis=vector(-0.2, 0, 0), radius=0.1, color=color.green)
# cil_course = cylinder(pos=(0.6,0,0),axis=(0.2,0,0),radius=0.01,color=color.blue)
# cil_course2 = cylinder(pos=(0.6,0,0),axis=(-0.2,0,0),radius=0.01,color=color.blue)
arrow_course = arrow(pos=vector(6, -0.5, 0), color=color.cyan, axis=vector(1, 0, 0), shaftwidth=0.1, fixedwidth=2)

# Roll,Pitch,Yaw labels
label(pos=vec(0, 1.4, 0), text="Roll", box=0)
label(pos=vec(3, 1.4, 0), text="Pitch", box=0)
label(pos=vec(6, 1.4, 0), text="Yaw", box=0)
label(pos=vec(9, 1.4, 0), text='Alt', box=0)
label(pos=vec(9, 0.6, 0), text='Lat', box=0)
label(pos=vec(9, 0, 0), text='Lon', box=0)



label(pos=vec(6, 0.5, 0), text="N", box=0, color=color.yellow)
label(pos=vec(6, -1.5, 0), text="S", box=0, color=color.yellow)
label(pos=vec(5, -0.5, 0), text="W", box=0, color=color.yellow)
label(pos=vec(7, -0.5, 0), text="E", box=0, opacity=0, color=color.yellow)

label(pos=vec(6.75, 0.25, 0), height=7, text="NE", box=0, color=color.yellow)
label(pos=vec(5.45, 0.25, 0), height=7, text="NW", box=0, color=color.yellow)
label(pos=vec(6.75, -1.25, 0), height=7, text="SE", box=0, color=color.yellow)
label(pos=vec(5.45, -1.25, 0), height=7, text="SW", box=0, color=color.yellow)


L1 = label(pos=vector(0, 1, 0), text="-", box=0, opacity=0)
L2 = label(pos=vector(3, 1, 0), text="-", box=0, opacity=0)
L3 = label(pos=vector(6, 1, 0), text="-", box=0, opacity=0)
L4 = label(pos=vector(9, 1, 0), text="-", box=0, opacity=0)
L5 = label(pos=vector(9, -0.3, 0), text="-", box=0, opacity=0)
L6 = label(pos=vector(9, -0.9, 0), text="-", box=0, opacity=0)

# Main scene objects
scene.select()
# Reference axis (x,y,z)
arrow(color=color.green, axis=vector(1, 0, 0), shaftwidth=0.02, fixedwidth=1)
arrow(color=color.green, axis=vector(0, 1, 0), shaftwidth=0.02, fixedwidth=1)
arrow(color=color.green, axis=vector(0, 0, 1), shaftwidth=0.02, fixedwidth=1)

# labels
label(pos=vector(0, 0.55, 0.8), text="Pololu MinIMU-9 + Arduino AHRS", box=0, opacity=0)
label(pos=vector(1, 0, 0), text="X", box=0, opacity=0)
label(pos=vector(0, 1, 0), text="Z", box=0, opacity=0)
label(pos=vector(0, 0, 1), text="Y", box=0, opacity=0)

# IMU object
platform = box(length=1, height=1, width=1, color=color.blue)
p_line = box(length=1, height=0.08, width=0.5, color=color.yellow)
plat_arrow = arrow(color=color.green, axis=vector(1, 0, 0), shaftwidth=0.06, fixedwidth=1)

# Plot Test
# Create figure for plotting
# fig = plt.figure()
# ax = fig.add_subplot(1, 1, 1)
# xs = []
# ys = []


f = open("Serial" + str(time()) + ".txt", 'w')

roll = float(0)
pitch = float(0)
yaw = float(0)

while 1:
    line = str(ser.readline())
    if line.find('!ANG:') != -1:  # filter out incomplete (invalid) lines

        line = line.replace("\\r\\n'", '')  # Delete "!ANG:"
        line = line.replace("b'!", '')  # Delete "!ANG:"
        f.write(line + '\n')  # Write to the output log file
        print(line)
        line = line.replace("ANG:", '')  # Delete "!ANG:"
        words_ang = line.split(",")  # Fields split
        if len(words_ang) > 2:

            roll = float(words_ang[0]) * grad2rad
            pitch = float(words_ang[1]) * grad2rad
            yaw = float(words_ang[2]) * grad2rad

            axisx = cos(pitch) * cos(yaw)
            axisy = -cos(pitch) * sin(yaw)
            axisz = sin(pitch)

            upx = sin(roll) * sin(yaw) + cos(roll) * sin(pitch) * cos(yaw)
            upy = sin(roll) * cos(yaw) - cos(roll) * sin(pitch) * sin(yaw)
            upz = -cos(roll) * cos(pitch)

            # axis = vector(axisx, axisy, axisz)
            # up = vector(upx, upy, upz)

            axis = vector(axisy, axisz, axisx)
            up = vector(upy, upz, upx)

            platform.axis = axis
            platform.up = up
            platform.length = 0.05
            platform.width = 0.05

            plat_arrow.axis = axis
            plat_arrow.up = up
            plat_arrow.length = 0.8
            p_line.axis = axis
            p_line.up = up

            cil_roll.axis = vector(1 * cos(roll), 1 * sin(roll), 0)
            cil_roll2.axis = vector(-1 * cos(roll), -1 * sin(roll), 0)
            cil_pitch.axis = vector(1 * cos(pitch), 1 * sin(pitch), 0)
            cil_pitch2.axis = vector(-1 * cos(pitch), -1 * sin(pitch), 0)
            arrow_course.axis = vector(1 * sin(yaw), 1 * cos(yaw), 0)

            L1.text = str(float(words_ang[0]))
            L2.text = str(float(words_ang[1]))
            L3.text = str(float(words_ang[2]))

    if line.find('AN:') != -1:  # filter out incomplete (invalid) lines
        # could probably replace this section with a simple function
        line2 = line.replace("b'", '')
        line2 = line2.replace("\\r\\n'", '')
        f.write(line2 + '\n')  # Write to the output log file
        print(line2)

    if line.find('DCM:') != -1:  # filter out incomplete (invalid) lines
        line3 = line.replace("b'", '')
        line3 = line3.replace("\\r\\n'", '')
        f.write(line3 + '\n')  # Write to the output log file
        print(line3)

    if line.find('PRS:') != -1:  # filter out incomplete (invalid) lines
        line4 = line.replace("b'", '')
        line4 = line4.replace("\\r\\n'", '')
        f.write(line4 + '\n')  # Write to the output log file
        print(line4)
        line4 = line4.replace("PS:", '')  # Delete "PS:"
        words_prs = line4.split(",")  # Fields split
        L4.text = str(float(words_prs[1])) + str("m")
    if line.find('GPS:') != -1:
        line5 = line.replace("b'", '')
        line5 = line5.replace("\\r\\n'", '')
        f.write(line5 + '\n')  # Write to the output log file
        print(line5)
        words_gps = line5.split(",")  # Fields split
        L5.text = str(float(words_gps[2]))
        L6.text = str(float(words_gps[1]))
ser.close
f.close
