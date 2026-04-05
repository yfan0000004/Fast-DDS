#include "gateway/tcp_output_adapter.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>

namespace gateway {

TcpOutputAdapter::TcpOutputAdapter(const TcpConfig& config)
    : config_(config)
    , socket_fd_(-1) {}

TcpOutputAdapter::~TcpOutputAdapter() {
    disconnect();
}

bool TcpOutputAdapter::connect() {
    socket_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        return false;
    }

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(config_.port);
    ::inet_pton(AF_INET, config_.host.c_str(), &addr.sin_addr);

    if (::connect(socket_fd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        ::close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }
    return true;
}

bool TcpOutputAdapter::send(const Message& msg) {
    if (socket_fd_ < 0 && !try_reconnect()) {
        return false;
    }

    uint32_t len = static_cast<uint32_t>(msg.payload.size());
    uint32_t net_len = htonl(len);

    ssize_t n = ::send(socket_fd_, &net_len, sizeof(net_len), MSG_NOSIGNAL);
    if (n <= 0) {
        try_reconnect();
        return false;
    }

    n = ::send(socket_fd_, msg.payload.data(), msg.payload.size(), MSG_NOSIGNAL);
    return n == static_cast<ssize_t>(msg.payload.size());
}

void TcpOutputAdapter::disconnect() {
    if (socket_fd_ >= 0) {
        ::close(socket_fd_);
        socket_fd_ = -1;
    }
}

bool TcpOutputAdapter::try_reconnect() {
    disconnect();
    std::this_thread::sleep_for(std::chrono::milliseconds(config_.reconnect_interval_ms));
    return connect();
}

} // namespace gateway
