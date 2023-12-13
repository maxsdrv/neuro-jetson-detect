#pragma once

#include <gst/gst.h>
#include <gst/app/gstappsink.h>

#include <vector>
#include <optional>
#include <memory>
#include <chrono>

class WGstReceiver {

public:
	explicit WGstReceiver(size_t port_);
	~WGstReceiver();
	
	struct Frame {
		std::vector<unsigned char> data{};
		int width = 0;
		int height = 0;
		//GstClockTime timestamp = 0;
		//std::time_t curr_time;
	};

	[[nodiscard]] bool start();
	std::optional<Frame> read_frame();

	GMainLoop* get_main_loop(); 
	void clean();
	void play();
	void pause();

private:
	class Impl;
	std::unique_ptr<Impl> receiver;
	
};



