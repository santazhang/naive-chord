#pragma once

#include "base/all.h"

namespace dht {

using base::i32;
using base::i64;
using base::NoCopy;
using base::RefCounted;
using base::Log;
using base::SpinLock;
using base::insert_into_map;

// host:port address
typedef std::string host_port;

} // namespace dht
