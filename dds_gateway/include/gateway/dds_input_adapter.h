#ifndef GATEWAY_DDS_INPUT_ADAPTER_H
#define GATEWAY_DDS_INPUT_ADAPTER_H

#include "gateway/input_adapter.h"
#include "gateway/blocking_queue.h"
#include "gateway/message.h"

#include <string>
#include <vector>
#include <memory>

namespace gateway {

struct DdsConfig {
    int domain_id;
    std::vector<std::string> topics;

    DdsConfig() : domain_id(0) {}
};

// Per-topic callback handler. In real DDS this would inherit from
// eprosima::fastdds::dds::DataReaderListener.
// One instance per topic, all sharing the same input_queue.
class TopicListener {
public:
    TopicListener(const std::string& topic_name,
                  std::shared_ptr<BlockingQueue<MessagePtr>> input_queue);

    // Called by DDS middleware when new data arrives on this topic.
    void on_data_available(/* DataReader* reader */);

private:
    std::string topic_name_;
    std::shared_ptr<BlockingQueue<MessagePtr>> input_queue_;
};

class DdsInputAdapter : public InputAdapter {
public:
    DdsInputAdapter(const DdsConfig& config,
                    std::shared_ptr<BlockingQueue<MessagePtr>> input_queue);
    ~DdsInputAdapter();

    bool start() override;
    void stop() override;

private:
    DdsConfig config_;
    std::shared_ptr<BlockingQueue<MessagePtr>> input_queue_;

    // One listener per topic
    std::vector<std::shared_ptr<TopicListener>> listeners_;

    // DDS entities (opaque pointers for skeleton)
    // In real code these would be:
    //   DomainParticipant* participant_;
    //   Subscriber* subscriber_;
    //   std::vector<DataReader*> readers_;
};

} // namespace gateway

#endif // GATEWAY_DDS_INPUT_ADAPTER_H
