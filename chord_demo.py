#!/usr/bin/env python

import sys
from dht import ChordClient

if len(sys.argv) < 2:
    print "usage: %s server_addr" % sys.argv[0]
    exit()

server_addr = sys.argv[1]

chord = ChordClient(server_addr)
chord.put("key", "value")
print chord.get("key")
chord.put("key", "new value")
print chord.get("key")
chord.remove("key")
