#pragma once

#include <string>

#include "config.h"

namespace dht {

class BigId {

    unsigned char sha1_[Config::hash_bytes];

public:
    BigId();
    BigId(const std::string& s);
    BigId(const char* p, int n);

    unsigned char* raw_bytes() {
        return sha1_;
    }

    const unsigned char* raw_bytes() const {
        return sha1_;
    }

    int get_bit(int pos) const;
    int cmp(const BigId& o) const;
    std::string str() const;
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


class BigRange {
    BigId low_;
    BigId high_;

public:
    BigRange(const BigId& low, const BigId& high_inclusive): low_(low), high_(high_inclusive) {}

    bool include(const BigId& id) const;
};

} // namespace dht
