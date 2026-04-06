#ifndef GATEWAY_DDS_INPUT_ADAPTER_H
#define GATEWAY_DDS_INPUT_ADAPTER_H

#include "gateway/input_adapter.h"
#include "gateway/blocking_queue.h"
#include "gateway/message.h"

#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>

#include <string>
#include <vector>
#include <memory>

namespace gateway {

namespace dds = eprosima::fastdds::dds;

struct DdsConfig {
    int domain_id;
    std::vector<std::string> topics;

    DdsConfig() : domain_id(0) {}
};

class TopicListener : public dds::DataReaderListener {
public:
    TopicListener(const std::string& topic_name,
                  std::shared_ptr<BlockingQueue<MessagePtr>> input_queue);

    void on_data_available(dds::DataReader* reader) override;

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

    dds::DomainParticipant* participant_;
    dds::Subscriber* subscriber_;
    std::vector<dds::DataReader*> readers_;
    std::vector<std::shared_ptr<TopicListener>> listeners_;
};

} // namespace gateway

#endif // GATEWAY_DDS_INPUT_ADAPTER_H
