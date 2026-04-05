#ifndef GATEWAY_CONFIG_H
#define GATEWAY_CONFIG_H

#include "gateway/dds_input_adapter.h"
#include "gateway/tcp_output_adapter.h"
#include "gateway/mqtt_output_adapter.h"

#include <string>
#include <vector>

namespace gateway {

struct RouteRule {
    std::string topic_pattern;   // exact match or "*" for wildcard
    std::string output_name;     // "tcp" or "mqtt"
};

struct GatewayConfig {
    DdsConfig  dds;
    TcpConfig  tcp;
    MqttConfig mqtt;
    bool       tcp_enabled;
    bool       mqtt_enabled;
    std::vector<RouteRule> routes;
    size_t     queue_size;

    GatewayConfig() : tcp_enabled(false), mqtt_enabled(false), queue_size(4096) {}

    static GatewayConfig load(const std::string& path);
};

} // namespace gateway

#endif // GATEWAY_CONFIG_H
