import thread
import random
import socket
import sys
import time

#IP address that directs it to the LocalHost 
global HOST, PORT, n 
HOST = '127.0.0.1'
PORT = 5000 
 
# UDP socket
server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
print 'The Socket is created'

# Bind socket a host(local) and port
server.bind((HOST, PORT))
print 'Socket bind complete'

def server_thread(server):
  f = float(sys.argv[1])
  n = int(f/10)
  print "in thread" + str(n)
  i  = 0
  rand = []
  #Finding the packets which have to be rejected
  for j in range(0,n):
    x = random.randint(0,9)
    while x in rand:
      x = random.randint(0,9)
    rand.append(x)

  print rand

  while 1:
    # receive data from client (data, address)
    data, address = server.recvfrom(1024)
  
    if not data: 
      break
     
    reply = data
    print "Data sequence : " + str(i)
    print "Data received : " + reply
    if i not in rand:
      print "Sending packet : " + str(i)
      time.sleep(int(sys.argv[2])*0.001)
      server.sendto(reply , address)
      print "Data sent, data sequence: " + str(i)
    else:
      print "Failed to send packet : " + str(i)
    i +=1
     
try:
  thread.start_new_thread(server_thread, (server,))
except:
  print "Error: Unable to start thread"

while 1:
  pass

server.close()

