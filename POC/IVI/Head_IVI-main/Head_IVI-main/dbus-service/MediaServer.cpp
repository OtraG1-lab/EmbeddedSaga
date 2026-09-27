#include <chrono>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <gst/gst.h>
#include <CommonAPI/CommonAPI.hpp>
#include "v1/org/example/MediaPlayerStubDefault.hpp"

namespace fs = std::filesystem;

namespace {

bool isMediaFile(const fs::directory_entry &entry)
{
    if (!entry.is_regular_file()) {
        return false;
    }

    auto extension = entry.path().extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });
    return extension == ".mp4" || extension == ".mkv" || extension == ".mp3" ||
           extension == ".wav" || extension == ".avi" || extension == ".webm";
}

std::vector<fs::path> findMediaFiles(const fs::path &directory)
{
    std::vector<fs::path> files;
    if (!fs::is_directory(directory)) {
        return files;
    }

    for (const auto &entry : fs::directory_iterator(directory)) {
        if (isMediaFile(entry)) {
            files.push_back(entry.path());
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}

}

class MediaPlayerStub final : public v1::org::example::MediaPlayerStubDefault {
public:
    explicit MediaPlayerStub(fs::path mediaDirectory)
        : m_mediaDirectory(std::move(mediaDirectory))
    {
    }

    ~MediaPlayerStub() override
    {
        stopPipeline();
    }

    void Play(std::shared_ptr<CommonAPI::ClientId>, PlayReply_t reply) override {
        const auto files = findMediaFiles(m_mediaDirectory);
        if (files.empty()) {
            std::cerr << "No supported media files found in "
                      << m_mediaDirectory << '\n';
            m_playbackStatus = "Error";
            reply();
            return;
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_pipeline) {
            m_pipeline = gst_element_factory_make("playbin", "media-player");
        }
        if (!m_pipeline) {
            m_playbackStatus = "Error";
            reply();
            return;
        }

        const char *configuredFile = std::getenv("MEDIA_FILE");
        const fs::path selectedFile = configuredFile ? configuredFile : files.front();
        if (!fs::is_regular_file(selectedFile) || !isMediaFile(fs::directory_entry(selectedFile))) {
            m_playbackStatus = "Error";
            reply();
            return;
        }

        gchar *fileUri = gst_filename_to_uri(fs::absolute(selectedFile).string().c_str(), nullptr);
        g_object_set(m_pipeline, "uri", fileUri, nullptr);
        g_free(fileUri);
        gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
        m_currentFile = selectedFile;
        m_playbackStatus = "Playing";
        reply();
    }

    void Pause(std::shared_ptr<CommonAPI::ClientId>, PauseReply_t reply) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_pipeline) {
            gst_element_set_state(m_pipeline, GST_STATE_PAUSED);
            m_playbackStatus = "Paused";
        }
        reply();
    }

    void Stop(std::shared_ptr<CommonAPI::ClientId>, StopReply_t reply) override {
        stopPipeline();
        reply();
    }

    const std::string &getPlaybackStatusAttribute() override {
        return m_playbackStatus;
    }

private:
    void stopPipeline()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_pipeline) {
            gst_element_set_state(m_pipeline, GST_STATE_NULL);
            gst_object_unref(m_pipeline);
            m_pipeline = nullptr;
        }
        m_playbackStatus = "Stopped";
    }

    fs::path m_mediaDirectory;
    fs::path m_currentFile;
    GstElement *m_pipeline{nullptr};
    std::mutex m_mutex;
    std::string m_playbackStatus{"Stopped"};
};

int main(int argc, char **argv) {
    gst_init(&argc, &argv);
    const char *configuredDirectory = std::getenv("MEDIA_DIR");
    const fs::path mediaDirectory = configuredDirectory ? configuredDirectory : "/media";

    auto runtime = CommonAPI::Runtime::get();
    auto service = std::make_shared<MediaPlayerStub>(mediaDirectory);
    runtime->registerService("local", "MediaPlayerInstance", service);
    std::cout << "Media DBus service running, media directory: "
              << mediaDirectory << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}
