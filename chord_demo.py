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

chord.refresh_lookup_table()
chord.debug_print()

for i in range(100):
    print chord.get(str(i))

chord.put("key2", "new value 2")
print chord.get("key2")
