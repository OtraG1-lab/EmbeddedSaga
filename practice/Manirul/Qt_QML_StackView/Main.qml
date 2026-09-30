import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    width: 640
    height: 480
    visible: true

    color: "grey"

    title: qsTr("Hello World")

    StackView {
        id: myStackView

        anchors.fill: parent

        initialItem: "Page_1.qml"
    }
}