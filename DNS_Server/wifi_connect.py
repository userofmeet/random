import network
import time

def connect():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect("Meet Jain", "userofmeet")

    max_attempts = 10
    attempt = 0
    while not wlan.isconnected() and attempt < max_attempts:
        print("Attempting to connect...")
        attempt += 1
        time.sleep(1)

    if wlan.isconnected():
        print("Connected with IP:", wlan.ifconfig()[0])
        return wlan.ifconfig()[0]
    else:
        print("Failed to connect")
        return None

