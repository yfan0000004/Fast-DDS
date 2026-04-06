#ifndef GATEWAY_OUTPUT_ADAPTER_H
#define GATEWAY_OUTPUT_ADAPTER_H

#include "gateway/message.h"

namespace gateway {

class OutputAdapter {
public:
    virtual ~OutputAdapter() {}
    virtual bool connect() = 0;
    virtual bool send(const MessagePtr& msg) = 0;
    virtual void disconnect() = 0;
};

} // namespace gateway

#endif // GATEWAY_OUTPUT_ADAPTER_H
