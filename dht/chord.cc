#include <errno.h>

#include "base/all.h"
#include "chord.h"

using namespace std;
using namespace rpc;

namespace dht {

Chord::Chord(const host_port& me): me_(me), succ_(me), stop_flag_(false) {
    clnt_ = new rpc::ClientPool;
    Log::debug("site %s created new chord ring", me_.c_str());
    Log::debug("site %s has id: %s", me_.c_str(), BigId(me_).str().c_str());

    Pthread_create(&stabilize_th_, nullptr, Chord::start_stabilize_loop, this);
}

Chord::Chord(const host_port& me, const host_port& join_at): me_(me), stop_flag_(false) {
    clnt_ = new rpc::ClientPool;
    ChordProxy proxy(clnt_->get_client(join_at));
    proxy.find_successor(BigId(me), &succ_);
    Log::debug("site %s joined chord ring at %s", me_.c_str(), join_at.c_str());
    Log::debug("site %s has id: %s", me_.c_str(), BigId(me_).str().c_str());

    Pthread_create(&stabilize_th_, nullptr, Chord::start_stabilize_loop, this);
}

Chord::~Chord() {
    stop_flag_ = true;
    Pthread_join(stabilize_th_, nullptr);
    delete clnt_;
}


void Chord::put(const std::string& key, const std::string& value, rpc::DeferredReply* defer) {
    // forward request to correct server
    ChordProxy proxy(clnt_->get_client(me_));
    BigId id_key(key);
    FutureAttr fu_attr;
    fu_attr.callback = [defer, this, &key, &value] (Future* fu) {
        string addr;
        fu->get_reply() >> addr;
        ChordProxy proxy2(clnt_->get_client(addr));
        FutureAttr fu_attr2;
        fu_attr2.callback = [defer] (Future* fu) {
            defer->reply();
        };
        Future* fu2 = proxy2.async_put(key, value);
        Future::safe_release(fu2);
    };
    Future* fu = proxy.async_find_successor(id_key, fu_attr);
    Future::safe_release(fu);
}

void Chord::get(const std::string& key, std::string* value, rpc::i8* ok, rpc::DeferredReply* defer) {
    // forward request to correct server
    ChordProxy proxy(clnt_->get_client(me_));
    BigId id_key(key);
    FutureAttr fu_attr;
    fu_attr.callback = [defer, this, &key, value, ok] (Future* fu) {
        string addr;
        fu->get_reply() >> addr;
        ChordProxy proxy2(clnt_->get_client(addr));
        FutureAttr fu_attr2;
        fu_attr2.callback = [defer, value, ok] (Future* fu) {
            fu->get_reply() >> *value >> *ok;
            defer->reply();
        };
        Future* fu2 = proxy2.async_get(key);
        Future::safe_release(fu2);
    };
    Future* fu = proxy.async_find_successor(id_key, fu_attr);
    Future::safe_release(fu);
}

void Chord::remove(const std::string& key, rpc::i8* ok, rpc::DeferredReply* defer) {
    // forward request to correct server
    ChordProxy proxy(clnt_->get_client(me_));
    BigId id_key(key);
    FutureAttr fu_attr;
    fu_attr.callback = [defer, this, &key, ok] (Future* fu) {
        string addr;
        fu->get_reply() >> addr;
        ChordProxy proxy2(clnt_->get_client(addr));
        FutureAttr fu_attr2;
        fu_attr2.callback = [defer, ok] (Future* fu) {
            fu->get_reply() >> *ok;
            defer->reply();
        };
        Future* fu2 = proxy2.async_remove(key);
        Future::safe_release(fu2);
    };
    Future* fu = proxy.async_find_successor(id_key, fu_attr);
    Future::safe_release(fu);
}

host_port Chord::closest_preceding_node(const dht::BigId& id) {
    // TODO scalable approach
    return succ_;
}

void Chord::find_successor(const dht::BigId& id, dht::host_port* addr, rpc::DeferredReply* defer) {
    ScopedLock sl(&l_);

    if (BigRange(BigId(me_), BigId(succ_)).include(id)) {
        *addr = succ_;
        defer->reply();
        Log::debug("site %s replied find_successor(%s) request ~ [%s, %s)", me_.c_str(), id.str().c_str(),
                    BigId(pred_).str().c_str(), BigId(me_).str().c_str());
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
        Log::debug("site %s relay find_successor(%s) request to %s", me_.c_str(), id.str().c_str(), node.c_str());
    }
}

void Chord::get_predecessor(dht::host_port* addr) {
    ScopedLock sl(&l_);

//    Log::debug("site %s has predecessor: %s", me_.c_str(), pred_.c_str());
    *addr = pred_;
}

void Chord::notify(const dht::host_port& maybe_pred) {
    ScopedLock sl(&l_);

    if (pred_ == "") {
        Log::debug("site %s set pred to %s (was NIL)", me_.c_str(), maybe_pred.c_str());
        pred_ = maybe_pred;
        Log::debug("site %s now in charge of [%s, %s)", me_.c_str(), BigId(pred_).str().c_str(), BigId(me_).str().c_str());
        return;
    }

    BigId id_pred(pred_);
    BigId id_me(me_);
    BigId id_maybe_pred(maybe_pred);

    BigRange range(id_pred, id_me);

    if (range.include(id_maybe_pred) && id_maybe_pred != id_me) {
        Log::debug("site %s update pred to %s (was %s)", me_.c_str(), maybe_pred.c_str(), pred_.c_str());
        pred_ = maybe_pred;
        Log::debug("site %s now in charge of [%s, %s)", me_.c_str(), BigId(pred_).str().c_str(), BigId(me_).str().c_str());
    }
}

void Chord::ping(const dht::host_port& sender) {
    // TODO
}

void Chord::put_key(const std::string& key, const std::string& value) {
    ScopedLock sl(&l_);
    store_.put(key, value);
}

void Chord::get_key(const std::string& key, std::string* value, rpc::i8* ok) {
    ScopedLock sl(&l_);
    if (store_.get(key, value)) {
        *ok = 0;
    } else {
        *ok = ENOENT;
    }
}

void Chord::remove_key(const std::string& key, rpc::i8* ok) {
    ScopedLock sl(&l_);
    if (store_.remove(key)) {
        *ok = 0;
    } else {
        *ok = ENOENT;
    }
}


void Chord::do_stabilize() {
    l_.lock();
    ChordProxy proxy(clnt_->get_client(succ_));
    l_.unlock();

    host_port x;
    proxy.get_predecessor(&x);

    if (x != "") {
        BigId id_x(x);
        BigId id_me(me_);
        BigId id_succ(succ_);

        BigRange range(id_me, id_succ);
        if (range.include(id_x) && id_x != id_succ) {
            succ_ = x;
        }
    }

    l_.lock();
    ChordProxy proxy2(clnt_->get_client(succ_));
    l_.unlock();

    proxy2.async_notify(me_);
}

void Chord::stabilize_loop() {
    while (!stop_flag_) {
        sleep(1);
//        Log::debug("wake up for stabilize");
        do_stabilize();
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
