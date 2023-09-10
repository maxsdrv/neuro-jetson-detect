#include "wgstreceiverwrapper.h"

WGstReceiverWrapper::WGstReceiverWrapper(size_t port, QObject* parent) : QObject{parent},
                                            receiver{std::make_unique<WGstReceiver>(port)}
{

}

void WGstReceiverWrapper::startReceiving()
{

}
