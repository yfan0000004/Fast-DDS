#include "gateway/dds_gateway.h"
#include "gateway/dds_input_adapter.h"
#include "gateway/tcp_output_adapter.h"
#include "gateway/mqtt_output_adapter.h"

#include <iostream>

namespace gateway {

DdsGateway::DdsGateway()
    : initialized_(false) {}

DdsGateway::~DdsGateway() {
    stop();
}

bool DdsGateway::init(const std::string& config_path) {
    try {
        config_ = GatewayConfig::load(config_path);
    } catch (const std::exception& e) {
        std::cerr << "[DdsGateway] failed to load config: " << e.what() << std::endl;
        return false;
    }

    input_queue_  = std::make_shared<BlockingQueue<Message>>(config_.queue_size);
    output_queue_ = std::make_shared<BlockingQueue<Message>>(config_.queue_size);

    input_ = std::make_shared<DdsInputAdapter>(config_.dds, input_queue_);

    processor_ = std::make_shared<Processor>(input_queue_, output_queue_);

    router_ = std::make_shared<Router>(output_queue_);
    if (config_.tcp_enabled) {
        router_->add_output("tcp", std::make_shared<TcpOutputAdapter>(config_.tcp));
    }
    if (config_.mqtt_enabled) {
        router_->add_output("mqtt", std::make_shared<MqttOutputAdapter>(config_.mqtt));
    }
    router_->set_routes(config_.routes);

    initialized_ = true;
    return true;
}

void DdsGateway::set_filter(FilterFunc filter) {
    if (processor_) {
        processor_->set_filter(filter);
    }
}

void DdsGateway::set_transform(TransformFunc transform) {
    if (processor_) {
        processor_->set_transform(transform);
    }
}

bool DdsGateway::start() {
    if (!initialized_) {
        std::cerr << "[DdsGateway] not initialized, call init() first" << std::endl;
        return false;
    }

    if (!router_->connect_all()) {
        std::cerr << "[DdsGateway] router connect failed" << std::endl;
        return false;
    }
    router_->start();
    processor_->start();
    input_->start();

    std::cout << "[DdsGateway] started" << std::endl;
    return true;
}

void DdsGateway::stop() {
    if (!initialized_) return;

    if (input_)     input_->stop();
    if (processor_) processor_->stop();
    if (router_)    router_->stop();
    if (router_)    router_->disconnect_all();

    std::cout << "[DdsGateway] stopped" << std::endl;
}

} // namespace gateway
