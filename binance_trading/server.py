import time
from http.server import BaseHTTPRequestHandler, HTTPServer
from binance.client import Client

HOST_NAME = '0.0.0.0'
PORT_NUMBER = 9000

api_key = "ckUg0iSmUGRkev85dM3MrstZcjPhDqvc0znL6PIaMmTf6QZl5ObKpWl0FO9wF1qZ"
api_secret = "vAxzSEpi1QG5OVw67Ivp3IXa7AqDXO1NeW5Q2iHZIlK0XGDug41KX8NkEAGsNVxS"




class MyHandler(BaseHTTPRequestHandler):

    def __init__(self, *args, **kwargs):
        self.binance_client = Client(api_key,api_secret)
        super().__init__(*args, **kwargs)

    def do_HEAD(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def do_GET(self):
        paths = {
            '/foo': {'status': 200},
            '/bar': {'status': 302},
            '/baz': {'status': 404},
            '/qux': {'status': 500}
        }

        if self.path in paths:
            self.respond(paths[self.path])
        else:
            self.respond({'status': 500})

    def handle_http(self, status_code, path):
        self.send_response(status_code)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        result = self.binance_client.get_avg_price(symbol='BTCBUSD')
        price = result['price'].split(".")[0] +'\n'
        return bytes(price, 'UTF-8')

    def respond(self, opts):
        response = self.handle_http(opts['status'], self.path)
        self.wfile.write(response)


if __name__ == '__main__':
    server_class = HTTPServer
    httpd = server_class((HOST_NAME, PORT_NUMBER), MyHandler)
    print(time.asctime(), 'Server Starts - %s:%s' % (HOST_NAME, PORT_NUMBER))
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    print(time.asctime(), 'Server Stops - %s:%s' % (HOST_NAME, PORT_NUMBER))
