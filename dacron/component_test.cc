#include <dacron/component.h>

#include <gtest/gtest.h>

namespace dacron {
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
  EXPECT_TRUE(ValidName("a"));
  EXPECT_FALSE(ValidName("0a"));
  EXPECT_FALSE(ValidName("a0/a"));
  EXPECT_TRUE(ValidName("a0"));
  EXPECT_TRUE(ValidName("a0a"));
  EXPECT_TRUE(ValidName("a_b"));
  Context ctx1;
  EXPECT_DEATH(Component(ctx1, ""), "Name is not valid");
}

TEST(Component, NameJoin) {
  EXPECT_EQ("a/b", NameJoin("a", "b"));
  EXPECT_EQ("a/b/c", NameJoin("a/b", "c"));
  EXPECT_DEATH(NameJoin("", "a"), "empty");
  EXPECT_DEATH(NameJoin("", ""), "empty");
  EXPECT_DEATH(NameJoin("a", ""), "empty");
}

TEST(Component, NameFullName) {
  Context ctx1;
  Component c1(ctx1, "foo");
  Component c2(c1, "bar");
  Component c3(c2, "baz");
  EXPECT_EQ("foo", c1.Name());
  EXPECT_EQ("bar", c2.Name());
  EXPECT_EQ("baz", c3.Name());
  EXPECT_EQ("foo", c1.FullName());
  EXPECT_EQ("foo/bar", c2.FullName());
  EXPECT_EQ("foo/bar/baz", c3.FullName());
}

}  // namespace dacron
