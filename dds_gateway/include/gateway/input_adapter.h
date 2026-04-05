#ifndef GATEWAY_INPUT_ADAPTER_H
#define GATEWAY_INPUT_ADAPTER_H

namespace gateway {

class InputAdapter {
public:
    virtual ~InputAdapter() {}
    virtual bool start() = 0;
    virtual void stop() = 0;
};

} // namespace gateway

#endif // GATEWAY_INPUT_ADAPTER_H
