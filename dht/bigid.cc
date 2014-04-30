#include <string.h>

#include "bigid.h"
#include "utils.h"
#include "sha1.h"

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

int BigId::cmp(const BigId& o) const {
    int full_bytes = BigId::M / 8;
    int r = memcmp(sha1_, o.sha1_, full_bytes);
    if (r != 0) {
        return r;
    }

    int residual = BigId::M % 8;
    if (residual == 0) {
        return r;
    }

    int mask = (1 << residual) - 1;
    int a = sha1_[full_bytes] & mask;
    int b = o.sha1_[full_bytes] & mask;
    if (a < b) {
        return -1;
    } else if (a > b) {
        return 1;
    } else {
        return 0;
    }
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

} // namespace dht
