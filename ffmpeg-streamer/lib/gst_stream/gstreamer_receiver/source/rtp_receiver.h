#pragma once

#include <gst/gst.h>
#include <opencv2/opencv.hpp>

struct Frame {
    std::vector<unsigned char> data{};
    int width = 0;
    int height = 0;
    double xCoord;
    double yCoord;
};

class RtpReceiver {
public:
	explicit RtpReceiver(unsigned short port_);
	~RtpReceiver();

	bool play();
	std::optional<Frame> read_frame();
	[[nodiscard]] bool pause();
	[[nodiscard]] bool stable_start();
	void cleanup() const;

    using FrameCallBack = std::function<void(const Frame&)>;
    void c_connect(const FrameCallBack& cb) {
        callbacks.push_back(cb);
    }

private:
	unsigned short port;
	GstElement* pipeline;
	GstElement* udpsrc;
	GstElement* appsink;
    GstElement* rtpvrawdepay;
	GstElement* rtpklvdepay;
    GstElement* capsfilter;
	GMainLoop* loop = nullptr;

    std::vector<FrameCallBack> callbacks;
    Frame last_frame {};
	std::mutex callback_mutex;

	[[nodiscard]] bool link_elements() const;
	bool set_state(GstState state_);
	void connect();
    static GstFlowReturn on_new_sample(GstElement* appsink, gpointer udata);
	static void on_new_queue_item(GstElement* queue, gpointer udata);
    void emit(const Frame& frame_data) const;
	static GstPadProbeReturn buffer_probe_cb(GstPad* pad, GstPadProbeInfo* info, gpointer user_data);
};
