#!/usr/bin/python

# mqtt_cleint

from __future__ import print_function

import sys
import json
import paho.mqtt.client as mqtt


if sys.version_info[:2] < (3, 3):
    old_print = print

    def print(*args, **kwargs):
        flush = kwargs.pop('flush', False)
        old_print(*args, **kwargs)
        if flush:
            file = kwargs.get('file', sys.stdout)
            # Why might file=None? IDK, but it works for print(i, file=None)
            file.flush() if file is not None else sys.stdout.flus
        # end if
    # end def
# end if

HOST = "solasolo.oicp.net"
PORT = 1883


def OnMessage(client, userdata, msg):
    # print(msg.topic + ": " + msg.payload.decode('ascii'))
    text = json.loads(msg.payload.decode('ascii'))
    # print(text['time'])
    print(text['time'], flush=True)
# end OnMessage


def test():
    # print("Connecting to server...")
    client = mqtt.Client()
    client.connect(HOST, PORT, 60)
    # print("Server connected.")

    # print("Subcribe topic...")
    client.subscribe("piday4fun/action", 1)
    client.on_message = OnMessage
    # print("Topic subcribed")

    client.loop_forever()
# end test


if __name__ == '__main__':
    test()
