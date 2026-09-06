import QtQuick
import QtQuick.Controls

ApplicationWindow {
    visible: true

    width: 640
    height: 480

    title: "Raspberry Pi Demo"

    Label {
        anchors.centerIn: parent
        text: "Hello Raspberry Pi!"
        font.pixelSize: 32
    }
}