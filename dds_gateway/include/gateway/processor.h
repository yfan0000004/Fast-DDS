#ifndef GATEWAY_PROCESSOR_H
#define GATEWAY_PROCESSOR_H

#include "gateway/message.h"
#include "gateway/thread_safe_queue.h"

#include <thread>
#include <atomic>
#include <functional>

namespace gateway {

using ProcessFunc = std::function<bool(Message&)>;

class Processor {
public:
    Processor(ThreadSafeQueue<Message>& input_queue,
              ThreadSafeQueue<Message>& output_queue);
    ~Processor();

    void set_filter(ProcessFunc filter);
    void set_transform(ProcessFunc transform);

    bool start();
    void stop();

private:
    void run();

    ThreadSafeQueue<Message>& input_queue_;
    ThreadSafeQueue<Message>& output_queue_;
    ProcessFunc filter_;
    ProcessFunc transform_;
    std::thread worker_;
    std::atomic<bool> running_;
};

} // namespace gateway

#endif // GATEWAY_PROCESSOR_H
