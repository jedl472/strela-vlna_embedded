
from http.server import BaseHTTPRequestHandler, HTTPServer

class handler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-type','text/html')
        self.end_headers()

        message = "Hello, World! Here is a GET response"
        self.wfile.write(bytes(message, "utf8"))
    def do_POST(self):
        self.send_response(200)
        self.send_header('Content-type','text/html')
        self.end_headers()
	
        msg = self.rfile.read(int(self.headers.get("Content-Length")))
        print(len(msg), msg)
        message_out = ""

        if msg == b'a':
                message_out = "Server received A from host " + self.headers.get("Host")
        elif msg == b'b':
                message_out = "Server received B from host " + self.headers.get("Host")
        elif msg == b'12':
                message_out = "Server received 12 from host " + self.headers.get("Host")
        else: message_out = "Unknown message from host " + self.headers.get("Host") + ":" + str(msg)
        
        self.wfile.write(bytes(message_out, "utf8"))

with HTTPServer(('192.168.226.52', 80), handler) as server:
    server.serve_forever()
