#include <dacron/context.h>

#include <gtest/gtest.h>

namespace dacron {
TEST(Context, Construct) {
  Context ctx1;
  Context ctx2;
  EXPECT_NE(&ctx1.GetIoService(), &ctx2.GetIoService());
  Context ctx3 = ctx2;
  EXPECT_EQ(&ctx2.GetIoService(), &ctx3.GetIoService());
  EXPECT_EQ(&ctx2.GetStrand(), &ctx3.GetStrand());
  Context ctx4(ctx2, ContextState::kDifferent);
  EXPECT_EQ(&ctx2.GetIoService(), &ctx4.GetIoService());
  EXPECT_NE(&ctx2.GetStrand(), &ctx4.GetStrand());
}

TEST(Context, Post) {
  Context ctx;
  bool dispatched = false;
  ctx.Post([&] { dispatched = true; });
  ctx.Run();
  EXPECT_TRUE(dispatched);
}

TEST(Context, Wrap) {
  Context ctx;
  bool dispatched = false;
  auto f = ctx.Wrap([&] { dispatched = true; });
  ctx.Run();
  ctx.Reset();
  EXPECT_FALSE(dispatched);
  f();
  EXPECT_FALSE(dispatched);
  ctx.Run();
  ctx.Reset();
  EXPECT_TRUE(dispatched);
}

}  // namespace dacron
