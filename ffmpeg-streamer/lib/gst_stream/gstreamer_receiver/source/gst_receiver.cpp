#include "gst_receiver.h"

#include <functional>
#include <mutex>
#include <iostream>



struct CustomReceiverData {
		GstElement* pipeline = nullptr;
		GstElement* source = nullptr;
		GstElement* depay = nullptr;
		GstElement* decoder = nullptr;
		GstElement* convert = nullptr;
        GstElement* capsfilter = nullptr;
		GstElement* sink = nullptr;
};

struct CustomReceiverDataDeleter {
	void operator() (CustomReceiverData* data) {
		if (data->pipeline) gst_object_unref(data->pipeline);
	}
};


class WGstReceiver::Impl {
public:
	explicit Impl(size_t port);
	~Impl() = default;

	using FrameCallback = std::function<void(const WGstReceiver::Frame&)>;
	void connect(const FrameCallback& cb) {
		callbacks.push_back(cb);
	}

	void emit(const Frame& frame_data);

	std::unique_ptr<CustomReceiverData, CustomReceiverDataDeleter> r_launcher;
	size_t port;
	GMainLoop* main_loop = nullptr;
	GstBus* bus = nullptr;
	gboolean is_live;
	std::mutex callback_mutex;
	std::vector<FrameCallback> callbacks;
	WGstReceiver::Frame last_frame;

	[[nodiscard]] bool link_elements() const;
	[[nodiscard]] bool set_source() const;
	void connect();
	[[nodiscard]] bool set_element_state() const;

	[[nodiscard]] GstElement* get_pipeline() const {
		return r_launcher->pipeline;
	}

	GstBus* get_bus();

	static GstFlowReturn on_new_sample(GstElement* sink, Impl* receiver); 
	static gboolean on_bus_message(GstBus* bus, GstMessage* message, WGstReceiver* receiver);
};

WGstReceiver::Impl::Impl(size_t port_) : r_launcher(new CustomReceiverData()), 
													port(port_), is_live(FALSE),
													main_loop(g_main_loop_new(nullptr, FALSE))
{
	gst_init(nullptr, nullptr);

	r_launcher->source = gst_element_factory_make("udpsrc", "source");
	r_launcher->depay = gst_element_factory_make("rtpjpegdepay", "depay");
	r_launcher->decoder = gst_element_factory_make("jpegdec", "decoder");
	r_launcher->convert = gst_element_factory_make("videoconvert", "convert");
    r_launcher->capsfilter = gst_element_factory_make("capsfilter", "caps_filter");
	r_launcher->sink = gst_element_factory_make("appsink", "sink");
	r_launcher->pipeline = gst_pipeline_new("veda-receiver-pipeline");

	if (!r_launcher->source  ||
		!r_launcher->depay   ||
		!r_launcher->decoder ||
		!r_launcher->convert ||
		!r_launcher->sink    ||
		!r_launcher->pipeline)
	{
		g_printerr("Not all elements could be created.\n");
		throw std::runtime_error("Failed creating pipeline.");
	}

	g_object_set(G_OBJECT(r_launcher->sink), "emit-signals", true, "sync", false, nullptr); 

}

WGstReceiver::WGstReceiver(size_t port_) : receiver(std::make_unique<Impl>(port_))
{
}

WGstReceiver::~WGstReceiver() 
{
	if (receiver->r_launcher && receiver->r_launcher->pipeline){
		gst_element_set_state(receiver->r_launcher->pipeline, GST_STATE_NULL);
	}
	if (receiver->bus) g_object_unref(receiver->bus);
	if (receiver->main_loop) g_object_unref(receiver->main_loop);

}
	
bool WGstReceiver::start()
{
    if (!receiver->set_source()) {
        g_printerr("Failed set source. \n");
        return false;
    }

	if (!receiver->link_elements()) {
		g_printerr("Failed link elements. \n");
		return false;
	}

	receiver->connect();

	if (!receiver->set_element_state()) {
		g_printerr("Failed set state for elements. \n");
		return false;
	}
		
	return true;
}

GMainLoop* WGstReceiver::get_main_loop() {
	return receiver->main_loop;
}

void WGstReceiver::clean()
{
	gst_element_set_state(receiver->r_launcher->pipeline, GST_STATE_NULL);
	gst_object_unref(receiver->r_launcher->pipeline);
}

void WGstReceiver::play()
{
	GstStateChangeReturn ret = gst_element_set_state(receiver->r_launcher->pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr("Unable to set the pipeline to the play state.\n");
	}
}

void WGstReceiver::pause()
{
	GstStateChangeReturn ret = gst_element_set_state(receiver->r_launcher->pipeline, GST_STATE_PAUSED);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr("Unable to set the pipeline to the pause state.\n");
	}
}

gboolean WGstReceiver::Impl::on_bus_message(GstBus* bus, GstMessage* message, WGstReceiver* receiver) {

	switch (GST_MESSAGE_TYPE(message)) {
		case GST_MESSAGE_BUFFERING:
		{
			gint percent = 0;
			gst_message_parse_buffering(message, &percent);
			g_print("Buffering (%3d%%)\r", percent);

			if (receiver->receiver->is_live)
				break;

			if (percent < 100) {
				gst_element_set_state(receiver->receiver->get_pipeline(), GST_STATE_PAUSED);
			}
			else {
				gst_element_set_state(receiver->receiver->get_pipeline(), GST_STATE_PLAYING);
			}
		}
		break;

		case GST_MESSAGE_STATE_CHANGED:
		{
			if (GST_MESSAGE_SRC(message) == GST_OBJECT(receiver->receiver->get_pipeline())) {
				GstState old_state, new_state, pending_state;
				gst_message_parse_state_changed(message, &old_state, &new_state, &pending_state);
				if (new_state == GST_STATE_PLAYING) {
					GstQuery* query = gst_query_new_latency();
					if (gst_element_query(receiver->receiver->get_pipeline(), query)) {
						gboolean live;
						gst_query_parse_latency(query, &live, nullptr, nullptr);
						receiver->receiver->is_live = live;
					}
					gst_query_unref(query);
				}
			}
		}
		break;

		default:
			break;
	}

	return TRUE;
}

void WGstReceiver::Impl::emit(const Frame& frame_data) {
	for (const auto& cb : callbacks) {
		cb(frame_data);
	}
}

std::optional<WGstReceiver::Frame> WGstReceiver::read_frame() {
	std::lock_guard<std::mutex> lock(receiver->callback_mutex);

	if (!receiver->last_frame.data.empty()) {
		return receiver->last_frame;
	}
	else {
		return std::nullopt;
	}
}

GstFlowReturn WGstReceiver::Impl::on_new_sample(GstElement* sink, Impl* receiver) {
	gint width = 0;
	gint height = 0;

	GstSample* sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));

	//auto now = std::chrono::system_clock::now();
	//std::time_t now_tt = std::chrono::system_clock::to_time_t(now);
	if (sample) {
		GstBuffer* buffer = gst_sample_get_buffer(sample);
		GstMapInfo map{};

		if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
			GstCaps* caps = gst_sample_get_caps(sample);
			GstStructure* structure = gst_caps_get_structure(caps, 0);

			gst_structure_get_int(structure, "width", &width);
			gst_structure_get_int(structure, "height", &height);
			auto format = gst_structure_get_string(structure, "format");

			std::vector<unsigned char> frame_data(map.data, map.data + map.size);

			//GstClockTime pts;
			//pts = GST_BUFFER_PTS(buffer);

			receiver->emit({ frame_data, width, height/*, pts, now_tt*/});
			
			gst_buffer_unmap(buffer, &map);
		}
		gst_sample_unref(sample);
	}

	return GST_FLOW_OK;
}


bool WGstReceiver::Impl::link_elements() const
{
	gst_bin_add_many(GST_BIN(r_launcher->pipeline),
		r_launcher->source,
		r_launcher->depay,
		r_launcher->decoder,
        r_launcher->capsfilter,
		r_launcher->convert,
		r_launcher->sink,
		nullptr);

	
	if (!gst_element_link_many(
		r_launcher->source,
		r_launcher->depay,
		r_launcher->decoder,
        r_launcher->capsfilter,
		r_launcher->convert,
		r_launcher->sink,
		nullptr))
	{
		g_printerr("Elements could not be linked.\n");
		return false;
	}

	return true;
}

bool WGstReceiver::Impl::set_source() const
{
	g_object_set(r_launcher->source, "port", port, nullptr);

	GstCaps* caps = gst_caps_new_simple("application/x-rtp",
		"media", G_TYPE_STRING, "video",
		"encoding-name", G_TYPE_STRING, "JPEG",
		"clock-rate", G_TYPE_INT, 90000,
		nullptr);
    GstCaps* filter = gst_caps_new_simple("video/x-raw",
                                          "format", G_TYPE_STRING, "I420",
                                          nullptr);
	if (caps && filter){
        g_object_set(r_launcher->source, "caps", caps, nullptr);
        g_object_set(r_launcher->capsfilter, "caps", filter, nullptr);
    } else
        return false;

	gst_caps_unref(caps);
    gst_caps_unref(filter);
	
	return true;
}

void WGstReceiver::Impl::connect() 
{
	auto bus_ = get_bus();
	gst_bus_add_signal_watch(bus_);

	//g_signal_connect(bus, "message", G_CALLBACK(on_bus_message), this);
	g_signal_connect(r_launcher->sink, "new-sample", G_CALLBACK(on_new_sample), this); 

	connect([&](const Frame& frame_data){
		Frame f;
		f.data = frame_data.data;
		f.width = frame_data.width;
		f.height = frame_data.height;
		//f.timestamp = frame_data.timestamp;
		//f.curr_time = frame_data.curr_time;
		last_frame = f;
	});
}

bool WGstReceiver::Impl::set_element_state() const
{
	GstStateChangeReturn ret = gst_element_set_state(r_launcher->pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr("Unable to set the pipeline to the playing state.\n");
		gst_object_unref(r_launcher->pipeline);
		return false;
	}

	return true;
}

GstBus* WGstReceiver::Impl::get_bus()  
{
	if (!bus) 
		bus = gst_element_get_bus(r_launcher->pipeline);

	return bus;
}







