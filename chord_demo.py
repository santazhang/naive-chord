#!/usr/bin/env python

import sys
from simplerpc import Client
from dht import ChordProxy

if len(sys.argv) < 2:
    print "usage: %s server_addr" % sys.argv[0]
    exit()

server_addr = sys.argv[1]

clnt = Client()
clnt.connect(server_addr)
proxy = ChordProxy(clnt)


proxy.sync_ping()
proxy.sync_put("key", "value")
print proxy.sync_get("key")
