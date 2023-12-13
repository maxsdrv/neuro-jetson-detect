#ifndef FFMPEG_STREAMER_FFMPEGADAPTER_H
#define FFMPEG_STREAMER_FFMPEGADAPTER_H

#include "IStreamer.h"

#ifdef ENABLE_FFMPEG
#include <FFMpegStreamer.h>
#endif

class FFmpegAdapter : public IStreamer {
public:
    bool initializeH264() override;
    bool sendFrame(const std::vector<unsigned char>& frame) override;
    [[nodiscard]] std::pair<int, int> frameSize() const override;

private:

};


#endif //FFMPEG_STREAMER_FFMPEGADAPTER_H
