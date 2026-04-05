#include "gateway/router.h"
#include <iostream>

namespace gateway {

Router::Router(BlockingQueue<Message>& output_queue)
    : output_queue_(output_queue)
    , running_(false) {}

Router::~Router() {
    stop();
}

void Router::add_output(const std::string& name,
                        std::shared_ptr<OutputAdapter> adapter) {
    outputs_[name] = adapter;
}

void Router::set_routes(const std::vector<RouteRule>& rules) {
    rules_ = rules;
}

bool Router::connect_all() {
    for (auto& kv : outputs_) {
        if (!kv.second->connect()) {
            std::cerr << "[Router] failed to connect output: " << kv.first << std::endl;
            return false;
        }
    }
    return true;
}

void Router::disconnect_all() {
    for (auto& kv : outputs_) {
        kv.second->disconnect();
    }
}

bool Router::start() {
    running_ = true;
    worker_ = std::thread(&Router::dispatch_loop, this);
    return true;
}

void Router::stop() {
    running_ = false;
    output_queue_.stop();
    if (worker_.joinable()) {
        worker_.join();
    }
}

void Router::dispatch_loop() {
    Message msg;
    while (running_) {
        if (!output_queue_.pop(msg)) {
            break;
        }
        dispatch(msg);
    }
}

bool Router::dispatch(const Message& msg) {
    bool any_ok = false;
    bool matched = false;

    for (const auto& rule : rules_) {
        if (rule.topic_pattern != "*" && rule.topic_pattern != msg.topic) {
            continue;
        }

        matched = true;
        auto it = outputs_.find(rule.output_name);
        if (it == outputs_.end()) continue;

        if (it->second->send(msg)) {
            any_ok = true;
        } else {
            std::cerr << "[Router] send failed: topic=" << msg.topic
                      << " output=" << rule.output_name << std::endl;
        }

        if (rule.topic_pattern != "*") {
            break;
        }
    }

    if (!matched) {
        std::cerr << "[Router] no route for topic: " << msg.topic << std::endl;
    }
    return any_ok;
}

} // namespace gateway
