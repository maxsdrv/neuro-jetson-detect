#ifndef FFMPEG_STREAMER_STREAMWORKER_H
#define FFMPEG_STREAMER_STREAMWORKER_H

#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <opencv2/opencv.hpp>

#ifdef ENABLE_FFMPEG
#include <FFMpegStreamer.h>
#endif

#ifdef ENABLE_GSTREAMER
#include <gst_sender.h>
#endif

#include "IStreamer.h"

template <typename TStreamer>
class StreamWorker {
public:
    explicit StreamWorker(std::string rtpAddr);
    ~StreamWorker();

    void runCaptureStream();
    void closeCaptureStream();

private:
    std::jthread _captureThread;
    std::mutex _mutex;
    std::condition_variable _condVar;
    std::queue<std::string> _commandQueue;
    std::unique_ptr<IStreamer> _streamer;
    std::string _rtpAddr;
    cv::VideoCapture _cap;

    void testCaptureStream(std::stop_token stopToken);
};


#endif //FFMPEG_STREAMER_STREAMWORKER_H
