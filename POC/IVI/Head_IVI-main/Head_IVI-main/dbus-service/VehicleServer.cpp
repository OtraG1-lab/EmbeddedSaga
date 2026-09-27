#include <chrono>
#include <iostream>
#include <string>
#include <memory>
#include <thread>

#include <CommonAPI/CommonAPI.hpp>
#include "v1/com/example/vehicle/VehicleStubDefault.hpp"

class VehicleStub final : public v1::com::example::vehicle::VehicleStubDefault {
public:
    void getSpeed(std::shared_ptr<CommonAPI::ClientId>, getSpeedReply_t reply) override {
        reply(m_speed);
    }

    void setSpeed(std::shared_ptr<CommonAPI::ClientId>, uint32_t speed, setSpeedReply_t reply) override {
        m_speed = speed;
        fireSpeedChangedEvent(m_speed);
        reply();
    }

    void getVehicleDetails(
        std::shared_ptr<CommonAPI::ClientId>,
        getVehicleDetailsReply_t reply) override {
        reply(m_speed, m_engineRpm, m_gear, m_outsideTemp, m_batteryLevel);
    }

    void setVehicleDetails(
        std::shared_ptr<CommonAPI::ClientId>,
        uint32_t speed,
        uint32_t engineRpm,
        std::string gear,
        double outsideTemp,
        uint32_t batteryLevel,
        setVehicleDetailsReply_t reply) override {
        m_speed = speed;
        m_engineRpm = engineRpm;
        m_gear = gear;
        m_outsideTemp = outsideTemp;
        m_batteryLevel = batteryLevel;
        fireSpeedChangedEvent(m_speed);
        reply();
    }

    const uint32_t &getSpeedAttribute() override {
        return m_speed;
    }

    void setSpeedAttribute(uint32_t speed) override {
        m_speed = speed;
        fireSpeedChangedEvent(m_speed);
    }

private:
    uint32_t m_speed{0};
    uint32_t m_engineRpm{1900};
    std::string m_gear{"D"};
    double m_outsideTemp{21.5};
    uint32_t m_batteryLevel{88};
};

int main() {
    auto runtime = CommonAPI::Runtime::get();
    auto service = std::make_shared<VehicleStub>();
    runtime->registerService("local", "VehicleInstance", service);
    std::cout << "Vehicle DBus service running" << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}
