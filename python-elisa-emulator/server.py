#!/usr/bin/python3
from http.server import BaseHTTPRequestHandler,HTTPServer
import time

PORT_NUMBER = 80

#This class will handles any incoming request from
#the browser 
class myHandler(BaseHTTPRequestHandler):
	
	#Handler for the GET requests
	def do_GET(self):
		self.send_response(200)
		self.send_header('Set-Cookie','JSESSIONID=node0k27hwjb5e1f51owbbmureepdq322717.node0;Path=/')
		self.send_header('Cache-Control','no-cache, no-store, must-revalidate, max-age=0, proxy-revalidate, no-transform')
		self.send_header('Pragma','no-cache')
		self.send_header('Content-type','application/xml;charset=utf-8')
		self.send_header('Server','Jetty(9.4.9.v20180320)')
		self.send_header('Content-Length', '156')
		self.end_headers()
		# Send the xml message
		self.wfile.write("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<operator>\n    <regNumber>10069046</regNumber>\n    <name>Tele2 Eesti Aktsiaselts</name>\n</operator>\n".encode('utf-8'))
		time.sleep (0.1);
		return

try:
	#Create a web server and define the handler to manage the
	#incoming request
	server = HTTPServer(('', PORT_NUMBER), myHandler)
	print ('Started httpserver on port ' , PORT_NUMBER)
	
	#Wait forever for incoming htto requests
	server.serve_forever()

except KeyboardInterrupt:
	print ('^C received, shutting down the web server')
	server.socket.close()

