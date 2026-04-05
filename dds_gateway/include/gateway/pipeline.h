#ifndef GATEWAY_PIPELINE_H
#define GATEWAY_PIPELINE_H

#include "gateway/thread_safe_queue.h"
#include "gateway/message.h"
#include "gateway/input_adapter.h"
#include "gateway/processor.h"
#include "gateway/router.h"

#include <memory>
#include <thread>
#include <atomic>

namespace gateway {

// Pipeline owns the two internal queues and wires all stages:
//   InputAdapter -> [input_queue] -> Processor -> [processed_queue] -> Router -> outputs
// External code only needs to build the components, hand them over, and call start/stop.
class Pipeline {
public:
    explicit Pipeline(size_t queue_size = 4096);
    ~Pipeline();

    // Non-copyable
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    ThreadSafeQueue<Message>& input_queue()     { return input_queue_; }
    ThreadSafeQueue<Message>& processed_queue()  { return processed_queue_; }

    void set_input(std::shared_ptr<InputAdapter> input);
    void set_processor(std::shared_ptr<Processor> processor);
    void set_router(std::shared_ptr<Router> router);

    bool start();
    void stop();

private:
    void dispatch_loop();

    ThreadSafeQueue<Message> input_queue_;
    ThreadSafeQueue<Message> processed_queue_;

    std::shared_ptr<InputAdapter> input_;
    std::shared_ptr<Processor>    processor_;
    std::shared_ptr<Router>       router_;

    std::thread dispatch_thread_;
    std::atomic<bool> running_;
};

} // namespace gateway

#endif // GATEWAY_PIPELINE_H
