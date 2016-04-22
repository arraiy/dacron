#ifndef OPENCV_AI_SPLICE_COMPONENT_H_
#define OPENCV_AI_SPLICE_COMPONENT_H_

#include <boost/optional.hpp>
#include <boost/regex.hpp>

#include <glog/logging.h>

#include <opencv_ai/splice/context.h>

namespace opencv_ai {

bool ValidName(const std::string& name) {
  static const boost::regex e("[a-zA-Z][\\w]+");
  return regex_match(name, e);
}

inline std::string NameJoin(const std::string& parent,
                            const std::string& child) {
  return parent + "/" + child;
}

class Component {
 public:
  explicit Component(Context ctx, std::string name,
                     ContextState state = ContextState::kSame)
      : ctx_(ctx, state), name_(std::move(name)) {
    CHECK(ValidName(name_)) << "Name is not valid." << name_;
  }

  explicit Component(const Component& parent, std::string name,
                     ContextState state = ContextState::kSame)
      : ctx_(parent.GetContext(), state),
        name_(std::move(name)),
        full_name_(NameJoin(parent.FullName(), name_)),
        parent_(parent) {
    CHECK(ValidName(name_)) << "Name is not valid." << name_;
  }

  Context& GetContext() { return ctx_; }

  const Context& GetContext() const { return ctx_; }

  const std::string& Name() const { return name_; }

  const std::string& FullName() const {
    if (parent_) {
      return full_name_;
    }
    return name_;
  }

 private:
  Context ctx_;
  std::string name_, full_name_;
  boost::optional<const Component&> parent_;
};
}  // namespace opencv_ai

#endif
