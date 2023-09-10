#pragma once

#include <QObject>

#include <gst_sender.h>

class WGstSenderWrapper : public QObject
{
    Q_OBJECT
public:
    explicit WGstSenderWrapper(const QString& ip,
                               size_t port,
                               WGstSender::STREAM_QUALITY quality,
                               QObject *parent = nullptr);

private:
    std::unique_ptr<WGstSender> sender;

public slots:
    void startSending();
signals:

};

