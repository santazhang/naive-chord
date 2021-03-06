#pragma once

#include <string>
#include <vector>

#include <pthread.h>

#include "utils.h"
#include "config.h"
#include "marshal-chord.h"
#include "services.h"
#include "storage.h"

namespace dht {

class Chord: public NoCopy, public ChordService {

    Storage store_;
    host_port me_;
    host_port pred_;
    host_port succ_;
    host_port finger_[Config::m];
    rpc::ClientPool* clnt_;
    SpinLock l_;

    bool stop_flag_;
    pthread_t stabilize_th_;

    Chord(const host_port& me);
    Chord(const host_port& me, const host_port& join_at);

    void stabilize_loop();
    host_port closest_preceding_node(const dht::BigId& id);
    void do_stabilize();
    void do_fix_finger();
    void do_dump_my_info();

public:

    ~Chord();

    virtual void put(const std::string& key, const std::string& value, rpc::DeferredReply* defer);
    virtual void get(const std::string& key, std::string* value, rpc::i8* ok, rpc::DeferredReply* defer);
    virtual void remove(const std::string& key, rpc::i8* ok, rpc::DeferredReply* defer);
    virtual void dump(rpc::DeferredReply* defer);
    virtual void find_successor(const dht::BigId& id, dht::host_port* addr, rpc::DeferredReply* defer);
    virtual void get_predecessor(dht::host_port* addr);
    virtual void notify(const dht::host_port& maybe_pred);
    virtual void ping(const dht::host_port& sender);
    virtual void put_key(const std::string& key, const std::string& value);
    virtual void get_key(const std::string& key, std::string* value, rpc::i8* ok);
    virtual void remove_key(const std::string& key, rpc::i8* ok);
    virtual void dump_info(const std::string& stops_at, rpc::DeferredReply* defer);

    static Chord* create(const host_port& me);
    static Chord* join(const host_port& me, const host_port& join_at);

    static void* start_stabilize_loop(void *);
};

} // namespace dht
