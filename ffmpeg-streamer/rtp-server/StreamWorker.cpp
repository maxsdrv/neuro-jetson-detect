#include <iostream>
#include <opencv2/opencv.hpp>

#include "StreamWorker.h"

StreamWorker::StreamWorker(std::string rtpAddr) : _rtpAddr(std::move(rtpAddr)) {
    _cap.open(0);
    if (!_cap.isOpened()) {
        std::cerr << "Error opening video capture.\n";
    }
}

StreamWorker::~StreamWorker() {
    std::cout << __func__ << std::endl;
    closeCaptureStream();
}

void StreamWorker::testCaptureStream(std::stop_token stopToken) {

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


void StreamWorker::closeCaptureStream() {
    if (_captureThread.joinable()) {
        _captureThread.request_stop();
        _captureThread.join();
    }
}

void StreamWorker::runCaptureStream() {
    if (!_captureThread.joinable()) {
        _streamer = std::make_unique<FFMpegStreamer>(_rtpAddr, VideoQuality::HIGH);

        if (!_streamer->initializeH264()) {
            std::cerr << "Cannot initialize stream with H264 codec.\n";
            closeCaptureStream();
            return;
        }

        _captureThread = std::jthread(&StreamWorker::testCaptureStream, this);
    }
}





