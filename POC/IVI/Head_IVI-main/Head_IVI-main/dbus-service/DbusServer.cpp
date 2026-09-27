#include <iostream>
#include <thread>
#include <chrono>
#include <CommonAPI/CommonAPI.hpp>
#include "src-gen/v1/commonapi/dbus/DbusServiceStubDefault.hpp"

class DbusServiceStubImpl: public v1::commonapi::dbus::DbusServiceStubDefault {
public:
    void triggerAction(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _command, triggerActionReply_t _reply) override {
        std::cout << "[D-Bus] Command executed: " << _command << std::endl;
        _reply(true);
    }
};

int main() {
    auto runtime = CommonAPI::Runtime::get();
    auto myService = std::make_shared<DbusServiceStubImpl>();
    runtime->registerService("local", "DbusServiceInstance", myService);
    while (true) { std::this_thread::sleep_for(std::chrono::seconds(30)); }
    return 0;
}
