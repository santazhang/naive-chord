#include <errno.h>

#include "chord.h"

namespace dht {

Chord::Chord(const host_port& me): me_(me) {
    succ_.reserve(Chord::R);
    succ_.push_back(me_);
}


void Chord::put(const std::string& key, const std::string& value) {
    store_.put(key, value);
}

void Chord::get(const std::string& key, std::string* value, rpc::i8* ok) {
    if (store_.get(key, value)) {
        *ok = 0;
    } else {
        *ok = ENOENT;
    }
}

void Chord::remove(const std::string& key, rpc::i8* ok) {
    if (store_.remove(key)) {
        *ok = 0;
    } else {
        *ok = ENOENT;
    }
}

void Chord::find_successor(const dht::BigId& id, dht::host_port* addr, rpc::DeferredReply* defer) {
    // TODO
}

void Chord::get_predecessor(dht::host_port* addr) {
    *addr = pred_;
}

void Chord::notify(const dht::host_port& maybe_pred) {
    // TODO
}

void Chord::ping(const dht::host_port& sender) {
    // TODO
}


Chord* Chord::create(const host_port& me) {
    Chord* chord = new Chord(me);
    // TODO
    return chord;
}


Chord* Chord::join(const host_port& me, const host_port& join_at) {
    Chord* chord = new Chord(me);
    // TODO
    return chord;
}

} // namespace dht
