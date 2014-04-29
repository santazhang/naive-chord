#include "chord.h"

namespace dht {

Chord::Chord(const host_port& me): me_(me) {
    succ_.reserve(Chord::R);
    succ_.push_back(me_);
}

} // namespace dht
