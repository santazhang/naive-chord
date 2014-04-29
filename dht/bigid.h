#pragma once

#include <string>

#include "config.h"

namespace dht {

class BigId {

    unsigned char sha1_[20];

public:
    BigId();
    BigId(const std::string& s);
    BigId(const char* p, int n);

    int cmp(const BigId& o) const;

    static const int M = g_chord_ring_m;

};

inline bool operator == (const BigId& bi1, const BigId& bi2) {
    return bi1.cmp(bi2) == 0;
}
inline bool operator != (const BigId& bi1, const BigId& bi2) {
    return bi1.cmp(bi2) != 0;
}
inline bool operator < (const BigId& bi1, const BigId& bi2) {
    return bi1.cmp(bi2) == -1;
}
inline bool operator > (const BigId& bi1, const BigId& bi2) {
    return bi1.cmp(bi2) == 1;
}
inline bool operator <= (const BigId& bi1, const BigId& bi2) {
    return bi1.cmp(bi2) != 1;
}
inline bool operator >= (const BigId& bi1, const BigId& bi2) {
    return bi1.cmp(bi2) != -1;
}

} // namespace dht
