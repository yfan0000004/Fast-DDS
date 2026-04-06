#include "gateway/dds_input_adapter.h"
#include <iostream>

#ifdef HAS_FASTDDS
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#endif

namespace gateway {

#ifdef HAS_FASTDDS

TopicListener::TopicListener(const std::string& topic_name,
                             std::shared_ptr<BlockingQueue<MessagePtr>> input_queue)
    : topic_name_(topic_name)
    , input_queue_(input_queue) {}

void TopicListener::on_data_available(dds::DataReader* reader) {
    dds::SampleInfo info;
    void* data = nullptr;
    while (reader->take_next_sample(&data, &info) == ReturnCode_t::RETCODE_OK) {
        if (!info.valid_data) continue;

        auto msg = std::make_shared<Message>();
        msg->topic = topic_name_;
        msg->type_name = reader->get_topicdescription()->get_type_name();
        // Serialize data into msg->payload according to your IDL type
        msg->source_id = "dds";
        input_queue_->push(std::move(msg));
    }
}

#endif // HAS_FASTDDS

DdsInputAdapter::DdsInputAdapter(const DdsConfig& config,
                                 std::shared_ptr<BlockingQueue<MessagePtr>> input_queue)
    : config_(config)
    , input_queue_(input_queue)
#ifdef HAS_FASTDDS
    , participant_(nullptr)
    , subscriber_(nullptr)
#endif
{}

DdsInputAdapter::~DdsInputAdapter() {
    stop();
}

bool DdsInputAdapter::start() {
#ifdef HAS_FASTDDS
    participant_ = dds::DomainParticipantFactory::get_instance()
        ->create_participant(config_.domain_id, dds::PARTICIPANT_QOS_DEFAULT);
    if (!participant_) return false;

    subscriber_ = participant_->create_subscriber(dds::SUBSCRIBER_QOS_DEFAULT);
    if (!subscriber_) return false;

    for (const auto& topic_name : config_.topics) {
        auto listener = std::make_shared<TopicListener>(topic_name, input_queue_);
        listeners_.push_back(listener);

        auto topic = participant_->create_topic(topic_name, /* type_name */ "", dds::TOPIC_QOS_DEFAULT);
        auto reader = subscriber_->create_datareader(topic, dds::DATAREADER_QOS_DEFAULT, listener.get());
        readers_.push_back(reader);
    }
#else
    std::cout << "[DDS] stub mode (compile with -DHAS_FASTDDS to enable real DDS)" << std::endl;
    for (const auto& topic_name : config_.topics) {
        std::cout << "[DDS] would subscribe to: " << topic_name << std::endl;
    }
#endif
    return true;
}

void DdsInputAdapter::stop() {
#ifdef HAS_FASTDDS
    for (auto* reader : readers_) {
        subscriber_->delete_datareader(reader);
    }
    readers_.clear();
    listeners_.clear();

    if (subscriber_) {
        participant_->delete_subscriber(subscriber_);
        subscriber_ = nullptr;
    }
    if (participant_) {
        dds::DomainParticipantFactory::get_instance()->delete_participant(participant_);
        participant_ = nullptr;
    }
#endif
}

} // namespace gateway
