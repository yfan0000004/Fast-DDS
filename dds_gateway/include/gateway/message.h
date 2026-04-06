#ifndef GATEWAY_MESSAGE_H
#define GATEWAY_MESSAGE_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <chrono>
#include <memory>
#include <functional>

namespace gateway {

struct Message {
    std::string topic;
    std::string type_name;              // IDL type identifier, e.g. "sensor::IMU", "sensor::PointCloud"
    std::vector<uint8_t> payload;       // serialized bytes
    std::map<std::string, std::string> properties;  // extensible key-value metadata
    uint64_t timestamp_ms;
    std::string source_id;

    Message()
        : timestamp_ms(0) {}

    Message(const std::string& topic,
            const std::string& type_name,
            const std::vector<uint8_t>& payload,
            const std::string& source_id = "")
        : topic(topic)
        , type_name(type_name)
        , payload(payload)
        , timestamp_ms(static_cast<uint64_t>(
              std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now().time_since_epoch()).count()))
        , source_id(source_id) {}
};

using MessagePtr    = std::shared_ptr<Message>;
using FilterFunc    = std::function<bool(const MessagePtr&)>;
using TransformFunc = std::function<MessagePtr(const MessagePtr&)>;

} // namespace gateway

#endif // GATEWAY_MESSAGE_H
