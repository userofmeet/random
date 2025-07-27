
import socket

BLOCKED_DOMAINS = [
    b'ads.google.com',
    b'ad.doubleclick.net',
    b'googlesyndication.com',
    b'googleadservices.com',
    b'pagead2.googlesyndication.com',
    b'securepubads.g.doubleclick.net',
    b'analytics.google.com',
    b'tracking.protonmail.ch',
    b'facebookads.com',
    b'connect.facebook.net',
    b'static.doubleclick.net',
    b'adclick.g.doubleclick.net',
    b'app-measurement.com',
    b'ads-twitter.com',
    b'ads.linkedin.com',
    b'stats.g.doubleclick.net',
    b'metrics.apple.com',
    b'telemetry.microsoft.com',
    b'telemetry.appex.bing.net',
    b'telemetry.bing.com',
    b'cdn.adsafeprotected.com',
    b'adservice.google.com',
    b'ads.yahoo.com',
    b'doubleclick.net',
    b'amazon-adsystem.com',
    b'moatads.com',
    b'scorecardresearch.com',
    b'taboola.com',
    b'zedo.com',
    b'criteo.com',
]


def is_blocked(domain):
    for blocked in BLOCKED_DOMAINS:
        if blocked in domain:
            return True
    return False

def start_dns_server():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("", 53))
    print("DNS server started")

    while True:
        data, addr = sock.recvfrom(512)
        domain = data[13:].split(b"\x00")[0]
        print("DNS Request for:", domain)

        if is_blocked(domain):
            print("Blocked:", domain)
            # Send "non-existent domain" response
            response = data[:2] + b"\x81\x83" + data[4:6] + b"\x00\x00\x00\x00\x00\x00"
            sock.sendto(response, addr)
        else:
            # Forward to real DNS
            forward = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            forward.sendto(data, ("1.1.1.1", 53))
            response, _ = forward.recvfrom(512)
            sock.sendto(response, addr)
            forward.close()

