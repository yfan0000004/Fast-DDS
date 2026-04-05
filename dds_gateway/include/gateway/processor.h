#ifndef GATEWAY_PROCESSOR_H
#define GATEWAY_PROCESSOR_H

#include "gateway/message.h"
#include "gateway/blocking_queue.h"

#include <thread>
#include <atomic>
#include <functional>

namespace gateway {

class Processor {
public:
    Processor(BlockingQueue<Message>& input_queue,
              BlockingQueue<Message>& output_queue);
    ~Processor();

    void set_filter(FilterFunc filter);
    void set_transform(TransformFunc transform);

    bool start();
    void stop();

private:
    void run();

    BlockingQueue<Message>& input_queue_;
    BlockingQueue<Message>& output_queue_;
    FilterFunc filter_;
    TransformFunc transform_;
    std::thread worker_;
    std::atomic<bool> running_;
};

} // namespace gateway

#endif // GATEWAY_PROCESSOR_H
