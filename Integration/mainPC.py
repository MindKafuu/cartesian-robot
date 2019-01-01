import serial
from struct import *
import cv2
import numpy as np
from matplotlib import pyplot as plt
from math import *
import time
import sys
from easygui import*

cm = 0
ratio = 0
setType = 1
# type = 2
pattern = 1
angle = 0
positionX = 0
positionY = 0

def gui():
    title = " Start with group 14 "
    option = ["Pattern 1","Pattern 2","Pattern 3"]
    option1 = ["Type A","Type B","Type C"]

    button = buttonbox("choose a pattern",title = title, choices = option)

    if button == option[0]:
        button1 = buttonbox("choose a type",title = title, choices = option1)
        if button1 == option1[0]:
            pattern = 1
            type = 1
            SendData(pattern)
            msgbox(msg = "Pattern: 1 \nType: A \n\nSucceeded",title = title)
        elif button1 == option1[1]:
            pattern = 1
            type = 2
            SendData(pattern)
            msgbox(msg = "Pattern: 1 \nType: B \n\nSucceeded",title = title)
        elif button1 == option1[2]:
            pattern = 1
            type = 3
            SendData(pattern)
            msgbox(msg = "Pattern: 1 \nType: C \n\nSucceeded",title = title)
        else:
            sys.exit()
    elif button == option[1]:
        button1 = buttonbox("choose a type",title = title, choices = option1)
        if button1 == option1[0]:
            pattern = 2
            type = 1
            SendData(pattern)
            msgbox(msg = "Pattern: 2 \nType: A \n\nSucceeded",title = title)
        elif button1 == option1[1]:
            pattern = 2
            type = 2
            SendData(pattern)
            msgbox(msg = "Pattern: 2 \nType: B \n\nSucceeded",title = title)
        elif button1 == option1[2]:
            pattern = 2
            type = 3
            SendData(pattern)
            msgbox(msg = "Pattern: 2 \nType: C \n\nSucceeded",title = title)
        else:
            sys.exit()
    elif button == option[2]:
        button1 = buttonbox("choose a type",title = title, choices = option1)
        if button1 == option1[0]:
            pattern = 3
            type = 1
            SendData(pattern)
            msgbox(msg = "Pattern: 3 \nType: A \n\nSucceeded",title = title)
        elif button1 == option1[1]:
            pattern = 3
            type = 2
            SendData(pattern)
            msgbox(msg = "Pattern: 3 \nType: B \n\nSucceeded",title = title)
        elif button1 == option1[2]:
            pattern = 3
            type = 3
            SendData(pattern)
            msgbox(msg = "Pattern: 3 \nType: C \n\nSucceeded",title = title)
        else:
            sys.exit()
    else:
        sys.exit()

def SendData(pattern):
    BAUDRATE = 115200
    PORT = 'COM17'
    pic = serial.Serial()
    pic.baudrate = BAUDRATE
    pic.port = PORT
    pic.timeout = 1
    pic.dtr = 0
    pic.rts = 0

    split_x = pack('h', positionX)
    x_1, x_2 = unpack('>BB', split_x)
    split_y = pack('h', positionY)
    y_1, y_2 = unpack('>BB', split_y)

    check = ((x_1 + x_2 + y_1 + y_2) - (angle + pattern + type + setType)) + 1

    if check < 0:
        check = check * -1

    split_check = pack('h', check)
    check_1, check_2 = unpack('>BB', split_check)

    data = [255, 255, pattern, type, setType, angle, x_2, x_1, y_2, y_1, check_2, check_1]
    array = bytes(data)
    print(pattern)
    
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

camera = cv2.VideoCapture(1)

while(1):
    ret, frame = camera.read()

    #####################################################
    kernel = np.ones((25, 25),np.uint8)
    lower_brown = np.array([8, 75,77])
    upper_brown = np.array([31, 127, 149])

    cv2.circle(frame , (80, 410), 3, (0, 0, 255), 5)
    # cv2.line(frame, (400, 0), (400, 320), (0, 0, 255), 2)
    # cv2.line(frame, (0, 320), (640, 320), (0, 0, 255), 2)

    box_zone = frame[0 : 320, 0 : 342]
    package_zone = frame[0 : 320, 342 : 640]
    waste_zone = frame[320 : 480, 0 : 640]

    #package zone
    gray_package = cv2.cvtColor(package_zone, cv2.COLOR_BGR2GRAY)
    _, thre_package = cv2.threshold(gray_package, 90, 255, cv2.THRESH_BINARY_INV)
    close_package = cv2.morphologyEx(thre_package, cv2.MORPH_CLOSE, kernel)
    img_package, contours_package, hierarchy = cv2.findContours(close_package, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # cv2.imshow('close', thre)

    for i in contours_package:
        if cv2.contourArea(i) < 1000:    
            continue
        rect_package = cv2.minAreaRect(i)  
        box_package = cv2.boxPoints(rect_package)   
        box_package = np.int0(box_package)        
        cv2.drawContours(package_zone, [box_package], 0, (255, 0, 0), 2)   

        if rect_package[1][0] < rect_package[1][1]:
            angle = 90 - rect_package[2]
        else: 
            angle = -rect_package[2]
        angle = 180 - round(angle)
        # print(angle)

        count_package = 0
        hsv = cv2.cvtColor(package_zone, cv2.COLOR_BGR2HSV)
        mask_package = cv2.inRange(hsv, lower_brown, upper_brown)
        # cv2.imshow('package', mask_package)

        for i in range(0, len(mask_package)):
            for j in range(0, len(mask_package[0])):
                if(mask_package[i][j] > 200):
                    count_package = count_package + 1
                    
        ratio = count_package / (len(mask_package) * len(mask_package[0]))
        # print(ratio)

        if ratio > 0 and ratio < 0.01:
            cv2.putText(package_zone, "TypeA", (int(rect_package[0][0] - 50), int(rect_package[0][1] - 50)), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 0), 3)
            type = 1
        elif ratio > 0.011 and ratio < 0.03:
            cv2.putText(package_zone, "TypeB", (int(rect_package[0][0] - 50), int(rect_package[0][1] - 50)), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 0), 3)
            type = 2
        elif ratio > 0.04:
            cv2.putText(package_zone, "TypeC", (int(rect_package[0][0] - 50), int(rect_package[0][1] - 50)), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 0), 3)
            type = 3

        cv2.circle(package_zone , (int(rect_package[0][0]), int(rect_package[0][1])), 3, (0, 0, 255), 3)

        packageX = 640 - (298 - round(rect_package[0][0]))
        packageY = round(rect_package[0][1])

    #box zone
    # gray_box = cv2.cvtColor(box_zone, cv2.COLOR_BGR2GRAY)
    # _, thre_box = cv2.threshold(gray_box, 90, 255, cv2.THRESH_BINARY_INV)
    # close_box = cv2.morphologyEx(thre_box, cv2.MORPH_CLOSE, kernel)
    # img_box, contours_box, hierarchy = cv2.findContours(close_box, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # cv2.imshow('close', close_box)

    # for i in contours_box:
    #     if cv2.contourArea(i) < 1000:    
    #         continue
    #     rect_box = cv2.minAreaRect(i)  
    #     box_box = cv2.boxPoints(rect_box)   
    #     box_box = np.int0(box_box)        
    #     cv2.drawContours(box_zone, [box_box], 0, (255, 0, 0), 2)   
    #     cv2.circle(box_zone , (int(rect_box[0][0]), int(rect_box[0][1])), 3, (0, 0, 255), 3)

    #     # cv2.circle(box_zone , (int(box_box[0][0]), int(box_box[0][1])), 3, (0, 255, 0), 3)
    #     # cv2.circle(box_zone , (int(box_box[1][0]), int(box_box[1][1])), 3, (0, 255, 0), 3)

    #     box_ref = sqrt((int(box_box[0][1]) - int(box_box[0][0]))**2 + (int(box_box[1][1]) - int(box_box[1][0]))**2) 

        # boxX = int(rect_box[0][0])
        # boxY = int(rect_box[0][1])
###################################################################################

        X = 480 - packageY 
        Y = packageX
        positionX = round((X * 41) / 480)
        positionY = round((Y * 41) / 640)
        # print(positionX, positionY)
            
    cv2.imshow('package', package_zone)
    cv2.imshow('box', box_zone)
    cv2.imshow('waste', waste_zone)

    ##################################################

    if not ret:
        break
    k = cv2.waitKey(1)

    if k%256 == 13:
        pattern = 1
        SendData(pattern)
    if k%256 == 103:
        pattern = 2
        SendData(pattern)
    if k%256 == 104:        
        pattern = 3
        SendData(pattern)
    if k%256 == 27:
        print("Closing")
        break

    ##################################################
    
camera.release()

cv2.destroyAllWindows()
