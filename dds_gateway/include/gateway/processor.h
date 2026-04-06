#ifndef GATEWAY_PROCESSOR_H
#define GATEWAY_PROCESSOR_H

#include "gateway/message.h"
#include "gateway/blocking_queue.h"

#include <thread>
#include <atomic>
#include <functional>
#include <memory>

namespace gateway {

class Processor {
public:
    Processor(std::shared_ptr<BlockingQueue<MessagePtr>> input_queue,
              std::shared_ptr<BlockingQueue<MessagePtr>> output_queue);
    ~Processor();

    void set_filter(FilterFunc filter);
    void set_transform(TransformFunc transform);

    bool start();
    void stop();

private:
    void run();

    std::shared_ptr<BlockingQueue<MessagePtr>> input_queue_;
    std::shared_ptr<BlockingQueue<MessagePtr>> output_queue_;
    FilterFunc filter_;
    TransformFunc transform_;
    std::thread worker_;
    std::atomic<bool> running_;
};

} // namespace gateway

#endif // GATEWAY_PROCESSOR_H
