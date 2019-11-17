import cv2
import numpy as np
import time
import paho.mqtt.client as mqtt
import sys

timestr = time.strftime("%Y-%m-%d %H:%M:%S")

cap = cv2.VideoCapture(0)
 
fourcc = cv2.VideoWriter_fourcc(*'H264')
 
if (cap.isOpened() == False): 
  print("Unable to read camera feed")

OUTPUT_FILE = 'video-'+ timestr + '-.mp4'

frame_width = 640
frame_height = 480
 
out = cv2.VideoWriter(OUTPUT_FILE,fourcc, 20.0, (frame_width,frame_height), True)

start_time = time.time()
def recording():
  while(cap.isOpened()):
    ret, frame = cap.read()
    if ret == True: 
      out.write(frame)
      print("Capturing video")
    else:
      break 
  cap.release()
  out.release()
  cv2.destroyAllWindows() 

if __name__ == "__main__":
    recording()
