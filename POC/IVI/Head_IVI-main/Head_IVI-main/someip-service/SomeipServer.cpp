#include <iostream>
#include <thread>
#include <chrono>
#include <CommonAPI/CommonAPI.hpp>
#include "src-gen/v1/commonapi/someip/SomeipServiceStubDefault.hpp"

class SomeipServiceStubImpl: public v1::commonapi::someip::SomeipServiceStubDefault {
public:
    void transmitTelemetry(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _value, transmitTelemetryReply_t _reply) override {
        std::cout << "[SOME/IP] Telemetry value received: " << _value << std::endl;
        _reply();
    }
};

int main() {
    auto runtime = CommonAPI::Runtime::get();
    auto myService = std::make_shared<SomeipServiceStubImpl>();
    runtime->registerService("local", "SomeipServiceInstance", myService);
    while (true) { std::this_thread::sleep_for(std::chrono::seconds(30)); }
    return 0;
}
