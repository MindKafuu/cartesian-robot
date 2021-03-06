# -*- coding: utf-8 -*-
"""
Created on Tue Aug 21 20:47:48 2018

@author: G_Peav
"""

import serial
import time
import struct

BAUDRATE = 115200
portName = "COM17"
serialDevice = serial.Serial( portName, BAUDRATE, 8, 'N', 1, 0, 0, 0, 0, 0 )
serialDevice.setRTS(0)
serialDevice.setDTR(0)

time.sleep(0.5)
print("ready")
floatData = 12.354
ba = bytearray(struct.pack("f", floatData))
print([ "0x%02x" % b for b in ba ])
Packet = [ 0xFF, 0xFF, 0x01]
str_packet = 'm'
str_packet = str_packet.join( [ chr( c ) for c in Packet ] )

serialDevice.write(str_packet.encode())
serialDevice.write(ba)

arrayData = [chr(252), chr(169), chr(69), chr(65)]
# print("get data = ", struct.unpack('f', struct.pack('4c', *arrayData))[0])
countData = 0
arrayData = []

startTime = time.time()
while ((time.time() - startTime) < 10.0):
    if serialDevice.inWaiting() > 0:
        data = serialDevice.read(1)
        print("data =", ord(data))

serialDevice.close()
print("end")
