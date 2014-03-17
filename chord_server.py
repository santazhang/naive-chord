#!/usr/bin/env python

import sys
import time
from simplerpc import Server
from dht import ChordService

if len(sys.argv) < 2:
    print "usage: %s server_addr [join_addr]" % sys.argv[0]
    exit()
if len(sys.argv) >= 2:
    server_addr = sys.argv[1]
    svc = ChordService(server_addr)
if len(sys.argv) >= 3:
    join_addr = sys.argv[2]
    svc = ChordService(server_addr, join_addr)

s = Server()
s.reg_svc(svc)
s.start(server_addr)

while True:
    time.sleep(1)
