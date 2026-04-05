#include "gateway/dds_gateway.h"

#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

static std::atomic<bool> g_running(true);

static void signal_handler(int) {
    g_running = false;
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::string config_path = (argc > 1) ? argv[1] : "config.json";

    gateway::DdsGateway gw;

    if (!gw.init(config_path)) {
        return 1;
    }

    // Optional: register custom business logic before start
    // gw.set_filter([](const gateway::Message& msg) { ... });
    // gw.set_transform([](const gateway::Message& msg) { ... });

    if (!gw.start()) {
        return 1;
    }

    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    gw.stop();
    return 0;
}
