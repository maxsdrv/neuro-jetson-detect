#ifndef FFMPEG_STREAMER_ISTREAMER_H
#define FFMPEG_STREAMER_ISTREAMER_H

#include <vector>

class IStreamer {
public:
    virtual ~IStreamer() = default;
    virtual bool initialize() = 0;
    virtual bool sendFrame(const std::vector<unsigned char>& frame) = 0;
    virtual std::pair<int, int> frameSize() const = 0;
};


#endif //FFMPEG_STREAMER_ISTREAMER_H
