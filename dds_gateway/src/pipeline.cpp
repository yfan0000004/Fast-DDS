#include "gateway/pipeline.h"
#include <iostream>

namespace gateway {

Pipeline::Pipeline(size_t queue_size)
    : input_queue_(queue_size)
    , processed_queue_(queue_size)
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

void Pipeline::set_router(std::shared_ptr<Router> router) {
    router_ = router;
}

bool Pipeline::start() {
    if (router_ && !router_->connect_all()) {
        std::cerr << "[Pipeline] router connect failed" << std::endl;
        return false;
    }

    if (processor_) {
        processor_->start();
    }

    if (input_) {
        input_->start();
    }

    running_ = true;
    dispatch_thread_ = std::thread(&Pipeline::dispatch_loop, this);
    return true;
}

void Pipeline::stop() {
    running_ = false;
    processed_queue_.stop();
    input_queue_.stop();

    if (input_)     input_->stop();
    if (processor_) processor_->stop();

    if (dispatch_thread_.joinable()) {
        dispatch_thread_.join();
    }

    if (router_) router_->disconnect_all();
}

void Pipeline::dispatch_loop() {
    Message msg;
    while (running_) {
        if (!processed_queue_.pop(msg)) {
            break;
        }
        if (router_) {
            router_->dispatch(msg);
        }
    }
}

} // namespace gateway
