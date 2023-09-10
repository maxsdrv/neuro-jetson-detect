#include "gsthandler.h"

GstHandler::GstHandler(QObject *parent)
    : QObject{parent}
{
    senderWrapper = new WGstSenderWrapper("192.168.1.133", 5000, WGstSender::STREAM_QUALITY::HIGH, this);
    receiverWrapper = new WGstReceiverWrapper(500, this);
    senderWrapper->moveToThread(&senderThread);
    receiverWrapper->moveToThread(&receiverThread);

    connect(&senderThread, &QThread::started, senderWrapper, &WGstSenderWrapper::startSending);
    connect(&receiverThread, &QThread::started, receiverWrapper, &WGstReceiverWrapper::startReceiving);

    senderThread.start();
    receiverThread.start();
}

GstHandler::~GstHandler()
{
    senderThread.quit();
    receiverThread.quit();
    senderThread.wait();
    receiverThread.wait();
}

void GstHandler::start()
{

}

void GstHandler::stop()
{

}

void GstHandler::handleCapture()
{

}

void GstHandler::handleReceive()
{

}
