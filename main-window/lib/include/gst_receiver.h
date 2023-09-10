#include <gst/gst.h>
#include <gst/app/gstappsink.h>

#include <vector>
#include <optional>
#include <memory>
#include <chrono>
#include <format>


class WGstReceiver {
	using Time = std::chrono::zoned_time<std::chrono::system_clock::duration>;
	struct Frame {
		std::vector<unsigned char> data{};
		int width = 0;
		int height = 0;
		GstClockTime timestamp = 0;
		Time curr_time;
	};

public:
	explicit WGstReceiver(size_t port_);
	~WGstReceiver();
	
	[[nodiscard]] bool start();
	std::optional<Frame> read_frame();

	GMainLoop* get_main_loop(); 

private:
	class Impl;
	std::unique_ptr<Impl> receiver;
	
};



