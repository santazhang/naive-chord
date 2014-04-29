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
    return memcmp(sha1_, o.sha1_, BigId::M);
}

} // namespace dht
