#!/usr/bin/env python

import time
from simplerpc import Server
from dht import ChordService

s = Server()
s.reg_svc(ChordService())
s.start("0.0.0.0:1987")

while True:
    time.sleep(1)
