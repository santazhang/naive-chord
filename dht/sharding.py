INIT_SHARDS = 100

SHARD_READY = 0
SHARD_MIG_OUT = 1
SHARD_MIG_IN = 2

class Shard(object):

    def __init__(self, keyrange):
        self.keyrange = keyrange
        self.status = SHARD_READY
        self.data = {}

    def get_status(self):
        return self.status

    def put(self, key, value):
        self.data[key] = value

    def get(self, key):
        return self.data[key]
