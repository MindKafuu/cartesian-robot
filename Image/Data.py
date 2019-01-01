import cv2
import numpy as np

cv2.namedWindow('image')

def nothing(x):
    pass

cv2.createTrackbar('size','image',0,29000,nothing)
cv2.createTrackbar('SIZE','image',0,29000,nothing)

camera = cv2.VideoCapture(1)
# cv2.namedWindow('image')

while(1):
    ret, frame = camera.read()
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    s = cv2.getTrackbarPos('size','image')
    S = cv2.getTrackbarPos('SIZE','image')

    #####################################################
    kernel = np.ones((3, 3),np.uint8)
    lower_brown = np.array([8, 75,77])
    upper_brown = np.array([31, 127, 149])
    mask = cv2.inRange(hsv, lower_brown, upper_brown)
    _, thre = cv2.threshold(mask, 200, 255, cv2.THRESH_BINARY)
    median = cv2.medianBlur(thre, 7)
    closing = cv2.morphologyEx(median, cv2.MORPH_CLOSE, kernel)
    im2, contours, hierarchy = cv2.findContours(closing, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE) 

    for i in contours:
        if cv2.contourArea(i) < 100:    
            continue
        rect = cv2.minAreaRect(i)  
        box = cv2.boxPoints(rect)  
        box = np.int0(box)

        #find width
        width1 = ((box[0] + box[1]) / 2)
        width2 = ((box[2] + box[3]) / 2)
        width = np.sqrt((width1[0] - width2[0]) ** 2 + (width1[1] - width2[1]) ** 2)

        #find height
        height1 = ((box[1] + box[2]) / 2)
        height2 = ((box[3] + box[0]) / 2)
        height = np.sqrt((height1[0] - height2[0]) ** 2 + (height1[1] - height2[1]) ** 2)

        if((height * width) > 29000):
            cv2.drawContours(frame, [box], 0, (255, 0, 0), 2)     #หินเล็กสุด
        elif((height * width) > 0 and (height * width) < 660):
            cv2.drawContours(frame, [box], 0, (0, 0, 255), 2)     #หินกลาง

    cv2.imshow('frame', frame)
    cv2.imshow('mask', closing)

    # cv2.imshow("Image", frame)
    #####################################################

    if not ret:
        break
    k = cv2.waitKey(1)

    if k%256 == 27:
        print("Closing")
        break
    
camera.release()

cv2.destroyAllWindows()