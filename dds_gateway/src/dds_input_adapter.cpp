#include "gateway/dds_input_adapter.h"
#include <iostream>
#include <chrono>

namespace gateway {

DdsInputAdapter::DdsInputAdapter(const DdsConfig& config,
                                 std::shared_ptr<BlockingQueue<Message>> input_queue)
    : config_(config)
    , input_queue_(input_queue)
    , running_(false) {}

DdsInputAdapter::~DdsInputAdapter() {
    stop();
}

bool DdsInputAdapter::start() {
    // TODO: initialize DDS participant, subscriber, data readers
    running_ = true;
    worker_ = std::thread(&DdsInputAdapter::subscribe_loop, this);
    return true;
}

void DdsInputAdapter::stop() {
    running_ = false;
    if (worker_.joinable()) {
        worker_.join();
    }
    // TODO: teardown DDS entities
}

void DdsInputAdapter::subscribe_loop() {
    while (running_) {
        // TODO: replace with actual DDS wait-set / take() call
        // Pseudocode:
        //   auto samples = data_reader->take();
        //   for (auto& s : samples) {
        //       Message msg(topic, serialize(s), "dds");
        //       input_queue_->push(msg);
        //   }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

} // namespace gateway
