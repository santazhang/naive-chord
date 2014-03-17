import dht.services
from dht.keyrange import keyrange
from dht.sharding import INIT_SHARDS, Shard, SHARD_READY
from dht.hashing import dhash
from dht.err_code import ERR_OK

class ChordService(dht.services._ChordService):

    def __init__(self, server_addr, join_addr=None):
        self.server_addr = server_addr
        self.lookup_table = {} # keyrange => server_addr
        self.shard = {} # keyrange => shard
        if join_addr == None:
            # create dht
            for rng in keyrange().split(INIT_SHARDS):
                self.lookup_table[rng] = self.server_addr
                self.shard[rng] = Shard(rng)
        else:
            print "TODO This shall be done!"
            print "TODO find server with most shards"

    def ping(self):
        print "ping!"


    def put(self, key, value):
        err = ERR_OK
        h = dhash(key)
        shard = None
        for rng in self.shard:
            if h in rng:
                shard = self.shard[rng]
                break
        if shard is None:
            err = ERR_NOENT
        elif shard.get_status() == SHARD_READY:
            shard.put(key, value)
        return err

    def get(self, key):
        err = ERR_OK
        h = dhash(key)
        shard = None
        for rng in self.shard:
            if h in rng:
                shard = self.shard[rng]
                break
        if shard is None:
            err = ERR_NOENT
        elif shard.get_status() == SHARD_READY:
            value = shard.get(key)
        return err, value
