#include <opencv_ai/splice/component.h>

#include <gtest/gtest.h>

namespace opencv_ai {
TEST(Component, Construct) {
  Context ctx1;
  Component comp1(ctx1, "comp1");
  EXPECT_EQ(&ctx1.GetStrand(), &comp1.GetContext().GetStrand());
  Component comp2(comp1, "comp2");
  EXPECT_EQ(&ctx1.GetStrand(), &comp2.GetContext().GetStrand());
  Component comp3(comp1, "comp3", ContextState::kDifferent);
  EXPECT_NE(&ctx1.GetStrand(), &comp3.GetContext().GetStrand());
}

TEST(Component, ValidName) {
  EXPECT_FALSE(ValidName(""));
  EXPECT_FALSE(ValidName("0"));
  EXPECT_FALSE(ValidName("a"));
  EXPECT_FALSE(ValidName("0a"));
  EXPECT_FALSE(ValidName("a0/a"));

  EXPECT_TRUE(ValidName("a0"));
  EXPECT_TRUE(ValidName("a0a"));
  Context ctx1;
  EXPECT_DEATH(Component(ctx1, ""), "Name is not valid");
}

}  // namespace opencv_ai
