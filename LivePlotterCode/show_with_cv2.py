from threading import Thread
import serial
import numpy as np
import cv2
import time
import math
from stream import DataStream

def dessine_robot(img,cx, cy, fx, fy):
    cv2.circle(img,  (cx,cy), 10, (0,0,255), 2)
    #cv2.arrowedLine(img, (cx,cy), (fx,fy), (0,0,255), 2)
    return img

with serial.Serial('/dev/cu.e-puck2_04020-UART', 19200, timeout=1) as ser:
    #x = ser.read()          # read one byte
    #s = ser.read(10)        # read up to ten bytes (timeout)

       # read a '\n' terminated line
    counter = 0
    all_values = []
    l=ser.readline().decode("utf-8")
    line1 = []
    line2 = []
    Ax = np.array([0 for _ in range(200)])
    Ay = np.array([0 for _ in range(200)])
    img = np.zeros([2500,1500,3])

    previous_x=500
    previous_y=500

    stream = DataStream(ser).start()

    img = np.zeros([1000,1000,3])

    cv2.rectangle(img, (800, 300), (200, 700), (255,0,0), 1)

    while(True):
        #l='update x=-5.373975753 .... y=2.907576560 .... angle=-0.495944738'
        x, y, angle = stream.read()
            

        cx=(int)(500+x*10)
        cy=(int)(500-y*10)

        fx= cx+(int)(15*math.cos(angle))
        fy= cy+(int)(15*math.sin(angle))

        
        cv2.line(img,(previous_x,previous_y),(cx,cy),(0,255,0),2)
        
        final_img = dessine_robot(img.copy(), cx, cy, fx, fy)

        previous_x= cx
        previous_y= cy

        cv2.imshow("image", final_img)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break    
        


