#ifndef OPENCV_AI_SPLICE_CONTEXT_H_
#define OPENCV_AI_SPLICE_CONTEXT_H_

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>

namespace opencv_ai {

enum class ContextState { kSame, kDifferent };

// A stranded execution context.
class Context {
 public:
  // Default construct a context, it gets its own io_service.
  Context()
      : io_service_(std::make_shared<boost::asio::io_service>()),
        strand_(
            std::make_shared<boost::asio::io_service::strand>(*io_service_)) {}

  // Copy a context, specify whether the context shares the parent's
  // strand, or gets a different one.
  Context(const Context& context, ContextState state)
      : io_service_(context.io_service_) {
    switch (state) {
      case ContextState::kSame:
        strand_ = context.strand_;
        break;
      case ContextState::kDifferent:
        strand_ =
            std::make_shared<boost::asio::io_service::strand>(*io_service_);
        break;
    }
  }

  Context(const Context& context) = default;
  Context& operator=(const Context& context) = default;
  Context(Context&& context) = default;
  Context& operator=(Context&& context) = default;

  template <typename Function>
  void post(Function&& f) {
    strand_->post(std::forward<Function>(f));
  }

  template <typename Function>
  std::function<void()> wrap(Function&& f) {
    return strand_->wrap(std::forward<Function>(f));
  }

  size_t run() { return io_service_->run(); }

  void reset() { io_service_->reset(); }

  boost::asio::io_service::strand& get_strand() const { return *strand_; }
  boost::asio::io_service& get_io_service() const { return *io_service_; }

 private:
  std::shared_ptr<boost::asio::io_service> io_service_;
  std::shared_ptr<boost::asio::io_service::strand> strand_;
};
}  // namespace opencv_ai

#endif  // OPENCV_AI_SPLICE_CONTEXT_H_
