import time
import serial

BAUDRATE = 115200
PORT = 'COM17'
ser = serial.Serial()
ser.baudrate = BAUDRATE
ser.port = PORT
ser.timeout = 1
ser.dtr = 0
ser.rts = 0

data = [255, 255, 180, 13]
a = bytearray(data)
print(a)
ser.open()

print(ser.write(a))

ser.flush()

while True:
    line = ser.readline()
    print(line)

ser.close()

