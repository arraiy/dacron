#include <splice/splice.h>

#include <gtest/gtest.h>

namespace splice {
TEST(Splice, Version) { EXPECT_EQ("0.0.0", Version()); }
}  // namespace opencv_ai
