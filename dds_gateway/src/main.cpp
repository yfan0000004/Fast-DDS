#include "gateway/pipeline.h"
#include "gateway/dds_input_adapter.h"
#include "gateway/processor.h"
#include "gateway/tcp_output_adapter.h"
#include "gateway/mqtt_output_adapter.h"

#include <iostream>
#include <csignal>
#include <atomic>

static std::atomic<bool> g_running(true);

static void signal_handler(int) {
    g_running = false;
}

int main() {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // --- Build pipeline ---

    gateway::Pipeline pipeline;

    // 1) Input: DDS subscriber
    gateway::DdsConfig dds_cfg;
    dds_cfg.domain_id = 0;
    dds_cfg.topics.push_back("sensor/imu");
    dds_cfg.topics.push_back("sensor/lidar");

    auto input = std::make_shared<gateway::DdsInputAdapter>(
        dds_cfg, pipeline.get_input_queue());

    // 2) Processor: business logic
    auto processor = std::make_shared<gateway::Processor>(
        pipeline.get_input_queue(), pipeline.get_processed_queue());

    processor->set_filter([](gateway::Message& msg) -> bool {
        // TODO: add real filtering rules
        return !msg.payload.empty();
    });

    processor->set_transform([](gateway::Message& msg) -> bool {
        // TODO: add real transform / enrichment
        return true;
    });

    // 3) Output: TCP + MQTT (can enable one or both)
    gateway::TcpConfig tcp_cfg;
    tcp_cfg.host = "127.0.0.1";
    tcp_cfg.port = 9000;

    gateway::MqttConfig mqtt_cfg;
    mqtt_cfg.broker_host = "127.0.0.1";
    mqtt_cfg.broker_port = 1883;

    auto tcp_out = std::make_shared<gateway::TcpOutputAdapter>(tcp_cfg);
    auto mqtt_out = std::make_shared<gateway::MqttOutputAdapter>(mqtt_cfg);

    // --- Assemble ---
    pipeline.set_input(input);
    pipeline.set_processor(processor);
    pipeline.add_output(tcp_out);
    pipeline.add_output(mqtt_out);

    // --- Run ---
    if (!pipeline.start()) {
        std::cerr << "Failed to start pipeline" << std::endl;
        return 1;
    }
    std::cout << "DDS Gateway running. Press Ctrl+C to stop." << std::endl;

    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    pipeline.stop();
    std::cout << "DDS Gateway stopped." << std::endl;
    return 0;
}
