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
  void Post(Function&& f) {
    strand_->post(std::forward<Function>(f));
  }

  void ResetAndRun() {
    Reset();
    Run();
  }

  // Run the io_service until it is out of work.
  // subsequent calls to Run. See io_service::reset() docs.

  size_t Run() { return io_service_->run(); }

  // Reset the Context's io_service. This should be invoked before
  // subsequent calls to Run. See io_service::reset() docs.
  void Reset() { io_service_->reset(); }

  boost::asio::io_service::strand& GetStrand() const { return *strand_; }
  boost::asio::io_service& GetIoService() const { return *io_service_; }

 private:
  // A class invariant is that io_service_, and strand_ are never
  // null.  These are shared_ptr so that they may be aliased between
  // copies of the same context.
  std::shared_ptr<boost::asio::io_service> io_service_;
  std::shared_ptr<boost::asio::io_service::strand> strand_;

 public:
  // Returns a wrapped function object which when invoked posts itself
  // to the context's strand.  The wrapped function object should have
  // the same signature as the f.
  //
  // NOTE: This needs to go at the bottom, so that the compiler has enough
  // type information to for the auto return type.
  template <typename Function>
  auto Wrap(Function&& f)
      -> decltype(this->strand_->wrap(std::forward<Function>(f))) {
    return strand_->wrap(std::forward<Function>(f));
  }
};
}  // namespace opencv_ai

#endif  // OPENCV_AI_SPLICE_CONTEXT_H_
