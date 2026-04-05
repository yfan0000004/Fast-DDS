#include "gateway/pipeline.h"
#include <iostream>

namespace gateway {

Pipeline::Pipeline(size_t queue_size)
    : input_queue_(queue_size)
    , output_queue_(queue_size) {}

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
    if (router_) {
        if (!router_->connect_all()) {
            std::cerr << "[Pipeline] router connect failed" << std::endl;
            return false;
        }
        router_->start();
    }

    if (processor_) {
        processor_->start();
    }

    if (input_) {
        input_->start();
    }

    return true;
}

void Pipeline::stop() {
    if (input_)     input_->stop();
    if (processor_) processor_->stop();
    if (router_)    router_->stop();
    if (router_)    router_->disconnect_all();
}

} // namespace gateway
