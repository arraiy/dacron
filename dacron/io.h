#ifndef DACRON_IO_H_
#define DACRON_IO_H_

#include <dacron/component.h>
#include <boost/signals2.hpp>

namespace dacron {

template <typename MessageType>
class ChannelService : public boost::asio::io_service::service {
 public:
  ChannelService(boost::asio::io_service& io_service)
      : boost::asio::io_service::service(io_service) {}

  /// The unique service identifier.
  static boost::asio::io_service::id id;
  // Required by base class.
  void shutdown_service() override {}

  typedef boost::signals2::signal<void(MessageType)> Signal;

  Signal& LookUpSignal(const std::string& channel_name) {
    return signals_[channel_name];
  }

 private:
  std::map<std::string, Signal> signals_;
};

template <typename MessageType>
boost::asio::io_service::id ChannelService<MessageType>::id;

template <typename MessageType>
class Output {
 public:
  typedef typename ChannelService<MessageType>::Signal Signal;

  Output(const Context& context, std::string name)
      : ctx_(context),
        name_(std::move(name)),
        full_name_(name_),
        service_(boost::asio::use_service<ChannelService<MessageType>>(
            ctx_.GetIoService())) {}

  Output(const Component& component, std::string name)
      : ctx_(component.GetContext()),
        name_(std::move(name)),
        full_name_(NameJoin(component.FullName(), name_)),
        service_(boost::asio::use_service<ChannelService<MessageType>>(
            ctx_.GetIoService())) {}

  void Connect(std::string channel_name) {
    CHECK(!Connected());
    channel_name_ = std::move(channel_name);
    signal_ = service_.LookUpSignal(channel_name_);
  }

  bool Connected() const { return !!signal_; }

  std::string Name() const { return name_; }

  std::string FullName() const { return full_name_; }

  std::string ChannelName() const {
    CHECK(Connected());
    return channel_name_;
  }

  void Send(const MessageType& m) {
    if (signal_) {
      (*signal_)(m);
    }
  }

 private:
  Context ctx_;
  std::string name_, full_name_;
  ChannelService<MessageType>& service_;
  std::string channel_name_;
  boost::optional<Signal&> signal_;
};

template <typename MessageType>
class Input {
 public:
  typedef typename ChannelService<MessageType>::Signal Signal;
  typedef std::function<void(const MessageType&)> MessageHandler;

  Input(const Context& context, std::string name, MessageHandler handler)
      : ctx_(context),
        name_(std::move(name)),
        full_name_(name_),
        service_(boost::asio::use_service<ChannelService<MessageType>>(
            ctx_.GetIoService())),
        handler_(std::move(handler)) {
    CHECK(handler_ != nullptr);
  }

  Input(Component& component, std::string name, MessageHandler handler)
      : ctx_(component.GetContext()),
        name_(std::move(name)),
        full_name_(NameJoin(component.FullName(), name_)),
        service_(boost::asio::use_service<ChannelService<MessageType>>(
            ctx_.GetIoService())),
        handler_(std::move(handler)) {
    CHECK(handler_ != nullptr);
  }

  ~Input() { connection_.disconnect(); }

  void Connect(std::string channel_name) {
    CHECK(!Connected());
    channel_name_ = std::move(channel_name);
    signal_ = service_.LookUpSignal(channel_name_);
    connection_ =
        signal_->connect(std::bind(&Input::Send, this, std::placeholders::_1));
  }

  bool Connected() const { return !!signal_; }

  std::string ChannelName() const {
    CHECK(Connected());
    return channel_name_;
  }

  std::string Name() const { return name_; }

  std::string FullName() const { return full_name_; }

  void Send(const MessageType& message) {
    ctx_.Post([message, this] { handler_(message); });
  }

 private:
  Context ctx_;
  std::string name_, full_name_;
  ChannelService<MessageType>& service_;
  std::string channel_name_;
  boost::optional<Signal&> signal_;
  boost::signals2::connection connection_;
  MessageHandler handler_;
};

template <typename Function, typename Class>
auto MemberHandler(Function&& f, Class* thiz)
    -> decltype(std::bind(std::forward<Function>(f), thiz,
                          std::placeholders::_1)) {
  return std::bind(std::forward<Function>(f), thiz, std::placeholders::_1);
}

template <typename MessageType>
typename Input<MessageType>::MessageHandler ValueSetter(MessageType& value) {
  return [&value](MessageType x) { value = std::move(x); };
}

template <typename MessageType>
void Connect(Output<MessageType>& output, Input<MessageType>& input) {
  CHECK(!(output.Connected() && input.Connected()));
  if (output.Connected()) {
    input.Connect(output.ChannelName());
  } else if (input.Connected()) {
    output.Connect(input.ChannelName());
  } else {
    output.Connect(output.FullName());
    input.Connect(output.FullName());
  }
}

}  // namespace dacron
#endif  // DACRON_IO_H_
