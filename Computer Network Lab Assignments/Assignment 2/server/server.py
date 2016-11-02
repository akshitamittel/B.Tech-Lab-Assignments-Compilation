"""
A program to create a simple HTTP server using TCP sockets and hosting pages in the directory of the server.
"""
import socket
import signal 
import sys
import time   
  
def headers(code):
	"""
	Defining the predefined return headers.
	They are standard. 
	The cases handled are 200(OK) and 404(not found)
	Return the header as a string
	"""
	header = ''
	if code == 404:
		header = 'HTTP/1.1 404 Not Found\n'
	else:
		header = 'HTTP/1.1 200 OK\n'

	current_date = time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime()) 
	header += 'Date: ' + current_date +'\n'
	header += 'Server: akshita\n'
	header += 'Connection: close\n\n'  

	return header

def connect(server):
	"""
	Listen for connections.
	Receive the GET request from the browser.
	If the request is a GET then process it, else ignore it
	the data received contains both the GET and link to be displayed
	If the link is present in the server directory then host it, else throw error
	"""
	while 1:
		server.listen(3)
		print "\nWaiting for a connection..."
		conn, addr = server.accept()
     
		data = conn.recv(1024) 

		#Data of the form "GET /path HTTP/1.1\r\n\r\n"
		#segment[0] == GET
		#segment[1]	== /path
		#segment[2]	== HTTP
		segment = data.split()
		request = segment[0]
		link = segment[1]

		if request == 'GET':
			link = link[1:] 
			print "Serving page: " + link

			#Try to open the path
			#If succesful get corresponding header and append the content
			#Else get the error header and append a self made error page
			#Send the response back through the connection and close it
			try:
				html = open(link, "rb")
				content = html.read()                       
				html.close()                
				response = headers(200)          
                 
			except Exception as e: 
				print "WARNING: File not found. Serving response code 404"
				content = b"<html><head title='ERROR'></head><body><h1>ERROR 404:</h1><h2>File not found</h2></body></html>"
				response = headers(404)  

			response +=  content
			conn.send(response)
			print "Transaction complete."
			conn.close()

		else:
			print "GET request not found"
"""
define the HOST as local host and try using port 80
If port 80 cannot connect use another port (from the ones permitted)
If you still can't connect, quit
Else, take the input from the browser url and host page.
"""
print "HTTP server started..."
HOST = ''
PORT = 80
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try: 
	print "Launching HTTP server on " + str(HOST) + ":80" 
	server.bind((HOST, PORT)) 
         
except Exception as e:
	print "Warning: Could not connect to port 80!"
	PORT = 8000
         
	try:
		print "Launching HTTP server on " + str(HOST) + ":8000"
		server.bind((HOST, PORT))

	except Exception as e:
		print "ERROR: Failed to connect"
		sys.exit(1)

connect(server)