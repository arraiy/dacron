#include <opencv_ai/splice/context.h>

#include <gtest/gtest.h>

namespace opencv_ai {
TEST(Context, Construct) {
  Context ctx1;
  Context ctx2;
  EXPECT_NE(&ctx1.get_io_service(), &ctx2.get_io_service());
  Context ctx3 = ctx2;
  EXPECT_EQ(&ctx2.get_io_service(), &ctx3.get_io_service());
  EXPECT_EQ(&ctx2.get_strand(), &ctx3.get_strand());
  Context ctx4(ctx2, ContextState::kDifferent);
  EXPECT_EQ(&ctx2.get_io_service(), &ctx4.get_io_service());
  EXPECT_NE(&ctx2.get_strand(), &ctx4.get_strand());
}

TEST(Context, Post) {
  Context ctx;
  bool dispatched = false;
  ctx.post([&] { dispatched = true; });
  ctx.run();
  EXPECT_TRUE(dispatched);
}

TEST(Context, Wrap) {
  Context ctx;
  bool dispatched = false;
  auto f = ctx.wrap([&] { dispatched = true; });
  ctx.run();
  ctx.reset();
  EXPECT_FALSE(dispatched);
  f();
  EXPECT_FALSE(dispatched);
  ctx.run();
  ctx.reset();
  EXPECT_TRUE(dispatched);
}

}  // namespace opencv_ai
