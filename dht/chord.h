#pragma once

#include <string>
#include <vector>

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
    rpc::ClientPool* clnt_;

    Chord(const host_port& me);

public:

    ~Chord();

    virtual void put(const std::string& key, const std::string& value, rpc::DeferredReply* defer);
    virtual void get(const std::string& key, std::string* value, rpc::i8* ok, rpc::DeferredReply* defer);
    virtual void remove(const std::string& key, rpc::i8* ok, rpc::DeferredReply* defer);
    virtual void find_successor(const dht::BigId& id, dht::host_port* addr, rpc::DeferredReply* defer);
    virtual void get_predecessor(dht::host_port* addr);
    virtual void notify(const dht::host_port& maybe_pred);
    virtual void ping(const dht::host_port& sender);
    virtual void put_key(const std::string& key, const std::string& value);
    virtual void get_key(const std::string& key, std::string* value, rpc::i8* ok);
    virtual void remove_key(const std::string& key, rpc::i8* ok);

    static Chord* create(const host_port& me);
    static Chord* join(const host_port& me, const host_port& join_at);
};

} // namespace dht
