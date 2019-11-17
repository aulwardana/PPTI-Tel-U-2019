from ftplib import FTP
import time
import paho.mqtt.client as mqtt
import sys
import os
import subprocess, signal, time
import socket
import sys

host = "Your Server"

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_address = (host, 3040)
print("----------------------Sesi MQTT------------------------")
print('connecting to {} port {}'.format(*server_address))
sock.connect(server_address)

port = 1883

topic3 = "finger/found"
topic4 = "status/device"

subTopic3 = topic3
subTopic4 = topic4

def on_connect_on(client, userData, flags, rc):
  print("subs with topic: ", subTopic3)
  clientMqtt.subscribe(subTopic3)

  print("subs with topic: ", subTopic4)
  clientMqtt.subscribe(subTopic4)


def on_message_on(client, userData, message):
  receivedMessage = str(message.payload.decode("utf-8"))
  if(receivedMessage == 'Initializing'):
  	print("Initializing, try again")
  elif(receivedMessage > '0'):
  	dataFinger = receivedMessage
  	print("Sending: " + dataFinger)
  	sock.sendall(dataFinger.encode(encoding="utf-8", errors="strict"))

clientMqtt = mqtt.Client()

def main():
  clientMqtt.on_message = on_message_on
  clientMqtt.on_connect = on_connect_on
  print("connecting to broker: ", host)
  clientMqtt.connect(host, port)

  clientMqtt.loop_forever()

if __name__ == "__main__":
    main()

