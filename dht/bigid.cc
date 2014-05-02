#include <string>

#include <string.h>

#include "bigid.h"
#include "utils.h"
#include "sha1.h"

using namespace std;

namespace dht {

BigId::BigId() {
    memset(sha1_, 0, arraysize(sha1_));
}

BigId::BigId(const std::string& s) {
    sha1::calc(&s[0], s.size(), sha1_);
}

BigId::BigId(const char* p, int n) {
    sha1::calc(p, n, sha1_);
}

int BigId::get_bit(int pos) const {
    int slot = pos / 8;
    int shift = pos % 8;
    return (sha1_[slot] >> shift) & 0x1;
}

void BigId::set_bit(int pos) {
    int slot = pos / 8;
    int shift = pos % 8;
    sha1_[slot] |= (0x1 << shift);
}

void BigId::unset_bit(int pos) {
    int slot = pos / 8;
    int shift = pos % 8;
    sha1_[slot] &= ~(0x1 << shift);
}

int BigId::cmp(const BigId& o) const {
    for (int i = 0; i < Config::m; i++) {
        if (get_bit(i) < o.get_bit(i)) {
            return -1;
        } else if (get_bit(i) > o.get_bit(i)) {
            return 1;
        }
    }
    return 0;
}

string BigId::str() const {
    string s;
    for (int i = 0; i < Config::m; i++) {
        s += '0' + get_bit(i);
    }
    if (s.size() < 15) {
        s += "=";
        int v = 0;
        int w = 1;
        for (int i = Config::m - 1; i >= 0; i--) {
            v += w * get_bit(i);
            w *= 2;
        }
        s += to_string(v);
    }
    return s;
}

bool BigRange::include(const BigId& id) const {
    if (low_ == high_) {
        // take care of special case (low, high] where low == high,
        // whole chord ring is included in this case
        return true;
    }

    if (low_ <= high_) {
        // no wrap around
        return low_ < id && id <= high_;
    } else {
        // have wrap around
        return low_ < id || id <= high_;
    }
}

BigId BigId::add_power_of_two(int pow) const {
    BigId new_id = *this;
    for (int idx = Config::m - pow - 1; idx >= 0; idx--) {
        if (new_id.get_bit(idx) == 1) {
            new_id.unset_bit(idx);
        } else {
            new_id.set_bit(idx);
            break;
        }
    }
    return new_id;
}

} // namespace dht
