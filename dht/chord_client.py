import time
import dht.services
from dht.keyrange import keyrange
from dht.hashing import dhash
from dht.err_code import ERR_OK, ERR_NOENT, ERR_RETRY
from simplerpc import Client


class ChordClient(object):

    def __init__(self, server_addr):
        self.rpcclnt = {}
        clnt = self._get_rpcclnt(server_addr)
        self._update_lookup_table(clnt)

    def _get_rpcclnt(self, server_addr):
        if server_addr not in self.rpcclnt:
            clnt = Client()
            clnt.connect(server_addr)
            self.rpcclnt[server_addr] = clnt
        return self.rpcclnt[server_addr]

    def _update_lookup_table(self, clnt):
        self.lookup_table = {}
        self.proxy = {}
        proxy = dht.services._ChordProxy(clnt)
        lkuptbl = proxy.sync_fetch_lookup_table()
        print lkuptbl
        for _rng in lkuptbl:
            rng = keyrange(low=_rng.low, high=_rng.high)
            server_addr = lkuptbl[_rng]
            self.lookup_table[rng] = server_addr
            self.proxy[rng] = dht.services._ChordProxy(self._get_rpcclnt(server_addr))

    def _find_server_proxy_by_hash(self, h):
        for rng in self.proxy:
            if h in rng:
                return self.proxy[rng]

    def put(self, key, value):
        h = dhash(key)
        proxy = self._find_server_proxy_by_hash(h)
        err = proxy.sync_put(key, value)
        return err

    def get(self, key):
        h = dhash(key)
        proxy = self._find_server_proxy_by_hash(h)
        while True:
            err, value = proxy.sync_get(key)
            if err == ERR_OK:
                break
            elif err == ERR_NOENT:
                value = None
                break
            # ERR_RETRY
            time.sleep(0.1)
        if err == ERR_NOENT:
            raise KeyError(key)
        return value

    def remove(self, key):
        h = dhash(key)
        proxy = self._find_server_proxy_by_hash(h)
        err = proxy.sync_remove(key)
        if err == ERR_NOENT:
            raise KeyError(key)
