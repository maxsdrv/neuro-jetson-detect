#include <iostream>

#include "GstreamerAdapter.h"

GstreamerAdapter::GstreamerAdapter(std::string addr, size_t port, WGstSender::STREAM_QUALITY quality)
    : _gstreamer{std::make_unique<WGstSender>(std::move(addr), port, quality)}
{

}

GstreamerAdapter::~GstreamerAdapter() {
    std::cout << __func__ << std::endl;
}

bool GstreamerAdapter::initializeH264() {
    if (!_gstreamer->stable_start()) {
        std::cerr << "Error start rtp sender.\n";
        return false;
    }

    _gstreamer->play();

    return true;
}

bool GstreamerAdapter::sendFrame(const std::vector<unsigned char> &frame) {
    if (!_gstreamer->send(frame)) {
        std::cerr << "Error send frame.\n";
        return false;
    }

    return true;
}

std::pair<int, int> GstreamerAdapter::frameSize() const {
    return std::make_pair(640, 480);
}

