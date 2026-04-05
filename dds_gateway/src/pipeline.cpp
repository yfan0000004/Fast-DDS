#include "gateway/pipeline.h"
#include <iostream>

namespace gateway {

Pipeline::Pipeline()
    : input_queue_(4096)
    , processed_queue_(4096)
    , running_(false) {}

Pipeline::~Pipeline() {
    stop();
}

void Pipeline::set_input(std::shared_ptr<InputAdapter> input) {
    input_ = input;
}

void Pipeline::set_processor(std::shared_ptr<Processor> processor) {
    processor_ = processor;
}

void Pipeline::add_output(std::shared_ptr<OutputAdapter> output) {
    outputs_.push_back(output);
}

bool Pipeline::start() {
    for (auto& out : outputs_) {
        if (!out->connect()) {
            std::cerr << "[Pipeline] output adapter connect failed" << std::endl;
            return false;
        }
    }

    if (processor_) {
        processor_->start();
    }

    if (input_) {
        input_->start();
    }

    running_ = true;
    dispatch_thread_ = std::thread(&Pipeline::output_dispatch_loop, this);
    return true;
}

void Pipeline::stop() {
    running_ = false;
    processed_queue_.stop();

    if (input_) input_->stop();
    if (processor_) processor_->stop();

    if (dispatch_thread_.joinable()) {
        dispatch_thread_.join();
    }

    for (auto& out : outputs_) {
        out->disconnect();
    }
}

void Pipeline::output_dispatch_loop() {
    Message msg;
    while (running_) {
        if (!processed_queue_.pop(msg)) {
            break;
        }

        for (auto& out : outputs_) {
            if (!out->send(msg)) {
                std::cerr << "[Pipeline] output send failed for topic: "
                          << msg.topic << std::endl;
            }
        }
    }
}

} // namespace gateway
