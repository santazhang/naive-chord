SHARD_READY = 0 # stable, ready for put/get/remove
SHARD_MIG_OUT = 1 # the shard is being migrated out. first write to old shard, then write to new shard
SHARD_MIG_IN = 2 # the shard is being migrated in, if key not found, retry later

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

    def remove(self, key):
        del self.data[key]
