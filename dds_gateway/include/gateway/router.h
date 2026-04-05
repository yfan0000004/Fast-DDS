#ifndef GATEWAY_ROUTER_H
#define GATEWAY_ROUTER_H

#include "gateway/message.h"
#include "gateway/blocking_queue.h"
#include "gateway/output_adapter.h"
#include "gateway/config.h"

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <atomic>

namespace gateway {

// Router owns the output_queue, consumes messages from it,
// and dispatches to the correct OutputAdapter based on RouteRules.
class Router {
public:
    explicit Router(BlockingQueue<Message>& output_queue);
    ~Router();

    // One-shot setup: register named outputs + set route rules together.
    void add_output(const std::string& name, std::shared_ptr<OutputAdapter> adapter);
    void set_routes(const std::vector<RouteRule>& rules);

    bool connect_all();
    void disconnect_all();

    bool start();
    void stop();

private:
    void dispatch_loop();
    bool dispatch(const Message& msg);

    BlockingQueue<Message>& output_queue_;
    std::map<std::string, std::shared_ptr<OutputAdapter>> outputs_;
    std::vector<RouteRule> rules_;
    std::thread worker_;
    std::atomic<bool> running_;
};

} // namespace gateway

#endif // GATEWAY_ROUTER_H
