#ifndef OPENCV_AI_SPLICE_TCP_H_
#define OPENCV_AI_SPLICE_TCP_H_

#include <opencv_ai/splice/context.h>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/bind.hpp>

#include <glog/logging.h>

namespace opencv_ai {
using tcp = boost::asio::ip::tcp;
constexpr const size_t kBufferHeaderLength = 4 + 8 + 4;

class BufferClient {
 public:
  BufferClient(Context ctx, std::string host, std::string port,
               std::function<void(size_t stamp,
                                  const std::vector<char>& buffer)> handler)
      : ctx_(ctx),
        host_(host),
        port_(port),
        resolver_(ctx_.GetIoService()),
        socket_(ctx_.GetIoService()),
        handler_(std::move(handler)) {
    CHECK(!!handler_) << "Handler may not be null.";
    tcp::resolver::query query(tcp::v4(), host, port);
    resolver_.async_resolve(
        query, ctx_.Wrap(boost::bind(&BufferClient::HandleResolve, this,
                                     boost::asio::placeholders::error,
                                     boost::asio::placeholders::iterator)));
  }

 private:
  void HandleResolve(const boost::system::error_code& ec,
                     tcp::resolver::iterator endpoint_iterator) {
    CHECK(!ec) << "Could not connect to tcp://" << host_ << ":" << port_
               << " Error: " << ec.message();
    boost::asio::connect(socket_, endpoint_iterator);
    ctx_.Post(std::bind(&BufferClient::ReceiveLoop, this,
                        boost::system::error_code(), size_t(0)));
  }

  void ReceiveLoop(boost::system::error_code ec, std::size_t n_bytes) {
    CHECK(!ec) << "ReceiveLoop error tcp://" << host_ << ":" << port_
               << " Error: " << ec.message();
    CHECK(ctx_.GetStrand().running_in_this_thread());
    BOOST_ASIO_CORO_REENTER(coro_) {
      while (true) {
        BOOST_ASIO_CORO_YIELD
        boost::asio::async_read(
            socket_, boost::asio::buffer(header_), boost::asio::transfer_all(),
            ctx_.Wrap(
                boost::bind(&BufferClient::ReceiveLoop, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred)));

        CHECK_EQ(kBufferHeaderLength, n_bytes);
        CHECK_EQ('c', header_[0]);
        CHECK_EQ('v', header_[1]);
        CHECK_EQ('a', header_[2]);
        CHECK_EQ('i', header_[3]);
        std::memcpy(&stamp_, &header_[4], 8);
        stamp_ = be64toh(stamp_);
        uint32_t data_length;
        std::memcpy(&data_length, &header_[12], 4);
        data_length = be32toh(data_length);
        buffer_.resize(data_length);
        BOOST_ASIO_CORO_YIELD
        boost::asio::async_read(
            socket_, boost::asio::buffer(buffer_), boost::asio::transfer_all(),
            ctx_.Wrap(
                boost::bind(&BufferClient::ReceiveLoop, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred)));

        CHECK_EQ(buffer_.size(), n_bytes);
        handler_(stamp_, buffer_);
      }
    }
  }

 private:
  boost::asio::coroutine coro_;
  Context ctx_;
  std::string host_, port_;
  tcp::resolver resolver_;
  tcp::socket socket_;

  std::array<char, kBufferHeaderLength> header_;
  uint64_t stamp_;
  std::vector<char> buffer_;
  std::function<void(size_t stamp, const std::vector<char>& buffer)> handler_;
};
}

#endif  // OPENCV_AI_SPLICE_TCP_H_
