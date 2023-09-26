#include <iostream>

#include "gst_sender.h"


struct CustomSenderData {
		GstElement* pipeline = nullptr;
		GstElement* appsrc = nullptr;
		GstElement* depay = nullptr;
		GstElement* convert = nullptr;
		GstElement* encoder = nullptr;
		GstElement* videoscale = nullptr;
		GstElement* videorate = nullptr;
		GstElement* capsfilter = nullptr;
		GstElement* sink = nullptr;

		std::pair<int, int> current_size{0, 0};
		int current_fps{ 0 };
};

struct CustomSenderDataDeleter {
	void operator()(CustomSenderData* data) {
		if (data->pipeline) gst_object_unref(data->pipeline);
	}
};

class WGstSender::Impl {
public:
	Impl(const std::string& ip_, size_t port_, STREAM_QUALITY quality_);
	~Impl() = default;

	std::unique_ptr<CustomSenderData, CustomSenderDataDeleter> s_launcher;
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

inline constexpr int base_camera_width = 640;
inline constexpr int base_camera_height = 480;
inline constexpr int auxiliary_camera_width = 320;
inline constexpr int auxiliary_camera_height = 240;
inline constexpr int high_fps = 20;
inline constexpr int low_fps = 2;

WGstSender::Impl::Impl(const std::string& ip_, size_t port_, STREAM_QUALITY quality_) : 
												  s_launcher(new CustomSenderData()), 
												  current_quality(quality_),
												  ip_addr(ip_),  port(port_),
												  bad_flow(GST_FLOW_EOS),
												  codec_format{"I420"}
{
	gst_init(nullptr, nullptr);

	s_launcher->appsrc = gst_element_factory_make("appsrc", "source");
	s_launcher->depay = gst_element_factory_make("rtpjpegpay", "pay");
	s_launcher->convert = gst_element_factory_make("videoconvert", "convert");
	s_launcher->encoder = gst_element_factory_make("jpegenc", "encoder");
	s_launcher->videoscale = gst_element_factory_make("videoscale", "scale");
	s_launcher->videorate = gst_element_factory_make("videorate", "rate");
	s_launcher->capsfilter = gst_element_factory_make("capsfilter", "customfilter");
	s_launcher->sink = gst_element_factory_make("udpsink", "sink");

	s_launcher->pipeline = gst_pipeline_new("veda-sender-pipeline");

	try {
		if (!s_launcher->pipeline ||
			!s_launcher->appsrc ||
			!s_launcher->depay ||
			!s_launcher->convert ||
			!s_launcher->encoder ||
			!s_launcher->videoscale ||
			!s_launcher->videorate ||
			!s_launcher->capsfilter ||
			!s_launcher->sink 
			)
		{
			g_printerr("Not all elements could be created.\n");
			throw std::runtime_error("Failed creating pipeline.");  
		}
	}
	catch (std::exception& ex) {
		std::cerr << ex.what();
		stop();
	}
}

WGstSender::WGstSender(const std::string& ip_, size_t port_, STREAM_QUALITY quality_) : 
									sender(std::make_unique<Impl>(ip_, port_, quality_))
{
	
}

void WGstSender::Impl::set_stable_properties()
{
	if (current_quality == STREAM_QUALITY::HIGH) {
		s_launcher->current_size = { base_camera_width, base_camera_height };
		s_launcher->current_fps = high_fps;
	}
	else {
		s_launcher->current_size = { auxiliary_camera_width, auxiliary_camera_height };
		s_launcher->current_fps = low_fps;
	}

	gst_bin_add_many(GST_BIN(s_launcher->pipeline), 
								s_launcher->appsrc,
								s_launcher->convert,
								s_launcher->videorate,
								s_launcher->videoscale,
								s_launcher->capsfilter,
								s_launcher->encoder,
								s_launcher->depay,
								s_launcher->sink, 
								nullptr);

	if (!gst_element_link_many(s_launcher->appsrc,
							s_launcher->convert,
							s_launcher->videorate,
							s_launcher->videoscale,
							s_launcher->capsfilter,
							s_launcher->encoder,
							s_launcher->depay,
							s_launcher->sink, nullptr)) 
	{
		throw std::runtime_error("Failed link elements");
	}


	g_object_set(G_OBJECT(s_launcher->appsrc), "format", GST_FORMAT_TIME, nullptr);
	g_object_set(G_OBJECT(s_launcher->appsrc), "is-live", TRUE, nullptr);
	g_object_set(G_OBJECT(s_launcher->appsrc), "do-timestamp", TRUE, nullptr);

	g_object_set(s_launcher->sink, "host", ip_addr.c_str() , "port", port, nullptr); 
	if (!G_IS_OBJECT(s_launcher->sink)) {
		g_printerr("Failed set udpsink. \n");
	}

	GstCaps* initial_caps = gst_caps_new_simple("video/x-raw",  
		"format", G_TYPE_STRING, codec_format.c_str(),
		"width", G_TYPE_INT, s_launcher->current_size.first,
		"height", G_TYPE_INT, s_launcher->current_size.second,
		"framerate", GST_TYPE_FRACTION, s_launcher->current_fps, 1,
		 nullptr);

	if (!initial_caps) {
		g_printerr("Failed to create initial caps.\n");
		throw std::runtime_error("Error creating initial caps.");
	}

	gst_app_src_set_caps(GST_APP_SRC(s_launcher->appsrc), initial_caps);
	gst_caps_unref(initial_caps); 
}


WGstSender::~WGstSender()
{
	if (sender->s_launcher && sender->s_launcher->pipeline) {
		gst_element_set_state(sender->s_launcher->pipeline, GST_STATE_NULL);
	}
	std::cout << __func__ << std::endl;
}

void WGstSender::Impl::set_quality(STREAM_QUALITY quality_)
{
	int quality{0};
	
	if (quality_ == STREAM_QUALITY::LOW) {
		quality = static_cast<int>(STREAM_QUALITY::JPEG_LOW);
	}
	else
		quality = static_cast<int>(STREAM_QUALITY::JPEG_HIGH);

	g_object_set(G_OBJECT(s_launcher->encoder), "quality", quality, nullptr);
}

void WGstSender::set_framerate(const int new_fps)
{
	gst_element_set_state(sender->s_launcher->pipeline, GST_STATE_PAUSED);

	sender->s_launcher->current_fps = new_fps;

	GstCaps* frame_caps = gst_caps_new_simple("video/x-raw",
		"format", G_TYPE_STRING, sender->codec_format.c_str(),
		"framerate", GST_TYPE_FRACTION, new_fps, 1,
		nullptr);
	g_object_set(G_OBJECT(sender->s_launcher->capsfilter), "caps", frame_caps, nullptr);
	gst_caps_unref(frame_caps);

	gst_element_set_state(sender->s_launcher->pipeline, GST_STATE_PLAYING);
}

void WGstSender::Impl::set_scale(const std::pair<int, int>& _size)
{
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
}

void WGstSender::send(const std::vector<unsigned char>& frame)
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
		
	ret = gst_app_src_push_buffer(GST_APP_SRC(sender->s_launcher->appsrc), buffer);

	if (ret != GST_FLOW_OK) {
		g_printerr("Error pushing buffer to appsrc. \n");
	}
	
}

void WGstSender::stop()
{
	sender->stop();
}

void WGstSender::Impl::stop() {
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
}

bool WGstSender::stable_start()
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

	gst_element_set_state(sender->s_launcher->pipeline, GST_STATE_PLAYING);

	return true;
}

