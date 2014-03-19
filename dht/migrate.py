import time
import threading
import simplerpc
import dht.services
from dht.sharding import SHARD_MIG_OUT

class MigrateQueue(object):

    def __init__(self, shard, dst_addr):
        self.shard = shard
        self.dst_addr = dst_addr
        self.queue = []

    def push_back(self, job):
        # job: ('put', key, value), ('remove', key)
        self.queue += job,

    def send_one_request(self, proxy):
        job = self.queue[0]
        self.queue = self.queue[1:]
        if job[0] == "put":
            key, value = job[1], job[2]
            proxy.sync_put(key, value)
        elif job[0] == "remove":
            key = job[1]
            proxy.sync_put(key, value)


class MigrateThread(threading.Thread):

    def set_service(self, svc):
        self.svc = svc

    def run(self):
        self.pending_migration = []
        self.migration_queue = {} # keyrange => MigrateQueue
        while True:
            time.sleep(1)
            # print "MigrateThread wake up"

            if len(self.pending_migration) > 0:
                shrd, dst_addr = self.pending_migration.pop()
                shrd.set_status(SHARD_MIG_OUT)
                clnt = simplerpc.Client()
                clnt.connect(dst_addr)
                _rng = dht.services._keyrange(low=shrd.keyrange.low, high=shrd.keyrange.high)
                proxy = dht.services._ChordProxy(clnt)
                proxy.sync_start_migration(_rng)
                q = MigrateQueue(shrd, dst_addr)
                self.migration_queue[shrd.get_keyrange()] = q
                for k in shrd.data:
                    q.push_back(('put', k, shrd.data[k]))

            for keyrange in self.migration_queue:
                q = self.migration_queue[keyrange]
                print "doing live migration to %s" % q.dst_addr
                cnt = 0 # live migration rate limit
                clnt = simplerpc.Client()
                clnt.connect(q.dst_addr)
                proxy = dht.services._ChordProxy(clnt)
                while len(q.queue) > 0 and cnt < 100:
                    q.send_one_request(proxy)
                    cnt += 1

            all_keyranges = self.migration_queue.keys()
            for keyrange in all_keyranges:
                q = self.migration_queue[keyrange]
                if len(q.queue) == 0:
                    del self.migration_queue[keyrange]
                    clnt = simplerpc.Client()
                    clnt.connect(q.dst_addr)
                    proxy = dht.services._ChordProxy(clnt)
                    _rng = dht.services._keyrange(low=q.shard.keyrange.low, high=q.shard.keyrange.high)
                    proxy.sync_finish_migration(_rng)
                    self.svc.switch_routing(q.shard.keyrange, q.dst_addr)


    def add_migration(self, shrd, dst_addr):
        print "*** will migrate %s to %s" % (shrd.get_keyrange(), dst_addr)
        self.pending_migration += (shrd, dst_addr),
