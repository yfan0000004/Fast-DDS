#ifndef GATEWAY_MQTT_OUTPUT_ADAPTER_H
#define GATEWAY_MQTT_OUTPUT_ADAPTER_H

#include "gateway/output_adapter.h"
#include <string>

namespace gateway {

struct MqttConfig {
    std::string broker_host;
    int broker_port;
    std::string client_id;
    int qos;
    int reconnect_interval_ms;

    MqttConfig()
        : broker_host("127.0.0.1")
        , broker_port(1883)
        , client_id("dds_gateway")
        , qos(1)
        , reconnect_interval_ms(3000) {}
};

class MqttOutputAdapter : public OutputAdapter {
public:
    explicit MqttOutputAdapter(const MqttConfig& config);
    ~MqttOutputAdapter();

    bool connect() override;
    bool send(const Message& msg) override;
    void disconnect() override;

private:
    bool try_reconnect();

    MqttConfig config_;
    void* client_handle_; // opaque handle for MQTT client library
};

} // namespace gateway

#endif // GATEWAY_MQTT_OUTPUT_ADAPTER_H
