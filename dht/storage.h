#pragma once

#include <unordered_map>
#include <string>

namespace dht {

class Storage {
    std::unordered_map<std::string, std::string> data_;

public:
    void put(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string* value);
    bool remove(const std::string& key);
    void dump();
};

} // namespace dht
