#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <CommonAPI/CommonAPI.hpp>
#include "v1/com/example/vehicle/VehicleProxy.hpp"

namespace {

void printUsage(const char *program) {
    std::cout << "Usage: " << program << " <command> [value]\n"
              << "  get              Read speed and vehicle details\n"
              << "  speed <km/h>     Set speed, then read vehicle details\n"
              << "  set-details <speed> <rpm> <gear> <outside-C> <battery-%>\n"
              << "                   Set all vehicle details, then read them\n"
              << "  watch [count]     Read details repeatedly (default: 10)\n";
}

bool printVehicleDetails(const std::shared_ptr<v1::com::example::vehicle::VehicleProxy<>> &proxy) {
    CommonAPI::CallStatus status;
    uint32_t speed = 0;
    uint32_t engineRpm = 0;
    std::string gear;
    double outsideTemp = 0.0;
    uint32_t batteryLevel = 0;
    proxy->getVehicleDetails(
        status, speed, engineRpm, gear, outsideTemp, batteryLevel, nullptr);

    if (status != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "getVehicleDetails failed with status "
                  << static_cast<int>(status) << '\n';
        return false;
    }

    std::cout << "speed=" << speed << " km/h, rpm=" << engineRpm
              << ", gear=" << gear << ", outside=" << outsideTemp
              << " C, battery=" << batteryLevel << "%\n";
    return true;
}

}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    auto runtime = CommonAPI::Runtime::get();
    auto proxy = runtime->buildProxy<v1::com::example::vehicle::VehicleProxy>(
        "local", "VehicleInstance");

    while (!proxy->isAvailable()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    const std::string command(argv[1]);
    if (command == "get") {
        return printVehicleDetails(proxy) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    if (command == "speed" && argc == 3) {
        const auto speed = static_cast<uint32_t>(std::strtoul(argv[2], nullptr, 10));
        CommonAPI::CallStatus status;
        proxy->setSpeed(speed, status, nullptr);
        if (status != CommonAPI::CallStatus::SUCCESS) {
            std::cerr << "setSpeed failed with status "
                      << static_cast<int>(status) << '\n';
            return EXIT_FAILURE;
        }
        return printVehicleDetails(proxy) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    if (command == "set-details" && argc == 7) {
        const auto speed = static_cast<uint32_t>(std::strtoul(argv[2], nullptr, 10));
        const auto engineRpm = static_cast<uint32_t>(std::strtoul(argv[3], nullptr, 10));
        const std::string gear(argv[4]);
        const auto outsideTemp = std::strtod(argv[5], nullptr);
        const auto batteryLevel = static_cast<uint32_t>(std::strtoul(argv[6], nullptr, 10));

        CommonAPI::CallStatus status;
        proxy->setVehicleDetails(
            speed, engineRpm, gear, outsideTemp, batteryLevel, status, nullptr);
        if (status != CommonAPI::CallStatus::SUCCESS) {
            std::cerr << "setVehicleDetails failed with status "
                      << static_cast<int>(status) << '\n';
            return EXIT_FAILURE;
        }
        return printVehicleDetails(proxy) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    if (command == "watch") {
        int count = 10;
        if (argc == 3) {
            count = std::atoi(argv[2]);
        }
        for (int index = 0; index < count; ++index) {
            if (!printVehicleDetails(proxy)) {
                return EXIT_FAILURE;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        return EXIT_SUCCESS;
    }

    printUsage(argv[0]);
    return EXIT_FAILURE;
}
