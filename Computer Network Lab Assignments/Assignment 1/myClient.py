import math
import select
import socket
import struct
import time

global start, delay, maxTime, minTime, avgTime, stdTime

#The application layer protocol is defined as follows:
#host = proto[:9]
#port = proto[9:13]
#ID = proto[13:14] --This length can be changed according to the number of ID's we have
#                  --Currently it ranges from 0-9 so only one char is given
#time = proto[14:27]
#The rest of the proto is occupies by the message
def protocol(host, port, iD):
  message = "hello" #This message can be changed to anything you want
  proto = host + str(port) + str(iD) + str(time.time()) + message
  return proto

#The following function makes a new connection to the socket
#It then sends the message to the server and wait for the reply in a given period of time
#If the time exceeds the timeout it assumes that the packet is lost 
#It then sends a "request timed out" message
def ping(host, port, i, timeout = 1):
  try:
    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  except:
    print "Socket creation failed"
  message = protocol(host, port, i)
  timeSent = time.time()
  try:
    client.sendto(message, (host, port))
  except:
    print "Message send failed"
  toReturn = select.select([client], [], [], 1)
  if toReturn[0] == []:
    client.close()
    return "Request timed out."
  
  try:
    data, address = client.recvfrom(1024)
    timeSen = data[14:27]
    delay = time.time() - timeSent
    client.close()
    if delay <= timeout:  
      return delay
  except:
    client.close()
    return "Request timed out."


#Sends 10 pings and calculates the datas
def startPings():
  
  global minTime
  global maxTime
  global missedPackets
  global receivedPackets
  global totalPackets
  global totalTime
  global start
  #The host is pointing to the localHost IP address
  host = '127.0.0.1'
  port = 5000

  for i in range(0,10):
    timeTaken = ping(host, port,i)
    #If the packet is lost
    if timeTaken == "Request timed out.":
      missedPackets += 1
      print timeTaken
    #If the packet is received
    else:
      receivedPackets += 1
      if timeTaken > maxTime:
        maxTime = timeTaken
      if timeTaken < minTime:
        minTime = timeTaken
      totalTime += timeTaken
      delay.append(timeTaken)
      print "Ping from %s: input_seq = %d time = %.5fs"%(host, i, timeTaken)
    totalPackets += 1
  
 #Calculating and printing statistics
  if receivedPackets != 0:
    totalTime = totalTime * 1000 / receivedPackets
  stdDev = 0.0
  for i in delay:
    stdDev = math.pow((i - totalTime), 2)
  stdDev = math.sqrt(stdDev/receivedPackets)

  print "\nSTATISTICS"
  print "Mininmum RTT: %.4fms"%(minTime * 1000)
  print "Maximum RTT: %.4fms"%(maxTime * 1000)
  print "Average RTT: %.4fms"%(totalTime)
  print "Standard deviation of RTT: %.4fms\n"%(stdDev)

  print "Packets received: %d"%(receivedPackets)
  print "Packets lost: %d"%(missedPackets)
  print "Packet loss %.1f%%"%(missedPackets*10)  
    
missedPackets = 0
receivedPackets = 0
totalPackets = 0
delay = []
maxTime = 0
minTime = 999
totalTime = 0
start = time.time()

startPings()
