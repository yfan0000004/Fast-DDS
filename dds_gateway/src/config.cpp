#include "gateway/config.h"
#include "gateway/json.h"

namespace gateway {

GatewayConfig GatewayConfig::load(const std::string& path) {
    JsonValue root = JsonValue::from_file(path);
    GatewayConfig cfg;

    // DDS
    const JsonValue& dds = root["dds"];
    cfg.dds.domain_id = dds["domain_id"].num();
    for (size_t i = 0; i < dds["topics"].size(); ++i) {
        cfg.dds.topics.push_back(dds["topics"][i].str());
    }

    // TCP output
    const JsonValue& tcp = root["outputs"]["tcp"];
    cfg.tcp_enabled          = tcp["enabled"].boolean();
    cfg.tcp.host             = tcp["host"].str();
    cfg.tcp.port             = tcp["port"].num();
    cfg.tcp.reconnect_interval_ms = tcp["reconnect_interval_ms"].num();

    // MQTT output
    const JsonValue& mqtt = root["outputs"]["mqtt"];
    cfg.mqtt_enabled              = mqtt["enabled"].boolean();
    cfg.mqtt.broker_host          = mqtt["broker_host"].str();
    cfg.mqtt.broker_port          = mqtt["broker_port"].num();
    cfg.mqtt.client_id            = mqtt["client_id"].str();
    cfg.mqtt.qos                  = mqtt["qos"].num();
    cfg.mqtt.reconnect_interval_ms = mqtt["reconnect_interval_ms"].num();

    // Routes
    const JsonValue& routes = root["routes"];
    for (size_t i = 0; i < routes.size(); ++i) {
        RouteRule r;
        r.topic_pattern = routes[i]["topic"].str();
        r.output_name   = routes[i]["output"].str();
        cfg.routes.push_back(r);
    }

    // Queue
    if (root["queue_size"].type() == JsonValue::Number) {
        cfg.queue_size = static_cast<size_t>(root["queue_size"].num());
    }

    return cfg;
}

} // namespace gateway
