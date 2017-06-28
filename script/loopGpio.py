#!/usr/bin/python3
import RPi.GPIO as GPIO
import time
import socket


TriggerList = []


##发送数据

def udpSendData(data):
    RemoteHost='127.0.0.1'
    RemotePort=6677
    udpClient = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udpClient.sendto(data, (RemoteHost,RemotePort))


##触发回调

def trigger_backup(channel):
    data = ''
    if GPIO.input(channel):
        data = '{EVENT:triger,GPIO:%d,VALUE:1}' % channel
    else:
        data = '{EVENT:triger,GPIO:%d,VALUE:0}' % channel
    udpSendData(data);



def SetGPIO(info):
   # print(info) #  '{"Event":"Triger", "Channel":11,"Edge":"Rising","Bouncetime":200}'
    infodict = eval(info)
    if infodict['Event'] == 'Triger':
        channel='Channel%d' % infodict['Channel']
        def getedge(edgestr):
            if edgestr == "Rising":
                return GPIO.RISING
            else:
                return GPIO.FALLING
        edge = getedge(infodict['Edge'])
        bouncetime=infodict['Bouncetime']
        if channel in TriggerList:
            print("modify %s " %channel)
            GPIO.remove_event_detect(infodict['Channel'])
            GPIO.add_event_detect(infodict['Channel'] , edge, callback=trigger_backup, bouncetime=bouncetime)
        else:
            print("add new :%s" % channel)
            GPIO.setup(infodict['Channel'], GPIO.IN)
            GPIO.add_event_detect(infodict['Channel'] , edge, callback=trigger_backup, bouncetime=bouncetime)
            TriggerList.append(channel)


#GPIO.setup(15,GPIO.OUT,initial =0)
#GPIO.setup(16,GPIO.OUT,initial =0)


print("a simple udp server start.")
GPIO.setmode(GPIO.BOARD)
HOST = ''
PORT =6678
BUFSIZ = 128
ADDR = (HOST, PORT)
udpServer = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udpServer.bind(ADDR)
while True:
    data, addr = udpServer.recvfrom(BUFSIZ)
    #print(addr,":",data)
    SetGPIO(data)


udpServer.close()

