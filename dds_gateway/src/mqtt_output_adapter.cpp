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
    // TODO: replace with actual MQTT client initialization
    // e.g. using Eclipse Paho C/C++ client:
    //   MQTTClient_create(&client_handle_, broker_uri, client_id, ...);
    //   MQTTClient_connect(client_handle_, &conn_opts);
    std::cout << "[MQTT] connecting to " << config_.broker_host
              << ":" << config_.broker_port << std::endl;
    return true;
}

bool MqttOutputAdapter::send(const Message& msg) {
    if (!client_handle_ && !try_reconnect()) {
        return false;
    }

    // TODO: replace with actual MQTT publish
    // e.g.:
    //   MQTTClient_message pubmsg = MQTTClient_message_initializer;
    //   pubmsg.payload = (void*)msg.payload.data();
    //   pubmsg.payloadlen = msg.payload.size();
    //   pubmsg.qos = config_.qos;
    //   MQTTClient_publishMessage(client_handle_, msg.topic.c_str(), &pubmsg, &token);
    return true;
}

void MqttOutputAdapter::disconnect() {
    if (client_handle_) {
        // TODO: MQTTClient_disconnect(client_handle_, timeout);
        //       MQTTClient_destroy(&client_handle_);
        client_handle_ = nullptr;
    }
}

bool MqttOutputAdapter::try_reconnect() {
    disconnect();
    std::this_thread::sleep_for(std::chrono::milliseconds(config_.reconnect_interval_ms));
    return connect();
}

} // namespace gateway
