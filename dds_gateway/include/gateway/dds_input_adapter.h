#ifndef GATEWAY_DDS_INPUT_ADAPTER_H
#define GATEWAY_DDS_INPUT_ADAPTER_H

#include "gateway/input_adapter.h"
#include "gateway/thread_safe_queue.h"
#include "gateway/message.h"

#include <string>
#include <vector>
#include <thread>
#include <atomic>

namespace gateway {

struct DdsConfig {
    int domain_id;
    std::vector<std::string> topics;

    DdsConfig() : domain_id(0) {}
};

class DdsInputAdapter : public InputAdapter {
public:
    DdsInputAdapter(const DdsConfig& config, ThreadSafeQueue<Message>& output_queue);
    ~DdsInputAdapter();

    bool start() override;
    void stop() override;

private:
    void subscribe_loop();

    DdsConfig config_;
    ThreadSafeQueue<Message>& output_queue_;
    std::thread worker_;
    std::atomic<bool> running_;
};

} // namespace gateway

#endif // GATEWAY_DDS_INPUT_ADAPTER_H
