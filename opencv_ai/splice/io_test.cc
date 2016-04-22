#include <opencv_ai/splice/io.h>

#include <gtest/gtest.h>
namespace opencv_ai {

class Counter : Component {
 public:
  Counter(Context ctx, std::string name)
      : Component(ctx, name), count_out(*this, "count") {}
  void Increment() {
    count_out.Send(count_);
    count_++;
  }
  Output<int> count_out;

 private:
  int count_ = 0;
};

class Adder : Component {
 public:
  Adder(Context ctx, std::string name)
      : Component(ctx, name),
        a_in(*this, "a", std::bind(&Adder::HandleA, this, _1)),
        b_in(*this, "b", std::bind(&Adder::HandleB, this, _1)),
        sum_out(*this, "sum") {}
  Input<int> a_in;
  Input<int> b_in;
  Output<int> sum_out;

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
  int a_ = 0, b_ = 0;
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
