import dht.services
from dht.keyrange import keyrange
from dht.sharding import Shard, SHARD_READY, SHARD_MIG_IN, SHARD_MIG_OUT
from dht.hashing import dhash
from dht.err_code import ERR_OK, ERR_NOENT, ERR_RETRY

class ChordService(dht.services._ChordService):

    def __init__(self, server_addr, join_addr=None):
        self.server_addr = server_addr
        self.lookup_table = {} # keyrange => server_addr
        self.shard = {} # keyrange => shard
        if join_addr == None:
            # create dht
            rng = keyrange()
            self.lookup_table[rng] = self.server_addr
            self.shard[rng] = Shard(rng)
        else:
            # join dht
            print "TODO This shall be done!"
            print "TODO find server with most shards"

    def _find_shard_by_hash(self, h):
        for rng in self.shard:
            if h in rng:
                return self.shard[rng]

    def fetch_lookup_table(self):
        lkuptbl = {}
        for rng in self.lookup_table:
            _rng = dht.services._keyrange(low=rng.low, high=rng.high)
            lkuptbl[_rng] = self.lookup_table[rng]
        return lkuptbl

    def put(self, key, value):
        err = ERR_OK
        h = dhash(key)
        shard = self._find_shard_by_hash(h)
        if shard is None:
            err = ERR_NOENT
        elif shard.get_status() in [SHARD_READY, SHARD_MIG_IN]:
            shard.put(key, value)
        else:
            # shard.get_status() == SHARD_MIG_OUT, first write to old shard, then to new shard
            print "TODO write to shard with SHARD_MIG_OUT status!"
        return err

    def get(self, key):
        err = ERR_OK
        value = ""
        h = dhash(key)
        shard = self._find_shard_by_hash(h)
        if shard is None:
            err = ERR_NOENT
        elif shard.get_status() in [SHARD_READY, SHARD_MIG_OUT]:
            try:
                value = shard.get(key)
            except KeyError:
                err = ERR_NOENT
        else:
            # SHARD_MIG_IN: if key not found, retry later
            try:
                value = shard.get(key)
            except KeyError:
                err = ERR_RETRY
        return err, value

    def remove(self, key):
        err = ERR_OK
        h = dhash(key)
        shard = self._find_shard_by_hash(h)
        if shard is None:
            err = ERR_NOENT
        elif shard.get_status() in [SHARD_READY, SHARD_MIG_IN]:
            try:
                shard.remove(key)
            except KeyError:
                err = ERR_NOENT
        else:
            # shard.get_status() == SHARD_MIG_OUT, first remove on old shard, then on new shard
            print "TODO remove on shard with SHARD_MIG_OUT status!"
        return err
