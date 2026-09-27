#include "VehicleSimulator.h"
#include <CommonAPI/CommonAPI.hpp>
#include "v1/com/example/vehicle/VehicleProxy.hpp"
#include <cmath>
#include <cstdint>
#include <QDebug>
#include <QMetaObject>
#include <QPointer>

class VehicleServiceClient
{
public:
    explicit VehicleServiceClient(VehicleSimulator *simulator)
        : m_simulator(simulator)
    {
        auto runtime = CommonAPI::Runtime::get();
        m_proxy = runtime->buildProxy<v1::com::example::vehicle::VehicleProxy>(
            "local", "VehicleInstance");

        m_proxy->getSpeedChangedEvent().subscribe(
            [simulator = m_simulator](uint32_t speed) {
                QMetaObject::invokeMethod(
                    simulator,
                    [simulator, speed]() {
                        if (simulator) {
                            simulator->setVehicleSpeedFromService(speed);
                        }
                    },
                    Qt::QueuedConnection);
            });
    }

    bool updateVehicleDetails()
    {
        if (!m_proxy->isAvailable()) {
            qWarning("Vehicle service unavailable; using local telemetry fallback");
            return m_proxy->isAvailable();
        }

        CommonAPI::CallStatus status;
        uint32_t speed = 0;
        uint32_t engineRpm = 0;
        std::string gear;
        double outsideTemp = 0.0;
        uint32_t batteryLevel = 0;
        m_proxy->getVehicleDetails(
            status, speed, engineRpm, gear, outsideTemp, batteryLevel, nullptr);
        if (status == CommonAPI::CallStatus::SUCCESS) {
            qInfo("Vehicle service read: speed=%u km/h, rpm=%u, gear=%s, outside=%.1f C, battery=%u%%",
                  speed,
                  engineRpm,
                  gear.c_str(),
                  outsideTemp,
                  batteryLevel);
            m_simulator->setVehicleSpeedFromService(speed);
            m_simulator->setVehicleDetailsFromService(
                engineRpm, QString::fromStdString(gear), outsideTemp, batteryLevel);
            return true;
        }

        qWarning("Vehicle service read failed with CommonAPI status %d",
                 static_cast<int>(status));
        return false;
    }

private:
    QPointer<VehicleSimulator> m_simulator;
    std::shared_ptr<v1::com::example::vehicle::VehicleProxy<>> m_proxy;
};

VehicleSimulator::VehicleSimulator(QObject *parent)
    : QObject(parent)
{
    m_vehicleClient = std::make_shared<VehicleServiceClient>(this);

    connect(&m_tickTimer, &QTimer::timeout, this, &VehicleSimulator::onTick);
    // 500ms simulation loop for smooth, lightweight updates on Raspberry Pi
    m_tickTimer.start(500);
}

void VehicleSimulator::setPosition(double lat, double lon, double heading)
{
    m_latitude = lat;
    m_longitude = lon;
    m_heading = heading;
}

void VehicleSimulator::setVehicleSpeedFromService(uint32_t speed)
{
    m_vehicleSpeed = static_cast<double>(speed);
}

void VehicleSimulator::setVehicleDetailsFromService(
    uint32_t engineRpm, const QString &gear, double outsideTemp, uint32_t batteryLevel)
{
    m_engineRpm = static_cast<double>(engineRpm);
    m_gear = gear;
    m_outsideTemp = outsideTemp;
    m_batteryLevel = static_cast<int>(batteryLevel);
}

void VehicleSimulator::onTick()
{
    m_tickCounter += 0.5;

    if (!m_vehicleClient->updateVehicleDetails()) {
        // Keep local telemetry moving while the CommonAPI service is offline.
        m_vehicleSpeed = 68.0 + 4.0 * std::sin(m_tickCounter * 0.1);
        m_engineRpm = 1900.0 + (m_vehicleSpeed - 68.0) * 45.0;
    }

    // Keep vehicle position fixed at current location
    // (GPS coordinates remain stationary unless actual vehicle movement / hardware GPS updates occur)
    // m_latitude and m_longitude stay at the current location

    // Media position advancement
    m_mediaPosition++;
    if (m_mediaPosition > m_mediaDuration) {
        m_mediaPosition = 0;
    }

    emit telemetryUpdated(m_vehicleSpeed, m_engineRpm, m_gear, m_outsideTemp, m_batteryLevel);
    emit gpsUpdated(m_latitude, m_longitude, m_heading, m_vehicleSpeed);
    emit mediaProgressUpdated(m_mediaPosition, m_mediaDuration);
}
