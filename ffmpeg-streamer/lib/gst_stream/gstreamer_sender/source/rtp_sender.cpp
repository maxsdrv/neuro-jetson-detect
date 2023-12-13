#include <gst/app/gstappsrc.h>
#include <gst/rtp/gstrtpbuffer.h>

#include <utility>

#include "rtp_sender.h"
#include "meta_api.h"

static constexpr guint8 klv_metadata[] = {
	/*Sample KLV metadata*/
	0x06, 0x0E, 0x2B, 0x34, 0x02, 0x0B, 0x01, 0x01, 0x0E, 0x01, 0x03, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x81, 0xAE, 0x02, 0x08, 0x00, 0x04, 0x60, 0x50,
	0x58, 0x4E, 0x01, 0x80, 0x03, 0x0A, 0x4D, 0x69, 0x73, 0x73, 0x69, 0x6F,
	0x6E, 0x20, 0x31, 0x32, 0x05, 0x02, 0x71, 0xC2, 0x06, 0x02, 0xFD, 0x3D,
	0x07, 0x02, 0x08, 0xB8, 0x0A, 0x08, 0x50, 0x72, 0x65, 0x64, 0x61, 0x74,
	0x6F, 0x72, 0x0B, 0x07, 0x45, 0x4F, 0x20, 0x4E, 0x6F, 0x73, 0x65, 0x0C,
	0x0E, 0x47, 0x65, 0x6F, 0x64, 0x65, 0x74, 0x69, 0x63, 0x20, 0x57, 0x47,
	0x53, 0x38, 0x34, 0x0D, 0x04, 0x55, 0x95, 0xB6, 0x6D, 0x0E, 0x04, 0x5B,
	0x53, 0x60, 0xC4, 0x0F, 0x02, 0xC2, 0x21, 0x10, 0x02, 0xCD, 0x9C, 0x11,
	0x02, 0xD9, 0x17, 0x12, 0x04, 0x72, 0x4A, 0x0A, 0x20, 0x13, 0x04, 0x87,
	0xF8, 0x4B, 0x86, 0x14, 0x04, 0x00, 0x00, 0x00, 0x00, 0x15, 0x04, 0x03,
	0x83, 0x09, 0x26, 0x16, 0x02, 0x12, 0x81, 0x17, 0x04, 0xF1, 0x01, 0xA2,
	0x29, 0x18, 0x04, 0x14, 0xBC, 0x08, 0x2B, 0x19, 0x02, 0x34, 0xF3, 0x30,
	0x1C, 0x01, 0x01, 0x01, 0x02, 0x01, 0x07, 0x03, 0x05, 0x2F, 0x2F, 0x55,
	0x53, 0x41, 0x0C, 0x01, 0x07, 0x0D, 0x06, 0x00, 0x55, 0x00, 0x53, 0x00,
	0x41, 0x16, 0x02, 0x04, 0x01, 0x41, 0x01, 0x02, 0x01, 0x02, 0x29, 0x72,
};


RtpSender::RtpSender(std::string  ip_addr_, unsigned short port_) : ip_addr{std::move(ip_addr_)}, port{port_}
{
	gst_init(nullptr, nullptr);

	/*GError* error = nullptr;
	if (!gst_plugin_load_file(plugin_path.c_str(), &error)) {
		std::cerr << "Error loading plugin: " << (error ? error->message : "Unknown error") << std::endl;
		if (error) g_error_free(error);
	}

	g_print("Plugin loaded. Creating pipeline. \n");*/

	source = gst_element_factory_make("appsrc", "source");
    //rtpvrawpay = gst_element_factory_make("rtpvrawpay", "rtpvraw_pay");
	rtpklvpay = gst_element_factory_make("rtpklvpay", "rtp_klv_pay");
	//customrtppay = gst_element_factory_make("plugin_payloader", "custom_rtp_pay");
	udp_sink = gst_element_factory_make("udpsink", "udp_sink");

    g_object_set(source,"format", GST_FORMAT_TIME, "do-timestamp", TRUE, "is-live", true, nullptr);
    g_object_set(udp_sink, "host", ip_addr.c_str() , "port", port, nullptr);
}

RtpSender::~RtpSender()
{
    std::cout << __func__ << std::endl;
}

void RtpSender::send(const std::vector<unsigned char>& frame, const std::vector<unsigned char>& coord) const
{
	/*GstMapInfo map;
	GstBuffer* video_buffer = gst_buffer_new_allocate(nullptr, frame.size(), nullptr);

	if (!video_buffer) {
		g_printerr("Failed to allocate GstBuffer. \n");
		return;
	}

	if (gst_buffer_map(video_buffer, &map, GST_MAP_WRITE)) {
		std::ranges::copy(frame.begin(), frame.end(), map.data);
		gst_buffer_unmap(video_buffer, &map);
	} else {
		g_printerr("Failed to map GstBuffer. \n");
		gst_buffer_unref(video_buffer);
		return;
	}

	GstFlowReturn ret = gst_app_src_push_buffer(GST_APP_SRC(source), video_buffer);

	if (ret != GST_FLOW_OK) {
		g_printerr("Error pushing buffer to appsrc. \n");
	}*/

	guint metalen = sizeof(klv_metadata);
	std::vector<guint8> metadata_vec(metalen);

	GstBuffer* klv_buffer = gst_buffer_new_allocate(nullptr, metalen, nullptr);
	GstMapInfo map;
	if (!gst_buffer_map(klv_buffer, &map, GST_MAP_WRITE)) {
		g_printerr("Failed to map GstBuffer for KLV data.\n");
		gst_buffer_unref(klv_buffer);
		return;
	}

	std::memcpy(map.data, klv_metadata, metalen);
	gst_buffer_unmap(klv_buffer, &map);

	GstFlowReturn ret = gst_app_src_push_buffer(GST_APP_SRC(source), klv_buffer);

	if (ret != GST_FLOW_OK) {
		g_printerr("Error pushing buffer to appsrc. \n");
	}
}

bool RtpSender::play()
{
	if (!set_state(GST_STATE_PLAYING)) {
		g_printerr("Failed set state play.\n");
		return false;
	}
	//bus = gst_element_get_bus(GST_ELEMENT(pipeline));
	//msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
	return true;
}

bool RtpSender::pause()
{

	return false;
}

void RtpSender::cleanup()
{
	gst_element_send_event(GST_ELEMENT(pipeline), gst_event_new_eos());
	gst_object_unref(bus);
	gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
	gst_object_unref(pipeline);
}

bool RtpSender::link_elements() {
    pipeline = GST_PIPELINE(gst_pipeline_new("rtp_veda_pipeline"));

    gst_bin_add_many(GST_BIN(pipeline), source, rtpklvpay, udp_sink, nullptr);
    if (!gst_element_link_many(source, rtpklvpay, udp_sink, nullptr)) {
        g_printerr("Video elements link failed.\n");
        gst_object_unref(pipeline);
        return false;
    }

    return true;
}
bool RtpSender::set_state(GstState state_) const
{
	GstStateChangeReturn ret = gst_element_set_state(GST_ELEMENT(pipeline), state_);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr("Failed to set state for pipeline.\n");
		return false;
	}
	return true;
}

bool RtpSender::stable_start()
{
	if (!G_IS_OBJECT(udp_sink)) {
		g_printerr("Failed set udpsink. \n");
		return false;
	}

	g_object_set(udp_sink, "sync", false, nullptr);
	//g_object_set(rtppay, "config-interval", 60, nullptr);
	//g_object_set(G_OBJECT(rtpvrawpay), "chunks-per-frame", 10, nullptr);

	/*GstCaps* video_caps = gst_caps_new_simple("video/x-raw",
        "format", G_TYPE_STRING, "I420",
        "width", G_TYPE_INT, 640,
        "height", G_TYPE_INT, 480,
        "framerate", GST_TYPE_FRACTION, 20, 1,
		nullptr);
	if (!video_caps) {
		g_printerr("Failed to create video caps.\n");
		return false;
	}*/
	GstCaps* video_caps = gst_caps_new_simple("meta/x-klv",
		"parsed", G_TYPE_BOOLEAN, TRUE,
		nullptr);

	gst_app_src_set_caps(GST_APP_SRC(source), video_caps);
	gst_caps_unref(video_caps);

	/*GstCaps* initial_caps = gst_caps_new_simple("video/x-raw",
		"format", G_TYPE_STRING, "I420",
		"width", G_TYPE_INT, 640,
		"height", G_TYPE_INT, 480,
		"framerate", GST_TYPE_FRACTION, 20, 1,
		 nullptr);

	if (!initial_caps) {
		g_printerr("Failed to create initial caps.\n");
	}*/

    if (!link_elements()) {
        g_printerr("Failed when was linking.\n");
        return false;
    }

	return true;
}








