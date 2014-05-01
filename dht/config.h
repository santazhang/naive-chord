#pragma once

namespace dht {

class Config {
public:
    static const int hash_bytes = 20;   // sha1
    static const int m = 41;    // chord ring parameter
};

} // namespace dht
