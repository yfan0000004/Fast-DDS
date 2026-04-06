#ifndef GATEWAY_MANAGER_H
#define GATEWAY_MANAGER_H

#include "gateway/blocking_queue.h"
#include "gateway/message.h"
#include "gateway/config.h"
#include "gateway/input_adapter.h"
#include "gateway/processor.h"
#include "gateway/router.h"

#include <memory>
#include <string>

namespace gateway {

class Manager {
public:
    Manager();
    ~Manager();

    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;

    bool init(const std::string& config_path);

    void set_input(std::shared_ptr<InputAdapter> input);
    void set_filter(FilterFunc filter);
    void set_transform(TransformFunc transform);

    std::shared_ptr<BlockingQueue<MessagePtr>> input_queue() const { return input_queue_; }

    bool start();
    void stop();

private:
    GatewayConfig config_;

    std::shared_ptr<BlockingQueue<MessagePtr>> input_queue_;
    std::shared_ptr<BlockingQueue<MessagePtr>> output_queue_;

    std::shared_ptr<InputAdapter> input_;
    std::shared_ptr<Processor>    processor_;
    std::shared_ptr<Router>       router_;

    bool initialized_;
};

} // namespace gateway

#endif // GATEWAY_MANAGER_H
