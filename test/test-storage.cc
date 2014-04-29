#include "base/all.h"
#include "dht/storage.h"

using namespace base;
using namespace std;
using namespace dht;

TEST(storage, basic_op) {
    Storage store;
    store.put("hello", "world");
    string fetch;
    EXPECT_TRUE(store.get("hello", &fetch));
    EXPECT_EQ(fetch, "world");
    EXPECT_FALSE(store.get("no such key", &fetch));
    EXPECT_EQ(fetch, "");
    EXPECT_FALSE(store.remove("no such key"));
    EXPECT_TRUE(store.remove("hello"));
    EXPECT_FALSE(store.get("hello", &fetch));
    EXPECT_EQ(fetch, "");
}
