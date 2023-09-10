import QtQuick 2.15
import QtQuick.Controls 2.15
import custom.window.module 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: "Video Stream"

    GWindow {
        id: gstWindow
    }

}
