#ifndef FFMPEG_STREAMER_STREAMWORKER_H
#define FFMPEG_STREAMER_STREAMWORKER_H

#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <opencv2/opencv.hpp>


template <typename TStreamer>
class StreamWorker {
public:
    template<typename... Args>
    StreamWorker(Args&&... args);

    ~StreamWorker();

    void runCaptureStream(std::stop_token stoken);
    void closeCaptureStream();

private:
    std::jthread _captureThread{};
    std::mutex _mutex{};
    std::condition_variable _condVar{};
    std::queue<std::string> _commandQueue{};
    std::unique_ptr<TStreamer> _streamer{};
    std::string _rtpAddr{};
    cv::VideoCapture _cap{};
};

/**
 *
 * @tparam TStreamer - FFmpegAdapter or GstreamerAdapter
 * @param args - constructor arguments for TStreamer
 */
template<typename TStreamer>
template<typename... Args>
StreamWorker<TStreamer>::StreamWorker(Args &&... args)
    : _streamer{std::make_unique<TStreamer>(std::forward<Args>(args)...)}
{
    _cap.open(0);
    if (!_cap.isOpened()) {
        std::cerr << "Error opening video capture.\n";
    }
}

template<typename TStreamer>
StreamWorker<TStreamer>::~StreamWorker() {
    std::cout << __func__ << std::endl;
    closeCaptureStream();
}


template<typename TStreamer>
void StreamWorker<TStreamer>::closeCaptureStream() {
    std::cout << "Stream is closed successfully.\n";
}

template<typename TStreamer>
void StreamWorker<TStreamer>::runCaptureStream(std::stop_token stoken) {
    if (!_streamer->initializeH264()) {
        std::cerr << "cannot initialize stream with h264 codec.\n";
        closeCaptureStream();
        return;
    }
    auto [width,  height]= _streamer->frameSize();

    while (!stoken.stop_requested()) {
        cv::Mat frame;
        cv::Mat yuy_frame;
        _cap >> frame;

        if (!frame.empty()) {
            cv::resize(frame, frame, cv::Size(width, height));
            cv::cvtColor(frame, yuy_frame, cv::COLOR_BGR2YUV_I420);
            std::vector<unsigned char> data_frame{yuy_frame.data, yuy_frame.data + yuy_frame.total() * yuy_frame.elemSize()};
            if (!_streamer->sendFrame(data_frame)) {
                std::cerr << "Interrupted frame transmission.\n";
            }
        }
        else {
            std::cerr << "Empty frame.\n";
            break;
        }
    }
    closeCaptureStream();
}



#endif //FFMPEG_STREAMER_STREAMWORKER_H
