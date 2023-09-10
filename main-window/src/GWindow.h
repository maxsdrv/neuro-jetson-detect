#pragma once

#include <QObject>
#include <QDebug>

class GWindow : public QObject {
    Q_OBJECT

public:
    explicit GWindow(QObject* parent = nullptr);
    ~GWindow() = default;

private:

};
