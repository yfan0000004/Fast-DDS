#include "gateway/dds_input_adapter.h"
#include <iostream>

namespace gateway {

// --- TopicListener ---

TopicListener::TopicListener(const std::string& topic_name,
                             std::shared_ptr<BlockingQueue<MessagePtr>> input_queue)
    : topic_name_(topic_name)
    , input_queue_(input_queue) {}

void TopicListener::on_data_available(/* DataReader* reader */) {
    // TODO: replace with actual DDS take() call
    // In real implementation:
    //
    //   SampleInfo info;
    //   while (reader->take_next_sample(&data, &info) == ReturnCode_t::RETCODE_OK) {
    //       if (info.valid_data) {
    //           auto msg = std::make_shared<Message>(
    //               topic_name_,
    //               data.type_name(),    // IDL type identifier
    //               serialize(data),     // payload bytes
    //               "dds");
    //           msg->properties["instance_handle"] = std::to_string(info.instance_handle);
    //           input_queue_->push(std::move(msg));
    //       }
    //   }
}

// --- DdsInputAdapter ---

DdsInputAdapter::DdsInputAdapter(const DdsConfig& config,
                                 std::shared_ptr<BlockingQueue<MessagePtr>> input_queue)
    : config_(config)
    , input_queue_(input_queue) {}

DdsInputAdapter::~DdsInputAdapter() {
    stop();
}

bool DdsInputAdapter::start() {
    // TODO: real DDS initialization:
    //
    // 1) Create DomainParticipant
    //   participant_ = DomainParticipantFactory::get_instance()
    //       ->create_participant(config_.domain_id, PARTICIPANT_QOS_DEFAULT);
    //
    // 2) Create Subscriber
    //   subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT);
    //
    // 3) For each topic, create a DataReader with its own TopicListener
    for (const auto& topic_name : config_.topics) {
        auto listener = std::make_shared<TopicListener>(topic_name, input_queue_);
        listeners_.push_back(listener);

        // TODO:
        //   auto topic = participant_->create_topic(topic_name, type_name, TOPIC_QOS_DEFAULT);
        //   auto reader = subscriber_->create_datareader(topic, DATAREADER_QOS_DEFAULT, listener.get());
        //   readers_.push_back(reader);

        std::cout << "[DDS] subscribed to topic: " << topic_name << std::endl;
    }

    return true;
}

void DdsInputAdapter::stop() {
    // TODO: real DDS teardown:
    //   for (auto* reader : readers_) subscriber_->delete_datareader(reader);
    //   participant_->delete_subscriber(subscriber_);
    //   DomainParticipantFactory::get_instance()->delete_participant(participant_);

    listeners_.clear();
    std::cout << "[DDS] all listeners stopped" << std::endl;
}

} // namespace gateway
