#pragma once

#include <gst/gst.h>
#include <opencv2/opencv.hpp>
#include <string>

struct CoordMetaData;

class RtpSender {

public:
	explicit RtpSender(std::string  ip_addr_, unsigned short port_);
	~RtpSender();

	void send(const std::vector<unsigned char>& frame, const std::vector<unsigned char>& coord) const;
	[[nodiscard]] bool play();
	[[nodiscard]] bool pause();
	[[nodiscard]] bool stable_start();
	void cleanup();

private:
    static constexpr int frame_width = 640;
    static constexpr int frame_height = 480;
    static constexpr int frame_framerate = 20;
	const std::string plugin_path{
			"/home/maxim/my-projects/project-gsn-nsu/video_sender_prototype//libs/elements/cmake-build-debug/libplugin_payloader.so"};

	std::string ip_addr;
	unsigned short port;
	GstPipeline* pipeline = nullptr;
	GstElement* source;
	GstElement* udp_sink;
    //GstElement* rtpvrawpay;
	GstElement* rtpklvpay;
	GstElement* customrtppay;
	GstBus* bus = nullptr;
	GstMessage* msg = nullptr;

	bool link_elements();
	[[nodiscard]] bool set_state(GstState state_) const;
};
