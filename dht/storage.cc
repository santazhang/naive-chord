#include "storage.h"
#include "utils.h"

namespace dht {

void Storage::put(const std::string& key, const std::string& value) {
    insert_into_map(data_, key, value);
}

bool Storage::get(const std::string& key, std::string* value) {
    auto it = data_.find(key);
    if (it == data_.end()) {
        value->clear();
        return false;
    } else {
        *value = it->second;
        return true;
    }
}

bool Storage::remove(const std::string& key) {
    auto it = data_.find(key);
    if (it == data_.end()) {
        return false;
    } else {
        data_.erase(it);
        return true;
    }
}

} // namespace dht
