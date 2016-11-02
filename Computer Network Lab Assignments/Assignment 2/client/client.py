"""
A program to implement a basic client which connects to the browser using TCP sockets.
It then sends a well defined header file to the server to retrieve the page.
From that page, it downloads the first five links and stores them in the local directory. 
"""
import socket
import re
import sys
import os

links = []

def get_links(filename):
    """
    Finds an href that indicates links and extracts those links into a list.
    """
    html = open(filename, "r")
    for line in html:
        if "href" in line:
            #Extracting links using regular expressions.
            #The link would be of the form <a href = "link">
            get = re.compile(r"\".*\"")         
            link = get.search(line).group()
            link = link[1:-1]
            get1 = re.compile(r"[^\/]*\.html")
            link = get1.search(line).group()
            links.append(link)

def get_pages(host, port, path):
    """
    Appending the link to the path and sending a GET request to the server
    The HOST and PORT have been decided beforehand
    Retreive the links and store it in the local directory
    The links have prepended headers that have to be removed
    """
    HOST = host
    PORT = port
    i = 1
    #Getting the parent path
    if not path == "/":
        get = re.compile(r"\/.*\/")
        path = get.search(path).group()

    for link in links:
        #The maximum number of links to be downloaded is 5. 
        #We break out of the loop if it is more than 5.
        if i == 6:
            break
        path1 = path + link
        #Creating a socket connection
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        #Allowing the connection to be reused
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        #Connecting to the host and port
        s.connect((HOST, PORT))
        #Sending GET request
        s.send("GET %s HTTP/1.0\r\n\r\n" % (path1))
        #Receiving the data and storing it in the directory
        data = (s.recv(1000000))
        html_file = open(link, "w")
        delimeter = data.find('\r\n\r\n')
        html_file.write(data[delimeter:])
        #Closing the file and connection
        html_file.close()
        s.close()
        print "file %s downloaded!" %(link)
        i += 1
    print "\n##################"
    print "Download complete!"
    print "##################"

def GET(host, port, path):
    """
    Downloading the requested page and parsing it for links to be called.
    Then we use those links to download them into our local directory
    """
    #The description of the connection is the same as in the get_pages function
    HOST = host
    PORT = port   
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.connect((HOST, PORT))
    s.send("GET %s HTTP/1.0\r\n\r\n" % (path))
    data = (s.recv(1000000))
    html = open("out.html", "w")
    html.write(data)
    s.close()
    print "Requested page Received"
    html.close()
    #Getting all the links in the page into a list
    get_links("out.html")
    #Retreiving those links
    get_pages(host, port, path)

"""
Getting the parameters from the command line sys arguments
"""
GET(str(sys.argv[1]), int(sys.argv[2]), str(sys.argv[3]))