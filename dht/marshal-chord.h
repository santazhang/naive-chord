#pragma once

#include "rpc/marshal.h"
#include "utils.h"
#include "bigid.h"

namespace dht {

inline rpc::Marshal& operator <<(rpc::Marshal& m, const BigId& bi) {
    m.write(bi.raw_bytes(), g_sha1_bytes);
    return m;
}

inline rpc::Marshal& operator >>(rpc::Marshal& m, BigId& bi) {
    m.read(bi.raw_bytes(), g_sha1_bytes);
    return m;
}

} // namespace dht
