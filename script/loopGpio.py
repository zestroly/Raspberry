#!/usr/bin/python
import RPi.GPIO as GPIO
import time

import socket

def udpSendData(data):
    RemoteHost='127.0.0.1'
    RemotePort=6677
    udpClient = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udpClient.sendto(data, (RemoteHost,RemotePort))


def trigger_backup(channel):
    data = ''
    if GPIO.input(channel):
        data = '{EVENT:triger,GPIO:%d,VALUE:1}' % channel
    else:
        data = '{EVENT:triger,GPIO:%d,VALUE:0}' % channel
    print(data)
    udpSendData(data);

GPIO.setmode(GPIO.BOARD)
GPIO.setup(11, GPIO.IN)
GPIO.setup(12, GPIO.IN)
GPIO.add_event_detect(11 , GPIO.FALLING, callback=trigger_backup, bouncetime=200)
GPIO.add_event_detect(12 , GPIO.FALLING, callback=trigger_backup, bouncetime=200)

GPIO.setup(15,GPIO.OUT,initial =0)
GPIO.setup(16,GPIO.OUT,initial =0)


print("a simple udp server start.")
HOST = ''
PORT =6678
BUFSIZ = 128
ADDR = (HOST, PORT)
udpServer = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udpServer.bind(ADDR)
while True:
    data, addr = udpServer.recvfrom(BUFSIZ)
    print(addr,":",data)
    udpSendData()
udpServer.close()

