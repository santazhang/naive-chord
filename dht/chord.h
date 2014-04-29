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
    std::vector<host_port> succ_;

    Chord(const host_port& me);

public:

    static const int R = g_chord_ring_r;
    static Chord* create(const host_port& me);
};

} // namespace dht
