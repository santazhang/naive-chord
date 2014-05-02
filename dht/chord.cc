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

    for (int i = 0; i < Config::m; i++) {
        finger_[i] = succ_;
    }

    Pthread_create(&stabilize_th_, nullptr, Chord::start_stabilize_loop, this);
}

Chord::Chord(const host_port& me, const host_port& join_at): me_(me), stop_flag_(false) {
    clnt_ = new rpc::ClientPool;
    ChordProxy proxy(clnt_->get_client(join_at));
    proxy.find_successor(BigId(me), &succ_);
    Log::debug("site %s joined chord ring at %s", me_.c_str(), join_at.c_str());
    Log::debug("site %s has id: %s", me_.c_str(), BigId(me_).str().c_str());

    for (int i = 0; i < Config::m; i++) {
        finger_[i] = succ_;
    }

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
    fu_attr.callback = [defer, this, &key, &value] (Future* cb_fu) {
        string addr;
        cb_fu->get_reply() >> addr;
        Log::debug("redirect put to %s", addr.c_str());
        ChordProxy proxy2(this->clnt_->get_client(addr));
        FutureAttr fu_attr2;
        fu_attr2.callback = [defer] (Future* cb_fu2) {
            defer->reply();
        };
        Future* fu2 = proxy2.async_put_key(key, value, fu_attr2);
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
    fu_attr.callback = [defer, this, &key, value, ok] (Future* cb_fu) {
        string addr;
        cb_fu->get_reply() >> addr;
        Log::debug("redirect get to %s", addr.c_str());
        ChordProxy proxy2(this->clnt_->get_client(addr));
        FutureAttr fu_attr2;
        fu_attr2.callback = [defer, value, ok] (Future* cb_fu2) {
            cb_fu2->get_reply() >> *value >> *ok;
            defer->reply();
        };
        Future* fu2 = proxy2.async_get_key(key, fu_attr2);
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
    fu_attr.callback = [defer, this, &key, ok] (Future* cb_fu) {
        string addr;
        cb_fu->get_reply() >> addr;
        Log::debug("redirect remove to %s", addr.c_str());
        ChordProxy proxy2(this->clnt_->get_client(addr));
        FutureAttr fu_attr2;
        fu_attr2.callback = [defer, ok] (Future* cb_fu2) {
            cb_fu2->get_reply() >> *ok;
            defer->reply();
        };
        Future* fu2 = proxy2.async_remove_key(key, fu_attr2);
        Future::safe_release(fu2);
    };
    Future* fu = proxy.async_find_successor(id_key, fu_attr);
    Future::safe_release(fu);
}


void Chord::dump(rpc::DeferredReply* defer) {
    ScopedLock sl(&l_);
    defer->reply();
    ChordProxy proxy(clnt_->get_client(succ_));
    string stops_at = me_;
    proxy.async_dump_info(stops_at);
}

void Chord::do_dump_my_info() {
    Log::debug("**** BEGIN DUMP ****");
    Log::debug("site %s: pre=%s, succ=%s", me_.c_str(), pred_.c_str(), succ_.c_str());
    if (pred_ != "") {
        Log::debug("range: [%s, %s)", BigId(pred_).str().c_str(), BigId(me_).str().c_str());
    }
    Log::debug("succ range: [%s, %s)", BigId(me_).str().c_str(), BigId(succ_).str().c_str());
    store_.dump();
    Log::debug("****  END  DUMP ****");
}

host_port Chord::closest_preceding_node(const dht::BigId& id) {
    BigRange range(BigId(me_), id);
    for (int i = Config::m - 1; i >= 0; i--) {
        BigId id_finger = finger_[i];
        if (range.include(id_finger) && id_finger != id) {
            return finger_[i];
        }
    }
    return succ_;
}

void Chord::find_successor(const dht::BigId& id, dht::host_port* addr, rpc::DeferredReply* defer) {
    ScopedLock sl(&l_);

    if (BigRange(BigId(me_), BigId(succ_)).include(id)) {
        *addr = succ_;
//        Log::debug("site %s replied find_successor(%s) request ~ [me=%s, succ=%s) -> addr=%s", me_.c_str(), id.str().c_str(),
//                    BigId(me_).str().c_str(), BigId(succ_).str().c_str(), succ_.c_str());
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
//        Log::debug("site %s relay find_successor(%s) request to %s", me_.c_str(), id.str().c_str(), node.c_str());
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
    Log::debug("site %s put %s=%s", me_.c_str(), key.c_str(), value.c_str());
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
    Log::debug("site %s get %s -> %s, err_code=%d", me_.c_str(), key.c_str(), value->c_str(), *ok);
}

void Chord::remove_key(const std::string& key, rpc::i8* ok) {
    ScopedLock sl(&l_);
    if (store_.remove(key)) {
        *ok = 0;
    } else {
        *ok = ENOENT;
    }
    Log::debug("site %s remove %s -> err_code=%d", me_.c_str(), key.c_str(), *ok);
}


void Chord::dump_info(const std::string& stops_at, rpc::DeferredReply* defer) {
    ScopedLock sl(&l_);
    if (me_ != stops_at) {
        ChordProxy proxy(clnt_->get_client(succ_));
        proxy.async_dump_info(stops_at);
    }
    defer->reply();
    do_dump_my_info();
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

void Chord::do_fix_finger() {
    l_.lock();
    ChordProxy proxy(clnt_->get_client(succ_));
    l_.unlock();

    for (int i = 0; i < Config::m; i++) {
        BigId x = BigId(me_).add_power_of_two(i);
        host_port old = finger_[i];
        proxy.find_successor(x, &finger_[i]);
        if (old != finger_[i]) {
            Log::debug("site %s: finger[%d] updated from %s to %s",
                        me_.c_str(), i, old.c_str(), finger_[i].c_str());
        }
    }
}

void Chord::stabilize_loop() {
    while (!stop_flag_) {
        sleep(1);
//        Log::debug("wake up for stabilize");
        do_stabilize();
        do_fix_finger();
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
