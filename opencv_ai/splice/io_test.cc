#include <opencv_ai/splice/io.h>

#include <gtest/gtest.h>
namespace opencv_ai {

class Counter {
 public:
  Counter(Context ctx, std::string name)
      : component_(ctx, name), count_out(component_, "count") {}

  void Increment() {
    count_out.Send(count_);
    count_++;
  }

 private:
  Component component_;
  int count_ = 0;

 public:
  Output<int> count_out;
};

class Adder {
 public:
  Adder(Context ctx, std::string name)
      : component_(ctx, name),
        a_in(component_, "a", MemberHandler(&Adder::HandleA, this)),
        b_in(component_, "b", MemberHandler(&Adder::HandleB, this)),
        sum_out(component_, "sum") {}

 private:
  void HandleA(int a) {
    a_ = a;
    Send();
  }

  void HandleB(int b) {
    b_ = b;
    Send();
  }

  void Send() { sum_out.Send(a_ + b_); }

  Component component_;
  int a_ = 0, b_ = 0;

 public:
  Input<int> a_in;
  Input<int> b_in;
  Output<int> sum_out;
};

TEST(InputOutput, Smoke) {
  Context ctx;
  Adder add(ctx, "add");
  Counter count1(ctx, "count1");
  Counter count2(ctx, "count1");
  Connect(count1.count_out, add.a_in);
  Connect(count2.count_out, add.b_in);
  int sum;
  Input<int> sum_in(ctx, "sum", ValueSetter(sum));
  Connect(add.sum_out, sum_in);
  for (int i = 0; i < 10; ++i) {
    count1.Increment();
    count2.Increment();
    ctx.ResetAndRun();
    EXPECT_EQ(i * 2, sum);
  }
}
}
