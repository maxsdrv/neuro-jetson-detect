#include "wgstsenderwrapper.h"

WGstSenderWrapper::WGstSenderWrapper(const QString& ip,
                                     size_t port,
                                     WGstSender::STREAM_QUALITY quality,
                                     QObject *parent)
    : QObject{parent},
    sender{std::make_unique<WGstSender>(ip.toStdString(), port, quality)}
{

}

void WGstSenderWrapper::startSending()
{

}
