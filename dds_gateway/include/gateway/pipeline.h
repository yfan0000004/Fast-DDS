#ifndef GATEWAY_PIPELINE_H
#define GATEWAY_PIPELINE_H

#include "gateway/blocking_queue.h"
#include "gateway/message.h"
#include "gateway/input_adapter.h"
#include "gateway/processor.h"
#include "gateway/router.h"

#include <memory>

namespace gateway {

// Pipeline is a lifecycle manager that owns the two queues and
// coordinates start/stop of all stages:
//   InputAdapter -> [input_queue] -> Processor -> [output_queue] -> Router -> outputs
class Pipeline {
public:
    explicit Pipeline(size_t queue_size = 4096);
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    BlockingQueue<Message>& input_queue()  { return input_queue_; }
    BlockingQueue<Message>& output_queue() { return output_queue_; }

    void set_input(std::shared_ptr<InputAdapter> input);
    void set_processor(std::shared_ptr<Processor> processor);
    void set_router(std::shared_ptr<Router> router);

    bool start();
    void stop();

private:
    BlockingQueue<Message> input_queue_;
    BlockingQueue<Message> output_queue_;

    std::shared_ptr<InputAdapter> input_;
    std::shared_ptr<Processor>    processor_;
    std::shared_ptr<Router>       router_;
};

} // namespace gateway

#endif // GATEWAY_PIPELINE_H
