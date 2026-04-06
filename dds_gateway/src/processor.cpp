#include "gateway/processor.h"
#include <iostream>

namespace gateway {

Processor::Processor(std::shared_ptr<BlockingQueue<MessagePtr>> input_queue,
                     std::shared_ptr<BlockingQueue<MessagePtr>> output_queue)
    : input_queue_(input_queue)
    , output_queue_(output_queue)
    , running_(false) {}

Processor::~Processor() {
    stop();
}

void Processor::set_filter(FilterFunc filter) {
    filter_ = filter;
}

void Processor::set_transform(TransformFunc transform) {
    transform_ = transform;
}

bool Processor::start() {
    running_ = true;
    worker_ = std::thread(&Processor::run, this);
    return true;
}

void Processor::stop() {
    running_ = false;
    input_queue_->stop();
    if (worker_.joinable()) {
        worker_.join();
    }
}

void Processor::run() {
    MessagePtr msg;
    while (running_) {
        if (!input_queue_->pop(msg)) {
            break;
        }

        if (filter_ && !filter_(msg)) {
            continue;
        }

        MessagePtr out = transform_ ? transform_(msg) : msg;
        output_queue_->push(out);
    }
}

} // namespace gateway
