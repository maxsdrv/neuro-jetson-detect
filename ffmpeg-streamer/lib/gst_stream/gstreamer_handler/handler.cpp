#include <iostream>
#include <thread>

#include <gst_sender.h>
#include <gst_receiver.h>
//#include <gstrtploaderplugin.h>
#include "rtp_sender.h"
#include "rtp_receiver.h"
#include <opencv2/opencv.hpp>

void test_rtp_stream(RtpSender* sender) {
	cv::VideoCapture cap(0);
	if (!cap.isOpened()) {
		g_printerr("Error opening video capture.\n");
		return;
	}

	double x = 123.456;
    double y = 789.012;
	std::vector<unsigned char> extension;
    extension.resize(sizeof(double) * 2);
    std::memcpy(extension.data(), &x, sizeof(double));
    std::memcpy(extension.data() + sizeof(double), &y, sizeof(double));

	while (true) {
		cv::Mat frame;
		cap >> frame;

		if (!frame.empty()) {
			cv::Mat yuy_frame;
            cv::resize(frame, frame, cv::Size(640, 480));
			cv::cvtColor(frame, yuy_frame, cv::COLOR_BGR2YUV_I420);
			//cv::cvtColor(frame, yuy_frame, cv::COLOR_BGR2GRAY, 1);
			std::vector<unsigned char> data_frame(yuy_frame.data, yuy_frame.data + yuy_frame.total() * yuy_frame.elemSize());
			sender->send(data_frame, extension);
		} else
			break;
	}
}

[[noreturn]] void test_rtp_receiver(RtpReceiver* receiver) {
	while (true) {
		auto frame_opt = receiver->read_frame();
		//g_print("Coordinates: %f%f\n", frame_opt->xCoord, frame_opt->yCoord);
		if (frame_opt) {
			cv::Mat frame(frame_opt->height + frame_opt->height / 2, frame_opt->width,
				CV_8UC1, frame_opt->data.data());
			cv::Mat bgr_frame;
			cv::cvtColor(frame, bgr_frame, cv::COLOR_YUV2BGR_I420);

			cv::imshow("receiver", bgr_frame);
			cv::waitKey(1);
		} else {
			g_printerr("Waiting for ... \n");
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
}

void capture_and_stream(WGstSender* sender, WGstSender::STREAM_QUALITY _quality) {
	cv::VideoCapture cap(0);
	if (!cap.isOpened()) {
		g_printerr("Error opening video capture.\n");
		return;
	}
	//GstClock* clock = gst_system_clock_obtain();
	//GstClockTime timestamp = gst_clock_get_time(clock);
	
	while (true) {
		cv::Mat frame;
		cv::Mat yuy_frame;
		cap >> frame;
		
		if (_quality == WGstSender::STREAM_QUALITY::LOW) 
		{
			cv::resize(frame, frame, cv::Size(320, 240));
		}

		if (!frame.empty()) {
            cv::resize(frame, frame, cv::Size(640, 480));
			cv::cvtColor(frame, yuy_frame, cv::COLOR_BGR2YUV_I420);
			std::vector<unsigned char> data_frame(yuy_frame.data, yuy_frame.data + yuy_frame.total() * yuy_frame.elemSize());
			sender->send(data_frame);
		}
		else {
			//sender->pause();
			//sender->clean();
			break;
		}
	}
	//g_object_unref(clock);
}

[[noreturn]] void run_receiver(WGstReceiver* receiver) {
	while (true) {
		auto frame_opt = receiver->read_frame();

		if (frame_opt) {
			cv::Mat frame(frame_opt->height + frame_opt->height / 2, frame_opt->width,
				CV_8UC1, frame_opt->data.data());
            //g_print("Size of buffer: %d", frame_opt->data.size());
            //g_print("Width: %d Height: %d \n", frame_opt->width, frame_opt->height);
			cv::Mat bgr_frame;
			cv::cvtColor(frame, bgr_frame, cv::COLOR_YUV2BGR_I420);

			/*g_print("PTS: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(frame_opt->timestamp));
			std::cout << "Current time: "
				<< std::ctime(&frame_opt->curr_time) << std::endl;*/

			cv::imshow("receiver", bgr_frame);
			cv::waitKey(1);
		}
		else {
			g_printerr("Waiting for ... \n");
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
}




int main(int argc, char** argv) {
	g_setenv("GST_DEBUG", "*:5", TRUE);

	std::string ip {"10.10.3.206"};
	unsigned short port{ 5000 };

	const auto rtp_sender = std::make_unique<RtpSender>(ip, port);
	const auto rtp_receiver = std::make_unique<RtpReceiver>(port);

	if (!rtp_sender->stable_start()) {
		std::cerr << "Error start rtp sender.\n";
		return -1;
	}
	if (!rtp_receiver->stable_start()) {
		std::cerr << "Error start rtp receiver.\n";
		return -1;
	}

	if (!rtp_sender->play()) {
		g_printerr("Failed play sender.\n");
		return -1; 
	}

	if (!rtp_receiver->play()) {
		g_printerr("Failed play receiver.\n");
		return -1;
	}

	std::thread capture_thread(test_rtp_stream, rtp_sender.get());
	std::thread receiver_thread(test_rtp_receiver, rtp_receiver.get());

	receiver_thread.join();
	capture_thread.join();

	return 0;
}


	/*/*
	auto quality = WGstSender::STREAM_QUALITY::HIGH;
	auto uSender = std::make_unique<WGstSender>(ip, port, quality);
	auto uReceiver = std::make_unique<WGstReceiver>(port);
					
	if (!uSender->stable_start()) {
		std::cerr << "Error start sender.\n";
		return -1;
	}
	
	if (!uReceiver->start()) {
		std::cerr << "Error start receiver.\n";
		return -1;
	}

	std::thread capture_thread(capture_and_stream, uSender.get(), quality);
	std::thread receiver_thread(run_receiver, uReceiver.get());
	#2#

	// for change quality
	/*
	while (true) {
		std::cout << "Enter command (change framerate: F, change scale: S, exit: Q/EXIT): ";
		std::string input;
		int user_frame{};
		std::pair<int, int> scale_size{};
		std::cin >> input;

		if (input == "F") {
			g_print("Input framerate: .\n");
			std::cin >> user_frame;
			uSender->set_framerate(user_frame);
			
		}
		else if (input == "L") {
			g_print("Change video quality. \n");
			uSender->set_video_quality(WGstSender::STREAM_QUALITY::LOW);
		}
		else if (input == "S") {
			g_print("Change scale. \n");
			uSender->set_video_scale({ 1024, 800 });
		}

		else if (input == "E") {
			g_print("Stop reader. \n");
			uReceiver->pause();
		}
		
		else if (input == "Q" || input == "EXIT") {
			g_print("QUIT.\n");
			break;
		}
		else {
			g_printerr("Error command.\n");
			continue;
		}
	}#2#
	
	//capture_thread.join();
	//receiver_thread.join();

	return 0;
}*/


