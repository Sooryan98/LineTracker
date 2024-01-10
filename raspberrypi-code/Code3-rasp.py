import time
import RPi.GPIO as pin

import numpy as np
import cv2
from picamera import PiCamera
from lcd_api import LcdApi
from i2c_lcd import I2cLcd
#
I2C_ADDR = 0x27
I2C_NUM_ROWS = 2
I2C_NUM_COLS = 16

led1=24
led2=25
prop=26
c3=19
c3start=13
Kp =0.001
pin.setmode(pin.BCM)
pin.setwarnings(False)
pin.setup(led1,pin.OUT)
pin.setup(led2,pin.OUT)
pin.setup(prop,pin.OUT)
pin.setup(c3,pin.IN)
pin.setup(c3start,pin.IN)

lcd = I2cLcd(1, I2C_ADDR, I2C_NUM_ROWS, I2C_NUM_COLS)
dictionary = cv2.aruco.Dictionary_get(cv2.aruco.DICT_6X6_250)

parameters = cv2.aruco.DetectorParameters_create()
#detector = cv2.aruco.ArucoDetector(daictionary, parameters)
markerlist = []
totallist=[]

def get_id(frame):
  
    target=None
    corners, ids, _ = cv2.aruco.detectMarkers(frame,dictionary,parameters=parameters)

        
    if len(corners) > 0:
        ids = ids.flatten()
      
        for (markerCorner, markerID) in zip(corners, ids):
            if markerID is not None:
                target=markerID                
              

 
                print("Target FOund")
                return target

            else:
                return None

    
def move(img,target_id):
    corners, ids, _ = cv2.aruco.detectMarkers(img,dictionary,parameters=parameters)
    if ids is not None:
        if target_id in ids:
            

            target_idx = np.where(ids == target_id)[0][0]

            target_corners = corners[target_idx][0]
 
            target_center = np.mean(target_corners, axis=0)

            frame_center = np.array([frame.shape[1] / 2, frame.shape[0] / 2])

            error = target_center - frame_center

            correction = Kp * error
            pin.output(led1,pin.LOW)
            pin.output(led2,pin.LOW)
            if abs(error[0]) > img.shape[1] / 10:
                if error[0] > 0:
                    print('Moving forward with correction to the right')
                    pin.output(led1,pin.HIGH)
                    pin.output(led2,pin.LOW)
                    time.sleep(1)
                    
                else:
                    print('Moving backward with correction to the left')
                    pin.output(led2,pin.HIGH)
                    pin.output(led1,pin.LOW)
                    time.sleep(1)
                    
            else:
                print('No correction needed')
                pin.output(led1,pin.LOW)
                pin.output(led2,pin.LOW)

    
def direction(a,b,c):
    ax=a[0]
    ay=a[1]
    bx=b[0]
    by=b[1]
    cx=c[0]
    cy=c[1]
    bx-=ax
    by-=ay
    cx-=ax
    cy-=ay

    cp=int(bx*cy-by*cx)

    return cp
def drawmarkers(corners, ids):
    if len(corners) > 0:
        ids = ids.flatten()
      
        for (markerCorner, markerID) in zip(corners, ids):
            if markerID in totallist:
                pass
            else:
                totallist.append(markerID)
    
    
            if markerID in markerlist:
                
                pass
            else:
                markerlist.append(markerID)
                
    return markerlist
def csketch(gray):
  
    ret , thrash = cv2.threshold(gray,127,255,cv2.THRESH_BINARY)
    cnts , hierarchy = cv2.findContours(thrash, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    if len(cnts)>0:
        for cnt in cnts:
            approx = cv2.approxPolyDP(cnt, 0.04* cv2.arcLength(cnt, True), True)
     
          
            
            if len(approx) == 3 and cv2.contourArea(cnt)>500 and cv2.contourArea(cnt)<2000:

                cv2.drawContours(img,[cnt], -1, (255, 0, 255), 5)

                leftmost = tuple(cnt[cnt[:, :, 0].argmin()][0])
                rightmost = tuple(cnt[cnt[:, :, 0].argmax()][0])
                topmost = tuple(cnt[cnt[:, :, 1].argmin()][0])
                bottommost = tuple(cnt[cnt[:, :, 1].argmax()][0])

                ld=direction(topmost,bottommost,leftmost)
                rd=direction(topmost,bottommost,rightmost)

                if abs(ld)>abs(rd):
                    print("Left")
    #                         return -1
                    pin.output(led1,pin.HIGH)
                    pin.output(led2,pin.LOW)
         
                elif abs(ld)<abs(rd):
                    print("Right")
                    pin.output(led1,pin.LOW)
                    pin.output(led2,pin.HIGH)

                                        
#                 return 1
#                     else:
#                         print("equal")
#    
                
    else:
            pass
        



cap=cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH,640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT,480)

while True:

  
    sucess,img=cap.read()

    imgout = img.copy()
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    if  (pin.input(c3))==0:
        mcorners, mid, rC = cv2.aruco.detectMarkers(gray,dictionary,parameters=parameters)
        result = drawmarkers(mcorners, mid)
        d, nd = 0, 0
        ndt,dt=0,0
        for i in result:
             if i % 2 == 0:
                      nd += 1
                      
                      
             else:
                      d += 1
        for i in totallist:
            if i%2==0:
                ndt+=1
            else:
                dt+=1
        print(f'Numbr of defective: {d}')
        print(f'Non defect :{nd}')
        markerlist=[]
        print(d,nd)
        lcd.move_to(0,0)
        lcd.putstr("ND: "+ str(nd)+ " D: "+ str(d))
        
        lcd.move_to(0,1)
        lcd.putstr(" TND:"+ str(ndt)+ " TD:"+ str(dt))
     
        csketch(gray)
    if( pin.input(c3))==1:
#        
        t=get_id(img)
        if t is not None:
            pin.output(prop,pin.HIGH)
            print("Entererd the loop")

            move(img,t)
            
        else:
            pin.output(prop,pin.LOW)
           
#    cv2.imshow("Extreme Points", img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        
         break
        
        
pin.output(led1,pin.LOW)
pin.output(led1,pin.LOW) 

cap.release()
cv2.destroyAllWindows()

