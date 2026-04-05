#include "gateway/processor.h"
#include <iostream>

namespace gateway {

Processor::Processor(ThreadSafeQueue<Message>& input_queue,
                     ThreadSafeQueue<Message>& output_queue)
    : input_queue_(input_queue)
    , output_queue_(output_queue)
    , running_(false) {}

Processor::~Processor() {
    stop();
}

void Processor::set_filter(ProcessFunc filter) {
    filter_ = filter;
}

void Processor::set_transform(ProcessFunc transform) {
    transform_ = transform;
}

bool Processor::start() {
    running_ = true;
    worker_ = std::thread(&Processor::run, this);
    return true;
}

void Processor::stop() {
    running_ = false;
    input_queue_.stop();
    if (worker_.joinable()) {
        worker_.join();
    }
}

void Processor::run() {
    Message msg;
    while (running_) {
        if (!input_queue_.pop(msg)) {
            break;
        }

        if (filter_ && !filter_(msg)) {
            continue;
        }

        if (transform_) {
            transform_(msg);
        }

        output_queue_.push(msg);
    }
}

} // namespace gateway
