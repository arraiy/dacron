#ifndef DACRON_COMPONENT_H_
#define DACRON_COMPONENT_H_

#include <boost/optional.hpp>
#include <boost/regex.hpp>

#include <glog/logging.h>

#include <dacron/context.h>

namespace dacron {

inline bool ValidName(const std::string& name) {
  static const boost::regex e("[a-zA-Z][\\w_]*");
  return regex_match(name, e);
}

inline std::string NameJoin(const std::string& parent,
                            const std::string& child) {
  CHECK(!parent.empty());
  CHECK(!child.empty());
  return parent + "/" + child;
}

class Component {
 public:
  explicit Component(Context ctx, std::string name,
                     ContextState state = ContextState::kSame)
      : ctx_(ctx, state), name_(std::move(name)), full_name_(name_) {
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

  const Context& GetContext() const { return ctx_; }

  const std::string& Name() const { return name_; }

  const std::string& FullName() const { return full_name_; }

 private:
  Context ctx_;
  std::string name_, full_name_;
  boost::optional<const Component&> parent_;
};
}  // namespace dacron

#endif
