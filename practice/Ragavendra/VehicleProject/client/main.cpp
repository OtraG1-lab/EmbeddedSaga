#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include <CommonAPI/CommonAPI.hpp>
#include <v1/com/example/vehicle/VehicleProxy.hpp>

int main()
{
    auto runtime = CommonAPI::Runtime::get();

    auto proxy =
        runtime->buildProxy<
            v1::com::example::vehicle::VehicleProxy
            >(
            "com.example.vehicle",
            "VehicleService"
            );

    std::cout << "Waiting for Vehicle service..." << std::endl;

    while (!proxy->isAvailable())
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)
            );
    }

    std::cout << "Vehicle service is available!" << std::endl;

    CommonAPI::CallStatus status;
    uint32_t speed = 0;

    proxy->getSpeed(
        status,
        speed,
        nullptr
        );

    if (status == CommonAPI::CallStatus::SUCCESS)
    {
        std::cout << "Current speed = "
                  << speed
                  << std::endl;
    }
    else
    {
        std::cout << "getSpeed() failed!"
                  << std::endl;
    }

    return 0;
}