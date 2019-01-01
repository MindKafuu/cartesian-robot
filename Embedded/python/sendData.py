import serial
from struct import *

BAUDRATE = 115200
PORT = 'COM17'
pic = serial.Serial()
pic.baudrate = BAUDRATE
pic.port = PORT
pic.timeout = 1
pic.dtr = 0
pic.rts = 0

mode = 0
type = 1
pattern = 1
theta = 90
positionX = 324
positionY = 422

def sendData(mode):
    if mode == 0:
        check = ((0 + 0 + 0 + 0 + 0) - (mode + type + pattern)) + 1
        if check < 0:
            check = check * -1
        split_check = pack('h', check)
        check_1, check_2 = unpack('>BB', split_check)

        data = [255, 255, mode, type, pattern, 0, 0, 0, 0, 0, check_2, check_1]
        array = bytes(data)
        print(check_1, check_2)
    elif mode == 1:
        split_x = pack('h', positionX)
        x_1, x_2 = unpack('>BB', split_x)
        split_y = pack('h', positionY)
        y_1, y_2 = unpack('>BB', split_y)

        check = ((90 + x_1 + x_2 + y_1 + y_2) - (mode + type + pattern)) + 1
        split_check = pack('h', check)
        check_1, check_2 = unpack('>BB', split_check)

        data = [255, 255, mode, type, pattern, 90, x_2, x_1, y_2, y_1, check_2, check_1]
        array = bytes(data)
        print(check)
    elif mode == 2:
        check = ((theta + 0 + 0 + 0 + 0) - (mode + type + pattern)) + 1
        split_check = pack('h', check)
        check_1, check_2 = unpack('>BB', split_check)

        data = [255, 255, mode, type, pattern, theta, 0, 0, 0, 0, check_2, check_1]
        array = bytes(data)
        print(check)
    
    pic.open()
    pic.write(array)
    print("send")
    pic.flush()

    while True:
        line = pic.readline().decode("utf-8")
        print(line)        
        if line != "end\n":
            print(line)
        else:
            print("Done!")
            break

    pic.close()

sendData(0)