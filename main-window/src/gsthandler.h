#pragma once

#include <QObject>
#include <QThread>

#include "wgstsenderwrapper.h"
#include "wgstreceiverwrapper.h"

class GstHandler : public QObject
{
    Q_OBJECT
public:
    explicit GstHandler(QObject *parent = nullptr);
    ~GstHandler();

    void start();
    void stop();

private:
    QThread senderThread;
    QThread receiverThread;

    WGstSenderWrapper* senderWrapper;
    WGstReceiverWrapper* receiverWrapper;

signals:
    void errorOccured(const QString&);
    void frameReceived(const QImage& frame);

private slots:
    void handleCapture();
    void handleReceive();
};

