#include <optional>

#include "rtp_receiver.h"
#include <gst/app/gstappsink.h>
#include <gst/rtp/gstrtpbuffer.h>
#include "meta_api.h"


RtpReceiver::RtpReceiver(unsigned short port_) : port{port_}

{
	gst_init(nullptr, nullptr);

	pipeline = gst_pipeline_new("rtp-veda-receiver");
	udpsrc = gst_element_factory_make("udpsrc", "udp_source");
    rtpvrawdepay = gst_element_factory_make("rtpvrawdepay", "rtpvraw_depay");
    rtpklvdepay = gst_element_factory_make("rtpklvdepay", "rtp_klv_depay");
    capsfilter = gst_element_factory_make("capsfilter", "video_filter");
    appsink = gst_element_factory_make("appsink", "video_sink");

}

RtpReceiver::~RtpReceiver()
{
    std::cout << __func__ << std::endl;
}

bool RtpReceiver::play()
{
	if (!set_state(GST_STATE_PLAYING)) {
		return false;
	}

    return true;
}

std::optional<Frame> RtpReceiver::read_frame()
{
    std::lock_guard<std::mutex> lock (callback_mutex);
    if (!last_frame.data.empty()) {
        return last_frame;
    }
}

bool RtpReceiver::pause()
{
	return false;
}

bool RtpReceiver::stable_start()
{
    g_object_set(udpsrc, "port", port, nullptr);
    g_object_set(G_OBJECT(appsink), "emit-signals", true, "sync", false, nullptr);
    //g_object_set(G_OBJECT(appsink), "buffer-list", true, nullptr);
    //g_object_set(G_OBJECT(queue), "max-size-buffers", 1, "max-size-bytes", 0, "max-size-time", 0, nullptr);

	connect();

	if (!link_elements()) {
		g_printerr("Failed elements link.\n");
		return false;
	}

	return true;
}

void RtpReceiver::cleanup() const
{
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	g_main_loop_unref(loop);
}

bool RtpReceiver::link_elements() const
{
    /*GstCaps* string_caps = gst_caps_new_simple("application/x-rtp",
                                        "media", G_TYPE_STRING, "video",
                                        "clock-rate", G_TYPE_INT, 90000,
                                        "encoding-name", G_TYPE_STRING, "RAW",
                                        "sampling", G_TYPE_STRING, "YCbCr-4:2:0",
                                        "width", G_TYPE_STRING, "640",
                                        "height", G_TYPE_STRING, "480",
                                        "colorimetry", G_TYPE_STRING, "BT601-5",
                                        nullptr);*/

    GstCaps* sink_caps = gst_caps_new_simple("application/x-rtp",
                                             "media", G_TYPE_STRING, "application",
                                             "clock-rate", G_TYPE_INT, 90000,
                                             "encoding-name", G_TYPE_STRING, "SMPTE336M",
                                             nullptr);

    if (sink_caps)
        g_object_set(udpsrc, "caps", sink_caps, nullptr);
    else
        return false;
    gst_caps_unref(sink_caps);

    /*GstCaps* caps = gst_caps_new_simple("application/x-rtp",
		"media", G_TYPE_STRING, "video",
		"encoding-name", G_TYPE_STRING, "JPEG",
		"clock-rate", G_TYPE_INT, 90000,
		nullptr);
    GstCaps* filter = gst_caps_new_simple("video/x-raw",
                                          "format", G_TYPE_STRING, "I420",
                                          nullptr);
	if (caps && filter){
        g_object_set(udpsrc, "caps", caps, nullptr);
        g_object_set(capsfilter, "caps", filter, nullptr);
    } else
        return false;

	gst_caps_unref(caps);
    gst_caps_unref(filter);*/

    gst_bin_add_many(GST_BIN(pipeline), udpsrc, rtpklvdepay, appsink, nullptr);
    if (bool res = gst_element_link_many(udpsrc, rtpklvdepay, appsink, nullptr); !res) {
        g_printerr("Failed link video elements.\n");
        return res;
    }

	return true;
}

bool RtpReceiver::set_state(GstState state_)
{
	GstStateChangeReturn ret = gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr("Failed to set state for pipeline.\n");
		return false;
	}
	return true;
}

void RtpReceiver::connect()
{
	const auto bus = gst_element_get_bus(pipeline);
	gst_bus_add_signal_watch(bus);

    g_signal_connect(appsink, "new-sample", G_CALLBACK(on_new_sample), this);

    /*
    GstPad* sink_pad = gst_element_get_static_pad(appsink, "sink");
    if (!sink_pad) {
        g_printerr("Failed to get sink pad from appsink.\n");
    }
    gst_pad_add_probe(sink_pad, GST_PAD_PROBE_TYPE_BUFFER, buffer_probe_cb, nullptr, nullptr);
    gst_object_unref(sink_pad);
    */

    //g_signal_connect(queue, "pushing", G_CALLBACK(on_new_queue_item), this);
    //gst_app_sink_set_buffer_list_support(GST_APP_SINK(appsink), true);

    c_connect([&](const Frame& frame_data) {
        Frame f;
        f.data = frame_data.data;
        f.width = frame_data.width;
        f.height = frame_data.height;
        f.xCoord = frame_data.xCoord;
        f.yCoord = frame_data.yCoord;
        last_frame = f;
    });
}

GstFlowReturn RtpReceiver::on_new_sample(GstElement* appsink, gpointer udata) {
    const auto receiver = static_cast<RtpReceiver *>(udata);
    gint width = 0;
    gint height = 0;
    std::vector<unsigned char> frame_data;

    GstSample* sample = gst_app_sink_pull_sample(GST_APP_SINK(appsink));

    if (sample) {
        GstBuffer* buffer = gst_sample_get_buffer(sample);
        GstMapInfo map{};

        auto size_buff = gst_buffer_get_size(buffer);

        if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
            GstCaps* caps = gst_sample_get_caps(sample);
            GstStructure* structure = gst_caps_get_structure(caps, 0);

            gst_structure_get_int(structure, "width", &width);
            gst_structure_get_int(structure, "height", &height);
            auto format = gst_structure_get_string(structure, "format");
            //g_print("Format: %s", format);
            frame_data.assign(map.data, map.data + map.size);

            receiver->emit({frame_data, width, height, 1.0, 2.0});
        }
    }
    gst_sample_unref(sample);

    return GST_FLOW_OK;
}

void RtpReceiver::on_new_queue_item(GstElement* queue, gpointer udata) {
}

void RtpReceiver::emit(const Frame &frame_data) const {
    for (const auto& cb : callbacks) {
        cb(frame_data);
    }
}

GstPadProbeReturn RtpReceiver::buffer_probe_cb(GstPad* pad, GstPadProbeInfo* info, gpointer user_data) {
    if (info->type & GST_PAD_PROBE_TYPE_BUFFER) {
        GstBuffer* buffer = GST_PAD_PROBE_INFO_BUFFER(info);

        auto meta_type = meta::SMetaType::getInstance().get_type();

        const auto meta_data = reinterpret_cast<CoordMetaData *>(gst_buffer_get_meta(buffer, meta_type));
        GstBuffer* copy = gst_buffer_copy(buffer);

        g_print("Meta type is: %f", meta_type);
        if (meta_data) {
            g_print("DTS: %d, PTS: %d", meta_data->dts, meta_data->pts);
        }
        else {
            g_printerr("Failed retrieve meta from buffer.\n");
        }

        CoordMetaData* coord_meta = reinterpret_cast<CoordMetaData *>(gst_buffer_get_meta(
            copy, meta::SMetaType::getInstance().get_type()));
        if (coord_meta) {
            g_print("Yes meta.\n");
        }
        else {
            g_print("Not meta.\n");
        }

    }

    return GST_PAD_PROBE_OK;
}
















