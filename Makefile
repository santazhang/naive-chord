.PHONY: all test

all: dht/services.py

dht/services.py: dht/services.rpc
	../simple-rpc/bin/rpcgen --python dht/services.rpc

test:
	. ./activate.sh
	./test.py
