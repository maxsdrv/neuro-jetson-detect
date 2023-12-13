
#include "FFmpegAdapter.h"

bool FFmpegAdapter::initializeH264() {
    return false;
}

std::pair<int, int> FFmpegAdapter::frameSize() const {

    return std::pair<int, int>();
}

bool FFmpegAdapter::sendFrame(const std::vector<unsigned char> &frame) {
    return false;
}
