#ifndef GATEWAY_MESSAGE_H
#define GATEWAY_MESSAGE_H

#include <string>
#include <vector>
#include <cstdint>
#include <chrono>

namespace gateway {

struct Message {
    std::string topic;
    std::vector<uint8_t> payload;
    uint64_t timestamp_ms;
    std::string source_id;

    Message()
        : timestamp_ms(0) {}

    Message(const std::string& topic,
            const std::vector<uint8_t>& payload,
            const std::string& source_id = "")
        : topic(topic)
        , payload(payload)
        , timestamp_ms(static_cast<uint64_t>(
              std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now().time_since_epoch()).count()))
        , source_id(source_id) {}
};

} // namespace gateway

#endif // GATEWAY_MESSAGE_H
