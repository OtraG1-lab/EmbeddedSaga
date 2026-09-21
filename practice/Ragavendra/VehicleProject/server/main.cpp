#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include <CommonAPI/CommonAPI.hpp>

#include "VehicleService.hpp"

int main()
{
    auto runtime = CommonAPI::Runtime::get();

    auto service = std::make_shared<VehicleService>();

    bool registered = runtime->registerService(
        "com.example.vehicle",
        "VehicleService",
        service
        );

    if (!registered)
    {
        std::cerr << "Failed to register Vehicle service!"
                  << std::endl;
        return 1;
    }

    std::cout << "Vehicle service registered successfully!"
              << std::endl;

    std::cout << "Vehicle server is running..."
              << std::endl;

    while (true)
    {
        std::this_thread::sleep_for(
            std::chrono::seconds(1)
            );
    }

    return 0;
}