#pragma once

#include <QObject>
#include <QTimer>
#include <cstdint>
#include <memory>

class VehicleServiceClient;

class VehicleSimulator : public QObject
{
    Q_OBJECT

public:
    explicit VehicleSimulator(QObject *parent = nullptr);

    double vehicleSpeed() const { return m_vehicleSpeed; }
    double engineRpm() const { return m_engineRpm; }
    QString gear() const { return m_gear; }
    double outsideTemp() const { return m_outsideTemp; }
    int batteryLevel() const { return m_batteryLevel; }

    double latitude() const { return m_latitude; }
    double longitude() const { return m_longitude; }
    double heading() const { return m_heading; }

    int mediaPosition() const { return m_mediaPosition; }
    int mediaDuration() const { return m_mediaDuration; }

    void setPosition(double lat, double lon, double heading = 42.0);

signals:
    void telemetryUpdated(double speed, double rpm, const QString &gear, double temp, int battery);
    void gpsUpdated(double lat, double lon, double heading, double speed);
    void mediaProgressUpdated(int position, int duration);

private slots:
    void onTick();

private:
    friend class VehicleServiceClient;

    void setVehicleSpeedFromService(uint32_t speed);
    void setVehicleDetailsFromService(
        uint32_t engineRpm, const QString &gear, double outsideTemp, uint32_t batteryLevel);

    QTimer m_tickTimer;
    std::shared_ptr<VehicleServiceClient> m_vehicleClient;
    double m_vehicleSpeed{64.0};
    double m_engineRpm{2100.0};
    QString m_gear{"D"};
    double m_outsideTemp{21.5};
    int m_batteryLevel{88};

    double m_latitude{12.9719445};
    double m_longitude{77.5936873};
    double m_heading{42.0};

    int m_mediaPosition{142};
    const int m_mediaDuration{245};
    double m_tickCounter{0.0};
};
