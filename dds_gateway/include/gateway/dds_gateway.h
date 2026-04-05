#ifndef GATEWAY_DDS_GATEWAY_H
#define GATEWAY_DDS_GATEWAY_H

#include "gateway/blocking_queue.h"
#include "gateway/message.h"
#include "gateway/config.h"
#include "gateway/input_adapter.h"
#include "gateway/processor.h"
#include "gateway/router.h"

#include <memory>
#include <string>

namespace gateway {

// DdsGateway is the single entry point of the application.
// It reads config, creates all internal components, and manages their lifecycle.
//
// Usage:
//   DdsGateway gw;
//   gw.init("config.json");
//   gw.start();
//   // ... wait for signal ...
//   gw.stop();
class DdsGateway {
public:
    DdsGateway();
    ~DdsGateway();

    DdsGateway(const DdsGateway&) = delete;
    DdsGateway& operator=(const DdsGateway&) = delete;

    bool init(const std::string& config_path);

    void set_filter(FilterFunc filter);
    void set_transform(TransformFunc transform);

    bool start();
    void stop();

private:
    GatewayConfig config_;

    std::shared_ptr<BlockingQueue<Message>> input_queue_;
    std::shared_ptr<BlockingQueue<Message>> output_queue_;

    std::shared_ptr<InputAdapter> input_;
    std::shared_ptr<Processor>    processor_;
    std::shared_ptr<Router>       router_;

    bool initialized_;
};

} // namespace gateway

#endif // GATEWAY_DDS_GATEWAY_H
