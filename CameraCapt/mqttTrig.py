import cv2
import numpy as np
import time
import paho.mqtt.client as mqtt
import sys
import os
import subprocess, signal, time

brokerHost = "Your MQTT Server"
port = 1883

topic1 = "pir/pirOn"
topic2 = "gerak/gerak"
topic3 = "finger/found"

subTopic1 = topic1
subTopic2 = topic2
subTopic3 = topic3



def on_connect_on(client, userData, flags, rc):
  print("subs with topic: ", subTopic1)
  clientMqtt.subscribe(subTopic1)

  print("subs with topic: ", subTopic2)
  clientMqtt.subscribe(subTopic2)

  print("subs with topic: ", subTopic3)
  clientMqtt.subscribe(subTopic3)




def on_message_on(client, userData, message):
  receivedMessage = str(message.payload.decode("utf-8"))

  founds = receivedMessage > '0'
  foundGerak = receivedMessage == "gerak_found"
  if (receivedMessage == 'Initializing'):
    print("Failed, Try again")
  elif (foundGerak):
      print(receivedMessage)    
      os.system('python3 camTrig.py')
  elif(receivedMessage == "1"):
    print(receivedMessage)
  elif(receivedMessage == "found_jari_keluar"):
    print("Stopping..")
    os.kill()
  
clientMqtt = mqtt.Client()

def main():
  clientMqtt.on_message = on_message_on
  clientMqtt.on_connect = on_connect_on
  print("connecting to broker: ", brokerHost)
  clientMqtt.connect(brokerHost, port)

  clientMqtt.loop_forever()

if __name__ == "__main__":
    main()
