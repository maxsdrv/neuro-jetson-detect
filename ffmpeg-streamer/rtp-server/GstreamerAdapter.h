#ifndef FFMPEG_STREAMER_GSTREAMERADAPTER_H
#define FFMPEG_STREAMER_GSTREAMERADAPTER_H

#include "IStreamer.h"
#include "gst_sender.h"

class GstreamerAdapter : public IStreamer {
public:
    GstreamerAdapter(std::string addr, size_t port, WGstSender::STREAM_QUALITY quality);
    ~GstreamerAdapter() override;

    bool initializeH264() override;
    bool sendFrame(const std::vector<unsigned char>& frame) override;
    [[nodiscard]] std::pair<int, int> frameSize() const override;

private:
    std::unique_ptr<WGstSender> _gstreamer;
};


#endif //FFMPEG_STREAMER_GSTREAMERADAPTER_H
