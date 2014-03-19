import pprint
import simplerpc
import dht.services
from dht.keyrange import keyrange
from dht.sharding import Shard, SHARD_READY, SHARD_MIG_IN, SHARD_MIG_OUT
from dht.hashing import dhash, dhash_to_int, int_to_dhash
from dht.err_code import ERR_OK, ERR_NOENT, ERR_RETRY
from dht.migrate import MigrateThread


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
            # FOR DEBUG
            for i in range(100):
                k = str(i)
                v = str(i)
                self.put(k, v)
        else:
            # join dht
            clnt = simplerpc.Client()
            clnt.connect(join_addr)
            proxy = dht.services._ChordProxy(clnt)
            lkuptbl = proxy.sync_fetch_lookup_table()
            for _rng in lkuptbl:
                rng = keyrange(low=_rng.low, high=_rng.high)
                self.lookup_table[rng] = lkuptbl[_rng]
            max_size_rng = max([(rng.size(), rng) for rng in self.lookup_table.keys()])[1]

            # ask for some data
            svr_holding_max_rng = self.lookup_table[max_size_rng]
            clnt = simplerpc.Client()
            clnt.connect(svr_holding_max_rng)
            proxy = dht.services._ChordProxy(clnt)
            proxy.sync_migrate_some_range(server_addr)

        self.migrate_thread = MigrateThread()
        self.migrate_thread.set_service(self)
        self.migrate_thread.start()


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

    def start_migration(self, _rng):
        err = ERR_OK
        rng = keyrange(low=_rng.low, high=_rng.high)
        self.shard[rng] = Shard(SHARD_MIG_IN)
        orig_rng = None
        for kr in self.lookup_table:
            if kr.include(rng):
                orig_rng = kr
                break

        self.lookup_table[rng] = self.server_addr
        if rng.low == orig_rng.low:
            another_rng = keyrange(low=int_to_dhash(dhash_to_int(rng.high) + 1), high=orig_rng.high)
        elif rng.high == orig_rng.high:
            another_rng = keyrange(low=orig_rng.low, high=int_to_dhash(dhash_to_int(rng.low) - 1))
        self.lookup_table[another_rng] = self.lookup_table[orig_rng]
        del self.lookup_table[orig_rng]

        return err

    def finish_migration(self, _rng):
        err = ERR_OK
        rng = keyrange(low=_rng.low, high=_rng.high)
        self.shard[rng].set_status(SHARD_READY)
        self.lookup_table[rng] = self.server_addr
        return err
    
    def switch_routing(self, keyrange, owner_addr):
        del self.shard[keyrange]
        self.lookup_table[keyrange] = owner_addr

    def debug_print(self):
        print "*** lookup table"
        pprint.pprint(self.lookup_table)
        print
        print "*** shard"
        pprint.pprint(self.shard)
        

    def migrate_some_range(self, dst_addr):
        assert dst_addr != self.server_addr
        err = ERR_OK
        max_size_rng = max([(rng.size(), rng) for rng in self.shard.keys()])[1]
        shard1, shard2 = self.shard[max_size_rng].split()
        del self.lookup_table[max_size_rng]
        self.lookup_table[shard1.get_keyrange()] = self.server_addr
        self.lookup_table[shard2.get_keyrange()] = self.server_addr
        del self.shard[max_size_rng]
        self.shard[shard1.get_keyrange()] = shard1
        self.shard[shard2.get_keyrange()] = shard2
        self.migrate_thread.add_migration(shard2, dst_addr)
        return err

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
        print "get request on key %s" % key
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
