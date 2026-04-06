#include "gateway/manager.h"

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

    gateway::Manager mgr;

    if (!mgr.init(config_path)) {
        return 1;
    }

    // Optional: replace default DDS input with custom source
    // mgr.set_input(my_custom_adapter);

    // Optional: register custom business logic
    // mgr.set_filter([](const gateway::MessagePtr& msg) {
    //     return msg->type_name == "sensor::IMU";
    // });
    // mgr.set_transform([](const gateway::MessagePtr& msg) {
    //     auto out = std::make_shared<gateway::Message>(*msg);
    //     out->properties["processed"] = "true";
    //     return out;
    // });

    if (!mgr.start()) {
        return 1;
    }

    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    mgr.stop();
    return 0;
}
