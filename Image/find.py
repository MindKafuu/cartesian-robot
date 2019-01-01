import cv2
import numpy as np
from matplotlib import pyplot as plt

import time
import serial

def sendData():
    BAUDRATE = 115200
    PORT = 'COM17'
    ser = serial.Serial()
    ser.baudrate = BAUDRATE
    ser.port = PORT
    ser.timeout = 1
    ser.dtr = 0
    ser.rts = 0
    check = type + angle + 1
    data = [255, 255, type, angle, check, 13]
    a = bytearray(data)
    print(a)
    ser.open()
    ser.write(a)
    ser.flush()
    # while True:
    #     line = ser.readline()
    #     if line != b"end\n":
    #         print(line)
    ser.close()

camera = cv2.VideoCapture(0)
# cv2.namedWindow('image')

while(1):
    ret, frame = camera.read()
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    #####################################################
    kernel = np.ones((25, 25),np.uint8)
    lower_brown = np.array([8, 75,77])
    upper_brown = np.array([31, 127, 149])
    count = 0
    type = 0
    angle = 0

    _, thre = cv2.threshold(gray, 80, 255, cv2.THRESH_BINARY_INV)
    close = cv2.morphologyEx(thre, cv2.MORPH_CLOSE, kernel)
    im2, contours, hierarchy = cv2.findContours(close, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    for i in contours:
        if cv2.contourArea(i) < 1000:    
            continue
        rect = cv2.minAreaRect(i)  
        box = cv2.boxPoints(rect)   
        box = np.int0(box)        
        cv2.drawContours(frame,[box],0,(255,0,0),2)   

        if rect[1][0] < rect[1][1]:
            angle = 90 - rect[2]
        else:
            angle = - rect[2]
        #print(angle)
        # cv2.putText(frame, str(round(angle)), (int(rect[0][0] - 50), int(rect[0][1] - 50)), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 0), 3)
        img1 = frame[int(rect[0][1] - rect[1][1] / 2):int(rect[0][1] + rect[1][1] / 2), 
                    int(rect[0][0] - rect[1][0] / 2):int(rect[0][0] + rect[1][0] / 2)]

        if len(img1) > 0 and len(img1[0]) > 0: 
            hsv = cv2.cvtColor(img1, cv2.COLOR_BGR2HSV)
            mask = cv2.inRange(hsv, lower_brown, upper_brown)

            for i in range(0, len(mask)):
                for j in range(0, len(mask[i])):
                    if(mask[i][j] > 200):
                        count = count + 1

            ratio = count / (len(mask) * len(mask[0]))
            # print(ratio)

            if ratio > 0.01 and ratio < 0.08:
                cv2.putText(frame, "TypeA", (int(rect[0][0] - 50), int(rect[0][1] - 50)), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 0), 3)
                type = 1
            elif ratio > 0.08 and ratio < 0.35:
                cv2.putText(frame, "TypeB", (int(rect[0][0] - 50), int(rect[0][1] - 50)), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 0), 3)
                type = 2
            elif ratio > 0.35:
                cv2.putText(frame, "TypeC", (int(rect[0][0] - 50), int(rect[0][1] - 50)), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 0), 3)
                type = 3

            # cv2.imshow("Image1", mask)   
            # cv2.imshow("Image2", img1)
    # print(round(angle))
    angle = round(angle)
            
    # cv2.imshow('mask', close)
    cv2.imshow('frame', frame)
    ##################################################

    if not ret:
        break
    k = cv2.waitKey(1)

    if k%256 == 13:
        sendData()

    if k%256 == 27:
        print("Closing")
        break
    
camera.release()

cv2.destroyAllWindows()