#include <iostream>

#include "rtp_server.h"


struct CustomOptions final : public videoOptions {
	URI resource{ "csi://0" };
	DeviceType device_type{ DeviceType::DEVICE_CSI };
	IoType io_type{ IoType::INPUT };
	Codec video_codec{ Codec::CODEC_H264 };
	int width{ 1280 };
	int height{ 720 };
	int frame_rate{ 30 };
	int num_buff{ 4 };
	bool zero_copy{ true };
	FlipMethod flip_method{ FlipMethod::FLIP_HORIZONTAL };
};

struct CustomOptionsDeleter {

};


class JRtpSender::Impl {
public:
	Impl(const std::string& ip_, size_t port_, STREAM_QUALITY quality_);
	~Impl() = default;

	videoSource* input;
	videoSource* output;
	CustomOptions m_options;
	STREAM_QUALITY current_quality;
	size_t port;
	std::string ip_addr;
	std::string codec_format;
	GstFlowReturn bad_flow;

	void set_quality(STREAM_QUALITY quality_); 
	void set_scale(const std::pair<int, int>& _size);
	void set_stable_properties();
	void stop();
};


JRtpSender::Impl::Impl(const std::string& ip_, size_t port_, STREAM_QUALITY quality_) : 
												  current_quality(quality_),
												  ip_addr(ip_),  port(port_)
{
	
}

JRtpSender::JRtpSender(const std::string& ip_, size_t port_, STREAM_QUALITY quality_) : 
									sender(std::make_unique<Impl>(ip_, port_, quality_))
{
	
}

void JRtpSender::Impl::set_stable_properties()
{
	

}


JRtpSender::~JRtpSender()
{

}

void JRtpSender::Impl::set_quality(STREAM_QUALITY quality_)
{
	int quality{0};
	
	if (quality_ == STREAM_QUALITY::LOW) {
		quality = static_cast<int>(STREAM_QUALITY::JPEG_LOW);
	}
	else
		quality = static_cast<int>(STREAM_QUALITY::JPEG_HIGH);

	//g_object_set(G_OBJECT(s_launcher->encoder), "quality", quality, nullptr);
}

void JRtpSender::set_framerate(const int new_fps)
{
	/*
	gst_element_set_state(sender->s_launcher->pipeline, GST_STATE_PAUSED);

	sender->s_launcher->current_fps = new_fps;

	GstCaps* frame_caps = gst_caps_new_simple("video/x-raw",
		"format", G_TYPE_STRING, sender->codec_format.c_str(),
		"framerate", GST_TYPE_FRACTION, new_fps, 1,
		nullptr);
	g_object_set(G_OBJECT(sender->s_launcher->capsfilter), "caps", frame_caps, nullptr);
	gst_caps_unref(frame_caps);

	gst_element_set_state(sender->s_launcher->pipeline, GST_STATE_PLAYING);
	*/
}

void JRtpSender::Impl::set_scale(const std::pair<int, int>& _size)
{
	/*
	gst_element_set_state(s_launcher->pipeline, GST_STATE_PAUSED);

	s_launcher->current_size = _size;

	GstCaps* scale_caps = gst_caps_new_simple("video/x-raw",
		"format", G_TYPE_STRING, codec_format.c_str(),
		"width", G_TYPE_INT, _size.first,
		"height", G_TYPE_INT, _size.second,
		nullptr);

	g_object_set(s_launcher->capsfilter, "caps", scale_caps, nullptr);
	gst_caps_unref(scale_caps);
	
	gst_element_set_state(s_launcher->pipeline, GST_STATE_PLAYING);
	*/
}

void JRtpSender::send(const std::vector<unsigned char>& frame)
{
	GstBuffer* buffer;
	GstMapInfo map;
	GstFlowReturn ret;
	
	buffer = gst_buffer_new_allocate(nullptr, frame.size(), nullptr);
	
	if (!buffer) {
		g_printerr("Failed to allocate GstBuffer. \n");
		return;
	}
	
	if (gst_buffer_map(buffer, &map, GST_MAP_WRITE)) {
		std::copy(frame.begin(), frame.end(), map.data);
		gst_buffer_unmap(buffer, &map);
	}
	else {
		g_printerr("Failed to map GstBuffer. \n");
		gst_buffer_unref(buffer);
		return;
	}
		
	//ret = gst_app_src_push_buffer(GST_APP_SRC(sender->s_launcher->appsrc), buffer);

	if (ret != GST_FLOW_OK) {
		g_printerr("Error pushing buffer to appsrc. \n");
	}
	
}

void JRtpSender::stop()
{
	sender->stop();
}

void JRtpSender::Impl::stop() {
	/*
	if (s_launcher->pipeline) {
		GstStateChangeReturn ret_state = gst_element_set_state(s_launcher->pipeline, GST_STATE_NULL);
		if (ret_state == GST_STATE_CHANGE_FAILURE) {
			g_printerr("Failed to set pipeline to GST_STATE_NULL. \n");
		}
		if (s_launcher->appsrc) {
			g_signal_emit_by_name(s_launcher->appsrc, "end-of-stream", &bad_flow);
			if (bad_flow != GST_FLOW_OK) {
				g_printerr("Error ending of stream. \n");
			}
		}
	}
	*/
}

bool JRtpSender::stable_start()
{
	if (sender->bad_flow == GST_FLOW_OK) {
		return false;
	}

	try {
		sender->set_stable_properties(); 
	}
	catch (const std::exception& ex) {
		g_printerr("Error set_properties. %s \n", ex.what());
		stop();
		return false;
	}

	sender->set_quality(sender->current_quality);

	//gst_element_set_state(sender->s_launcher->pipeline, GST_STATE_PLAYING);

	return true;
}

