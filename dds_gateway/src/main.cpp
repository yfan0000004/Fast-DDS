#include "gateway/config.h"
#include "gateway/pipeline.h"
#include "gateway/dds_input_adapter.h"
#include "gateway/processor.h"
#include "gateway/router.h"
#include "gateway/tcp_output_adapter.h"
#include "gateway/mqtt_output_adapter.h"

#include <iostream>
#include <csignal>
#include <atomic>

static std::atomic<bool> g_running(true);

static void signal_handler(int) {
    g_running = false;
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::string config_path = "config.json";
    if (argc > 1) {
        config_path = argv[1];
    }

    // 1) Load config
    gateway::GatewayConfig cfg;
    try {
        cfg = gateway::GatewayConfig::load(config_path);
    } catch (const std::exception& e) {
        std::cerr << "Failed to load config: " << e.what() << std::endl;
        return 1;
    }

    // 2) Build pipeline (owns the queues)
    gateway::Pipeline pipeline(cfg.queue_size);

    // 3) Input adapter
    auto input = std::make_shared<gateway::DdsInputAdapter>(
        cfg.dds, pipeline.input_queue());

    // 4) Processor
    auto processor = std::make_shared<gateway::Processor>(
        pipeline.input_queue(), pipeline.processed_queue());

    processor->set_filter([](gateway::Message& msg) -> bool {
        return !msg.payload.empty();
    });

    processor->set_transform([](gateway::Message& msg) -> bool {
        // TODO: implement actual business logic
        return true;
    });

    // 5) Router + outputs
    auto router = std::make_shared<gateway::Router>();
    router->set_rules(cfg.routes);

    if (cfg.tcp_enabled) {
        router->register_output("tcp",
            std::make_shared<gateway::TcpOutputAdapter>(cfg.tcp));
    }
    if (cfg.mqtt_enabled) {
        router->register_output("mqtt",
            std::make_shared<gateway::MqttOutputAdapter>(cfg.mqtt));
    }

    // 6) Assemble
    pipeline.set_input(input);
    pipeline.set_processor(processor);
    pipeline.set_router(router);

    // 7) Run
    if (!pipeline.start()) {
        std::cerr << "Failed to start pipeline" << std::endl;
        return 1;
    }
    std::cout << "DDS Gateway running (config: " << config_path << "). "
              << "Press Ctrl+C to stop." << std::endl;

    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    pipeline.stop();
    std::cout << "DDS Gateway stopped." << std::endl;
    return 0;
}
