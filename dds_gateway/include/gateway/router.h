#ifndef GATEWAY_ROUTER_H
#define GATEWAY_ROUTER_H

#include "gateway/message.h"
#include "gateway/output_adapter.h"
#include "gateway/config.h"

#include <map>
#include <vector>
#include <string>
#include <memory>

namespace gateway {

// Routes each message to one or more OutputAdapters based on RouteRules.
// Matching order: first exact match wins; "*" is the fallback.
class Router {
public:
    void register_output(const std::string& name, std::shared_ptr<OutputAdapter> adapter);
    void set_rules(const std::vector<RouteRule>& rules);

    bool connect_all();
    void disconnect_all();

    // Dispatch a message: find matching outputs by topic, send to each.
    // Returns false if ALL matched sends fail.
    bool dispatch(const Message& msg);

private:
    std::map<std::string, std::shared_ptr<OutputAdapter>> outputs_;
    std::vector<RouteRule> rules_;
};

} // namespace gateway

#endif // GATEWAY_ROUTER_H
