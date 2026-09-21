#ifndef VEHICLESERVICE_HPP
#define VEHICLESERVICE_HPP

#include <v1/com/example/vehicle/VehicleStub.hpp>

class VehicleService :
                       public v1::com::example::vehicle::VehicleStub
{
public:
    VehicleService();

    v1::com::example::vehicle::VehicleStubRemoteEvent* initStubAdapter(
        const std::shared_ptr<v1::com::example::vehicle::VehicleStubAdapter>& _stubAdapter) override;

    const CommonAPI::Version& getInterfaceVersion(
        std::shared_ptr<CommonAPI::ClientId> _client) override;

    const uint32_t& getSpeedAttribute(
        std::shared_ptr<CommonAPI::ClientId> _client) override;

    void getSpeed(
        std::shared_ptr<CommonAPI::ClientId> _client,
        getSpeedReply_t _reply) override;

    void setSpeed(
        std::shared_ptr<CommonAPI::ClientId> _client,
        uint32_t _speed,
        setSpeedReply_t _reply) override;

    void notifySpeedChanged();

private:
    uint32_t speed_;
};

#endif