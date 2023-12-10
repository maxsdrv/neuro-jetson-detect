#include <opencv2/opencv.hpp>
#include <thread>
#include <fstream>

#include <FFMpegReceiver.h>
#include <FFMpegStreamer.h>

void test_capture_stream(FFMpegStreamer* sender) {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video capture.\n";
        return;
    }

    auto [width,  height]= sender->frameSize();

    while (true) {
        cv::Mat frame;
        cv::Mat yuy_frame;
        cap >> frame;

        if (!frame.empty()) {
            cv::resize(frame, frame, cv::Size(width, height));
            cv::cvtColor(frame, yuy_frame, cv::COLOR_BGR2YUV_I420);
            std::vector<unsigned char> data_frame{yuy_frame.data, yuy_frame.data + yuy_frame.total() * yuy_frame.elemSize()};
            if (!sender->sendFrame(data_frame)) {
                std::cerr << "Interrupted frame transmission.\n";
            }
        }
        else {
            std::cerr << "Empty frame.\n";
            break;
        }
    }
}

void test_receiver() {
    const std::string sdpPath {"../../cmake-build-debug/bin/test.sdp"};
    const int width = 640;
    const int height = 480;

    const auto receiver = std::make_unique<FFMpegReceiver>(sdpPath);

    if (!receiver->openStream(sdpPath)) {
        std::cerr << "Failed open stream.\n";
        return;
    }
    while (true) {
        auto frame = receiver->receiveAndDecodeFrame();
        if (!frame) {
            std::cerr << "Failed to receive and decode frame.\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } else {
            cv::Mat frame_mat(height + height / 2, width, CV_8UC1, frame->data());
            cv::Mat bgr_frame;
            cv::cvtColor(frame_mat, bgr_frame, cv::COLOR_YUV2BGR_I420);
            cv::imshow("frame", bgr_frame);
            cv::waitKey(1);
        }
    }
}

int main (int argc, char* argv[]) {
    const std::string rtp_destination {"rtp://127.0.0.1:5000"};

    const auto streamer = std::make_unique<FFMpegStreamer>(rtp_destination, VideoQuality::HIGH);

    if (!streamer->initializeH264()) {
        std::cerr << "Failed initialize stream.\n";
        return -1;
    }

    std::thread sender_thread(test_capture_stream, streamer.get());
    std::thread receiver_thread(test_receiver);

    sender_thread.join();
    receiver_thread.join();

    return 0;
}