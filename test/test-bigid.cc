#include "base/all.h"
#include "dht/bigid.h"
#include "dht/marshal-chord.h"

using namespace base;
using namespace dht;
using namespace rpc;

TEST(bigid, cmp) {
    BigId bigid;
    EXPECT_TRUE(bigid == bigid);
    EXPECT_FALSE(bigid != bigid);
    EXPECT_FALSE(bigid < bigid);
    EXPECT_FALSE(bigid > bigid);
    EXPECT_TRUE(bigid <= bigid);
    EXPECT_TRUE(bigid >= bigid);
}

TEST(bigid, marshal) {
    Marshal m;
    BigId bigid("blah");
    m << bigid;
    EXPECT_EQ(m.content_size(), g_sha1_bytes);
    BigId bi2;
    m >> bi2;
    EXPECT_TRUE(bi2 == bigid);
    EXPECT_EQ(m.content_size(), 0);
}
