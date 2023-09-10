#pragma once

#include <QObject>

#include <gst_receiver.h>

class WGstReceiverWrapper : public QObject
{
    Q_OBJECT

public:
    explicit WGstReceiverWrapper(size_t port, QObject* parent = nullptr);
    ~WGstReceiverWrapper() = default;

private:
    std::unique_ptr<WGstReceiver> receiver;

public slots:
    void startReceiving();
};

