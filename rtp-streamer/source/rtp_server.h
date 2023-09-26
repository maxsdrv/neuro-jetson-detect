#pragma once 

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <jetson-utils/videoSource.h>
#include <jetson-utils/videoOutput.h>

#include <vector>
#include <string>
#include <memory>

class JRtpSender {
public:
	enum class STREAM_QUALITY {
		LOW = 1,
		HIGH,
		JPEG_LOW = 30,
		JPEG_HIGH = 70
	};

	JRtpSender(const std::string& ip_, size_t port_, STREAM_QUALITY quality_);
	~JRtpSender();

	void send(const std::vector<unsigned char>& frame);
	[[nodiscard]] bool stable_start(); 
	void set_framerate(const int new_fps); 
	void stop();

private:
	class Impl;
	std::unique_ptr<Impl> sender;
};



