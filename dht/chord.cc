#include <errno.h>

#include "base/all.h"
#include "chord.h"

using namespace rpc;

namespace dht {

Chord::Chord(const host_port& me): me_(me), succ_(me), stop_flag_(false) {
    clnt_ = new rpc::ClientPool;

    Pthread_create(&stabilize_th_, nullptr, Chord::start_stabilize_loop, this);
}

Chord::Chord(const host_port& me, const host_port& join_at): me_(me), stop_flag_(false) {
    clnt_ = new rpc::ClientPool;
    ChordProxy proxy(clnt_->get_client(join_at));
    proxy.find_successor(BigId(me), &succ_);

    Pthread_create(&stabilize_th_, nullptr, Chord::start_stabilize_loop, this);
}

Chord::~Chord() {
    stop_flag_ = true;
    Pthread_join(stabilize_th_, nullptr);
    delete clnt_;
}


void Chord::put(const std::string& key, const std::string& value, rpc::DeferredReply* defer) {
    // TODO forward request to correct server
    defer->reply();
}

void Chord::get(const std::string& key, std::string* value, rpc::i8* ok, rpc::DeferredReply* defer) {
    // TODO forward request to correct server
    defer->reply();
}

void Chord::remove(const std::string& key, rpc::i8* ok, rpc::DeferredReply* defer) {
    // TODO forward request to correct server
    defer->reply();
}

host_port Chord::closest_preceding_node(const dht::BigId& id) {
    // TODO
    return succ_;
}

void Chord::find_successor(const dht::BigId& id, dht::host_port* addr, rpc::DeferredReply* defer) {
    // TODO scalable approach
    if (BigRange(BigId(me_), BigId(succ_)).include(id)) {
        *addr = succ_;
        defer->reply();
    } else {
        host_port node = closest_preceding_node(id);
        ChordProxy proxy(clnt_->get_client(node));
        FutureAttr fu_attr;
        fu_attr.callback = [defer, addr] (Future* fu) {
            fu->get_reply() >> *addr;
            defer->reply();
        };
        Future* fu = proxy.async_find_successor(id, fu_attr);
        Future::safe_release(fu);
    }
}

void Chord::get_predecessor(dht::host_port* addr) {
    *addr = pred_;
}

void Chord::notify(const dht::host_port& maybe_pred) {
    if (pred_ == "") {
        pred_ = maybe_pred;
        return;
    }

    BigId id_pred(pred_);
    BigId id_me(me_);
    BigId id_maybe_pred(maybe_pred);

    BigRange range(id_pred, id_me);

    if (range.include(id_maybe_pred) && id_maybe_pred != id_me) {
        pred_ = maybe_pred;
    }
}

void Chord::ping(const dht::host_port& sender) {
    // TODO
}

void Chord::put_key(const std::string& key, const std::string& value) {
    store_.put(key, value);
}

void Chord::get_key(const std::string& key, std::string* value, rpc::i8* ok) {
    if (store_.get(key, value)) {
        *ok = 0;
    } else {
        *ok = ENOENT;
    }
}

void Chord::remove_key(const std::string& key, rpc::i8* ok) {
    if (store_.remove(key)) {
        *ok = 0;
    } else {
        *ok = ENOENT;
    }
}


void Chord::stabilize_loop() {
    while (!stop_flag_) {
        sleep(1);
        Log::debug("wake up for stabilize");
        // TODO: stabilize
        // TODO: fix fingers
        // TODO: check predecessor
    }
}


Chord* Chord::create(const host_port& me) {
    Chord* chord = new Chord(me);
    return chord;
}


Chord* Chord::join(const host_port& me, const host_port& join_at) {
    Chord* chord = new Chord(me, join_at);
    return chord;
}

void* Chord::start_stabilize_loop(void* arg) {
    Chord* chord = (Chord *) arg;
    chord->stabilize_loop();

    pthread_exit(nullptr);
    return nullptr;
}

} // namespace dht
