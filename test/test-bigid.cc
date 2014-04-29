#include "base/all.h"
#include "dht/bigid.h"

using namespace base;
using namespace dht;

TEST(bigid, cmp) {
    BigId bigid;
    EXPECT_TRUE(bigid == bigid);
    EXPECT_FALSE(bigid != bigid);
    EXPECT_FALSE(bigid < bigid);
    EXPECT_FALSE(bigid > bigid);
    EXPECT_TRUE(bigid <= bigid);
    EXPECT_TRUE(bigid >= bigid);
}
