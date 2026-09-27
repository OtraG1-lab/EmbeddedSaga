#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

#include <CommonAPI/CommonAPI.hpp>
#include "v1/org/example/MediaPlayerProxy.hpp"

namespace {

void printUsage(const char *program) {
    std::cout << "Usage: " << program << " <play|pause|stop|status>\n";
}

bool printStatus(const std::shared_ptr<v1::org::example::MediaPlayerProxy<>> &proxy) {
    CommonAPI::CallStatus status;
    std::string playbackStatus;
    proxy->getPlaybackStatusAttribute().getValue(status, playbackStatus, nullptr);
    if (status != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "Reading PlaybackStatus failed with status "
                  << static_cast<int>(status) << '\n';
        return false;
    }

    std::cout << "PlaybackStatus=" << playbackStatus << '\n';
    return true;
}

}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    auto runtime = CommonAPI::Runtime::get();
    auto proxy = runtime->buildProxy<v1::org::example::MediaPlayerProxy>(
        "local", "MediaPlayerInstance");

    while (!proxy->isAvailable()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    const std::string command(argv[1]);
    CommonAPI::CallStatus status;
    if (command == "play") {
        proxy->Play(status, nullptr);
    } else if (command == "pause") {
        proxy->Pause(status, nullptr);
    } else if (command == "stop") {
        proxy->Stop(status, nullptr);
    } else if (command != "status") {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    if (command != "status" && status != CommonAPI::CallStatus::SUCCESS) {
        std::cerr << "Media command failed with status "
                  << static_cast<int>(status) << '\n';
        return EXIT_FAILURE;
    }

    return printStatus(proxy) ? EXIT_SUCCESS : EXIT_FAILURE;
}
