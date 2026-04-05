#ifndef GATEWAY_GATEWAY_H
#define GATEWAY_GATEWAY_H

#include "gateway/blocking_queue.h"
#include "gateway/message.h"
#include "gateway/config.h"
#include "gateway/input_adapter.h"
#include "gateway/processor.h"
#include "gateway/router.h"

#include <memory>
#include <string>

namespace gateway {

// Gateway is the single entry point of the entire application.
// It reads config, creates all internal components, and manages their lifecycle.
//
// Usage:
//   Gateway gw;
//   gw.init("config.json");
//   gw.start();
//   // ... wait for signal ...
//   gw.stop();
class Gateway {
public:
    Gateway();
    ~Gateway();

    Gateway(const Gateway&) = delete;
    Gateway& operator=(const Gateway&) = delete;

    bool init(const std::string& config_path);

    void set_filter(FilterFunc filter);
    void set_transform(TransformFunc transform);

    bool start();
    void stop();

private:
    GatewayConfig config_;

    std::unique_ptr<BlockingQueue<Message>> input_queue_;
    std::unique_ptr<BlockingQueue<Message>> output_queue_;

    std::shared_ptr<InputAdapter> input_;
    std::shared_ptr<Processor>    processor_;
    std::shared_ptr<Router>       router_;

    bool initialized_;
};

} // namespace gateway

#endif // GATEWAY_GATEWAY_H
