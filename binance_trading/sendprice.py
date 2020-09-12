from binance.client import Client
import time
import urllib3

api_key = "ckUg0iSmUGRkev85dM3MrstZcjPhDqvc0znL6PIaMmTf6QZl5ObKpWl0FO9wF1qZ"
api_secret = "vAxzSEpi1QG5OVw67Ivp3IXa7AqDXO1NeW5Q2iHZIlK0XGDug41KX8NkEAGsNVxS"


def do_send(ip):
    binance_client = Client(api_key, api_secret)
    while True:
        result = binance_client.get_avg_price(symbol='BTCBUSD')
        btcprice = result['price'].split(".")[0]
        send_data = ip + "/price=" + btcprice + "="
        print(send_data)
        http = urllib3.PoolManager()
        r = http.request('GET', send_data)
        print(r.data)
        time.sleep(5)


if __name__ == "__main__":
    serverIP = "http://192.168.0.30"
    do_send(serverIP)
