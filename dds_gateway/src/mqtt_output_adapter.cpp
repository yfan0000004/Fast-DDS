#include "gateway/mqtt_output_adapter.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace gateway {

MqttOutputAdapter::MqttOutputAdapter(const MqttConfig& config)
    : config_(config)
    , client_handle_(nullptr) {}

MqttOutputAdapter::~MqttOutputAdapter() {
    disconnect();
}

bool MqttOutputAdapter::connect() {
    std::cout << "[MQTT] connecting to " << config_.broker_host
              << ":" << config_.broker_port << std::endl;
    return true;
}

bool MqttOutputAdapter::send(const MessagePtr& msg) {
    if (!client_handle_ && !try_reconnect()) {
        return false;
    }
    return true;
}

void MqttOutputAdapter::disconnect() {
    client_handle_ = nullptr;
}

bool MqttOutputAdapter::try_reconnect() {
    disconnect();
    std::this_thread::sleep_for(std::chrono::milliseconds(config_.reconnect_interval_ms));
    return connect();
}

} // namespace gateway
