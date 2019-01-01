import cv2
import numpy as np
cv2.namedWindow('image')

def nothing(x):
    pass

cv2.createTrackbar('lower_R','image',0,255,nothing)
cv2.createTrackbar('lower_G','image',0,255,nothing)
cv2.createTrackbar('lower_B','image',0,255,nothing)
cv2.createTrackbar('upper_R','image',0,255,nothing)
cv2.createTrackbar('upper_G','image',0,255,nothing)
cv2.createTrackbar('upper_B','image',0,255,nothing)

camera = cv2.VideoCapture(1)
# cv2.namedWindow('image')

while(1):
    ret, frame = camera.read()
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    r = cv2.getTrackbarPos('lower_R','image')
    g = cv2.getTrackbarPos('lower_G','image')
    b = cv2.getTrackbarPos('lower_B','image')
    R = cv2.getTrackbarPos('upper_R','image')
    G = cv2.getTrackbarPos('upper_G','image')
    B = cv2.getTrackbarPos('upper_B','image')

    #####################################################
    kernel = np.ones((3, 3),np.uint8)
    lower_brown = np.array([r, g, b])
    upper_brown = np.array([R, G, B])
    mask = cv2.inRange(hsv, lower_brown, upper_brown)
    _, thre = cv2.threshold(mask, 200, 255, cv2.THRESH_BINARY)
    median = cv2.medianBlur(thre, 5)
    closing = cv2.morphologyEx(median, cv2.MORPH_CLOSE, kernel)
    im2, contours, hierarchy = cv2.findContours(closing, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE) 

    cv2.imshow('frame', frame)
    cv2.imshow('mask', closing)
    #####################################################

    if not ret:
        break
    k = cv2.waitKey(1)

    if k%256 == 27:
        print("Closing")
        break
    
camera.release()

cv2.destroyAllWindows()