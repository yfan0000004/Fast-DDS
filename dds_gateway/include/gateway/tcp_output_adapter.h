#ifndef GATEWAY_TCP_OUTPUT_ADAPTER_H
#define GATEWAY_TCP_OUTPUT_ADAPTER_H

#include "gateway/output_adapter.h"
#include <string>

namespace gateway {

struct TcpConfig {
    std::string host;
    int port;
    int reconnect_interval_ms;

    TcpConfig() : host("127.0.0.1"), port(9000), reconnect_interval_ms(3000) {}
};

class TcpOutputAdapter : public OutputAdapter {
public:
    explicit TcpOutputAdapter(const TcpConfig& config);
    ~TcpOutputAdapter();

    bool connect() override;
    bool send(const Message& msg) override;
    void disconnect() override;

private:
    bool try_reconnect();

    TcpConfig config_;
    int socket_fd_;
};

} // namespace gateway

#endif // GATEWAY_TCP_OUTPUT_ADAPTER_H
