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

    void runCaptureStream();
    void closeCaptureStream();

private:
    std::jthread _captureThread;
    std::mutex _mutex;
    std::condition_variable _condVar;
    std::queue<std::string> _commandQueue;
    std::unique_ptr<TStreamer> _streamer;
    std::string _rtpAddr;
    cv::VideoCapture _cap;

    void testCaptureStream();
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
void StreamWorker<TStreamer>::testCaptureStream() {

    auto [width,  height]= _streamer->frameSize();

    while (true) {
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
}


template<typename TStreamer>
void StreamWorker<TStreamer>::closeCaptureStream() {
    if (_captureThread.joinable()) {
        _captureThread.request_stop();
        _captureThread.join();
    }
}

template<typename TStreamer>
void StreamWorker<TStreamer>::runCaptureStream() {
    if (!_captureThread.joinable()) {

        if (!_streamer->initializeH264()) {
            std::cerr << "cannot initialize stream with h264 codec.\n";
            closeCaptureStream();
            return;
        }

        _captureThread = std::jthread(&StreamWorker::testCaptureStream, this);
    }
}



#endif //FFMPEG_STREAMER_STREAMWORKER_H
