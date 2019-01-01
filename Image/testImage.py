import cv2
import numpy as np
from decimal import Decimal

#select video from camera
cam = cv2.VideoCapture(1)

while True:
    ret, img = cam.read()   #read image
    kernel = np.ones((50 ,50),np.uint8)  #ขนาดของสี่เหลี่ยมที่เอามาต่อจุด
    if not ret:
        break
    k = cv2.waitKey(10)

    if k%256 == 27:
        # ESC pressed
        print("Escape hit, closing...")
        break
    
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)  #BGR to HSV
    h, s, v = cv2.split(hsv)    

    _, thre = cv2.threshold(s, 90, 255, cv2.THRESH_BINARY)  #threshold the imgae
    closing = cv2.morphologyEx(thre, cv2.MORPH_CLOSE, kernel)   #expand pixel

    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)    #gray scale
    blur = cv2.blur(gray,(4, 4))     #blur

    #find contours
    im2, contours, hierarchy = cv2.findContours(closing, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE) 
    # cv2.drawContours(img, contours, -1, (0, 0, 255), 3)

    circles = cv2.HoughCircles(blur, cv2.HOUGH_GRADIENT, 2, 1000)    #find circle
    refSize = 3
    radius = 0

    # ensure at least some circles were found
    if circles is not None:
        # convert the (x, y) coordinates and radius of the circles to integers
        circles = np.round(circles[0, :]).astype("int")
    
        # loop over the (x, y) coordinates and radius of the circles
        for (x, y, r) in circles:
            radius = r
            # draw the circle in the output image, then draw a rectangle
            # corresponding to the center of the circle
            cv2.circle(img, (x, y), r, (0, 0, 255), 4)
            cv2.rectangle(img, (x - 5, y - 5), (x + 5, y + 5), (0, 128, 255), -1)

    #find diameter
    d = 2 * radius
            
    #create a box cover each contour
    for i in contours:
        if cv2.contourArea(i) < 1000:    #if area less than 100 continue
            continue
        rect = cv2.minAreaRect(i)   #create the least area of rectangle cover the contour
        box = cv2.boxPoints(rect)   #create a box from rect value
        box = np.int0(box)          #convert the box data to a numpy array
        cv2.drawContours(img,[box],0,(255,0,0),2)       #draw the box

        #draw vertex
        # cv2.putText(img, str(box[0]), tuple(box[0]), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 0), 3)
        # cv2.putText(img, str(box[1]), tuple(box[1]), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 0), 3)
        # cv2.putText(img, str(box[2]), tuple(box[2]), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 0), 3)
        # cv2.putText(img, str(box[3]), tuple(box[3]), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 0, 0), 3)

        #find width
        width1 = ((box[0] + box[1]) / 2)
        width2 = ((box[2] + box[3]) / 2)
        width = np.sqrt((width1[0] - width2[0]) ** 2 + (width1[1] - width2[1]) ** 2)
       # print(width)

        #find height
        height1 = ((box[1] + box[2]) / 2)
        height2 = ((box[3] + box[0]) / 2)
        height = np.sqrt((height1[0] - height2[0]) ** 2 + (height1[1] - height2[1]) ** 2)
       # print(height)

        #find cm
        cmWidth = (width / d) * refSize
        cmHeight = (height / d) * refSize
        cv2.putText(img, str(round(cmWidth, 2)) + " cm.", tuple(box[0]), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 2)
        # cv2.putText(img, str(round(cmHeight, 2)) + " cm.", tuple(box[1]), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 2)
        
    cv2.imshow("foreground",closing)
    cv2.imshow("img2", img)

    # cv2.imshow("H", h)
    # cv2.imshow("S", s)
    # cv2.imshow("V", v)

cam.release()

cv2.destroyAllWindows()