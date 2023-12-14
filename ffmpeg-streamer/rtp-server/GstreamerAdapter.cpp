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
    return false;
}

bool GstreamerAdapter::sendFrame(const std::vector<unsigned char> &frame) {
    return false;
}

std::pair<int, int> GstreamerAdapter::frameSize() const {
    return std::pair<int, int>();
}
