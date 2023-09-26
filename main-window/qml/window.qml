import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15
import custom.window.module 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: "Video Stream"

    GWindow {
        id: gstWindow
    }

    RowLayout {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10

        Button {
            id: playBtn
            text: qsTr("Play")

            contentItem: Text {
                text: playBtn.text
                font: playBtn.font
                opacity: enabled ? 1.0 : 0.3
                color: playBtn.down ? "#17a81a" : "#21be2b"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            background: Rectangle {
                implicitWidth: 100
                implicitHeight: 40
                opacity: enabled ? 1 : 0.3
                color: "white"
                border.color: "black"
                border.width: 2
                radius: 10
            }
        }
        Button {
            id: stopBtn
            text: qsTr("Stop")

            contentItem: Text {
                text: stopBtn.text
                font: stopBtn.font
                opacity: enabled ? 1.0 : 0.3
                color: stopBtn.down ? "#17a81a" : "#21be2b"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            background: Rectangle {
                implicitWidth: 100
                implicitHeight: 40
                opacity: enabled ? 1 : 0.3
                color: "white"
                border.color: "black"
                border.width: 2
                radius: 10
            }

        }

    }

}
