#include "VehicleService.hpp"

VehicleService::VehicleService()
    : speed_(80)
{
}

v1::com::example::vehicle::VehicleStubRemoteEvent*
VehicleService::initStubAdapter(
    const std::shared_ptr<v1::com::example::vehicle::VehicleStubAdapter>& _stubAdapter)
{
    stubAdapter_ = _stubAdapter;

    return nullptr;
}

const CommonAPI::Version& VehicleService::getInterfaceVersion(
    std::shared_ptr<CommonAPI::ClientId> _client)
{
    (void)_client;

    static const CommonAPI::Version version(1, 0);

    return version;
}

const uint32_t& VehicleService::getSpeedAttribute(
    std::shared_ptr<CommonAPI::ClientId> _client)
{
    (void)_client;

    return speed_;
}

void VehicleService::getSpeed(
    std::shared_ptr<CommonAPI::ClientId> _client,
    getSpeedReply_t _reply)
{
    (void)_client;

    _reply(speed_);
}

void VehicleService::setSpeed(
    std::shared_ptr<CommonAPI::ClientId> _client,
    uint32_t _speed,
    setSpeedReply_t _reply)
{
    (void)_client;

    speed_ = _speed;

    fireSpeedAttributeChanged(speed_);
    fireSpeedChangedEvent(speed_);

    _reply();
}

void VehicleService::notifySpeedChanged()
{
    fireSpeedChangedEvent(speed_);
}