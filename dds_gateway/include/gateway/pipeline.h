#ifndef GATEWAY_PIPELINE_H
#define GATEWAY_PIPELINE_H

#include "gateway/thread_safe_queue.h"
#include "gateway/message.h"
#include "gateway/dds_input_adapter.h"
#include "gateway/processor.h"
#include "gateway/output_adapter.h"

#include <memory>
#include <vector>
#include <thread>
#include <atomic>

namespace gateway {

class Pipeline {
public:
    Pipeline();
    ~Pipeline();

    void set_input(std::shared_ptr<InputAdapter> input);
    void set_processor(std::shared_ptr<Processor> processor);
    void add_output(std::shared_ptr<OutputAdapter> output);

    bool start();
    void stop();

    ThreadSafeQueue<Message>& get_input_queue() { return input_queue_; }
    ThreadSafeQueue<Message>& get_processed_queue() { return processed_queue_; }

private:
    void output_dispatch_loop();

    ThreadSafeQueue<Message> input_queue_;
    ThreadSafeQueue<Message> processed_queue_;

    std::shared_ptr<InputAdapter> input_;
    std::shared_ptr<Processor> processor_;
    std::vector<std::shared_ptr<OutputAdapter>> outputs_;

    std::thread dispatch_thread_;
    std::atomic<bool> running_;
};

} // namespace gateway

#endif // GATEWAY_PIPELINE_H
