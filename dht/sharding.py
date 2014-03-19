from dht.hashing import dhash

SHARD_READY = 0 # stable, ready for put/get/remove
SHARD_MIG_OUT = 1 # the shard is being migrated out. first write to old shard, then write to new shard
SHARD_MIG_IN = 2 # the shard is being migrated in, if key not found, retry later

class Shard(object):

    def __init__(self, keyrange, status=SHARD_READY):
        self.keyrange = keyrange
        self.status = status
        self.data = {}

    def get_status(self):
        return self.status

    def set_status(self, status):
        self.status = status

    def get_keyrange(self):
        return self.keyrange

    def put(self, key, value):
        self.data[key] = value

    def get(self, key):
        return self.data[key]

    def remove(self, key):
        del self.data[key]

    def split(self):
        rng1, rng2 = self.keyrange.split(2)
        shard1 = Shard(rng1)
        shard2 = Shard(rng2)
        for k in self.data:
            if dhash(k) in rng1:
                shard1.data[k] = self.data[k]
            else:
                shard2.data[k] = self.data[k]
        return shard1, shard2

    def __str__(self):
        return "keyrange: %s, status: %s, data: %s" % (self.keyrange, self.status.__str__(), self.data.__repr__())

    def __repr__(self):
        return self.__str__()