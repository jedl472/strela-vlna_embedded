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

                if b'335aa91a' in msg:
                        message_out = "12959"
                else: 
                        message_out = "n"
                        
                self.wfile.write(bytes(message_out, "utf8"))

with HTTPServer(('192.168.22.222', 80), handler) as server:
    server.serve_forever()