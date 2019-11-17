import cv2
import numpy as np
import time
import paho.mqtt.client as mqtt
import sys
import os
from subprocess import check_call

brokerHost = "Your Server"
port = 1883

topic4 = "finger/found_out"

subTopic4 = topic4

def on_connect_on(client, userData, flags, rc):
  print("connect with: "+str(rc))
  print("subs with topic: ", subTopic4)
  clientMqtt.subscribe(subTopic4)

def on_message_on(client, userData, message):
  receivedMessage = str(message.payload.decode("utf-8"))
  if (receivedMessage == 'Initializing'):
    print("Error, Try again")
  elif(receivedMessage == "found_jari_keluar"):
    time.sleep(10)
    files = [f for f in os.listdir('.') if os.path.isfile(f)]
    try:
        for f in files:
            fname, ext = os.path.splitext(f)
            if (ext == '.mp4'):
                os.system("ffmpeg -i  -vcodec libx264 ")
        if ct > 0:
            print('Upload Success!\n' + str(ct) + ' files uploaded')
        else:
            print('No file uploaded')
    except Exception as e:
        print(e)
    
    print("received message = " , receivedMessage)

clientMqtt = mqtt.Client()

def destroyS():
  sys.exit()

def main():
  clientMqtt.on_message = on_message_on
  clientMqtt.on_connect = on_connect_on
  print("connecting to broker: ", brokerHost)
  clientMqtt.connect(brokerHost, port)

  clientMqtt.loop_forever()

if __name__ == "__main__":
    main()
