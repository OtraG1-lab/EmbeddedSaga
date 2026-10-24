import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: car_Menu_View

    width: parent ? parent.width : 1100
    height: parent ? parent.height : 580

    Rectangle {
        id: carMenuBackground
        anchors.fill: parent
        color: "#161616"
        radius: 30
    }

    GridLayout {
        id: menuGrid
        anchors.fill: carMenuBackground
        anchors.leftMargin: 22
        anchors.rightMargin: 22
        anchors.topMargin: 18
        anchors.bottomMargin: 18

        columns: 5
        rowSpacing: 22
        columnSpacing: 12

        // Keep the order aligned with the reference screenshot.
        Repeater {
            model: [
                { label: "Bluetooth",    file: "0_Bluetooth.png" },
                { label: "Radio",        file: "0_Radio.png" },
                { label: "Spotify",      file: "0_Spotify.png" },
                { label: "Clock",        file: "0_clock.png" },
                { label: "Settings",     file: "0_Settings.png" },
                { label: "Calls",        file: "0_Calls.png" },
                { label: "Contacts",     file: "0_Contacts.png" },
                { label: "Volume",       file: "0_Volume.png" },
                { label: "Wi-Fi",        file: "0_WiFi.png" },
                { label: "Netflix",      file: "0_Netflix.png" },
                { label: "YT Music",     file: "0_YT_Music.png" },
                { label: "Android Auto", file: "0_Android_Auto.png" },
                { label: "DRVM",         file: "0_DRVM.png" },
                { label: "YouTube",      file: "0_YouTube.png" },
                { label: "Users",        file: "0_Users.png" }
            ]

            delegate: Item {
                id: menuIconItem

                required property int index
                required property var modelData

                Layout.row: Math.floor(index / 5)
                Layout.column: index % 5
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 0
                Layout.minimumHeight: 130

                property bool isHovered: tapHandler.hovered
                property bool isPressed: tapHandler.pressed

                scale: isPressed ? 0.94 : 1.0
                Behavior on scale {
                    NumberAnimation { duration: 100 }
                }

                Rectangle {
                    anchors.fill: parent
                    radius: 16
                    color: menuIconItem.isPressed
                           ? "#383838"
                           : menuIconItem.isHovered ? "#252525" : "transparent"
                    Behavior on color {
                        ColorAnimation { duration: 120 }
                    }
                }

                Image {
                    id: menuIconImage
                    source: "qrc:/Qt_QML_Car_HMI/Images/Resources/" + modelData.file
                    width: Math.min(100, parent.width * 0.72)
                    height: width
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }

                Text {
                    text: modelData.label
                    color: "#FFFFFF"
                    font.family: "Arial"
                    font.pixelSize: 20
                    font.bold: true
                    anchors.top: menuIconImage.bottom
                    anchors.topMargin: 5
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                TapHandler {
                    id: tapHandler
                    acceptedButtons: Qt.LeftButton

                    // Intentionally does not navigate or open another window.
                    // This confirms the tap in Qt Creator's Application Output.
                    onTapped: console.log(modelData.label + " icon tapped")
                }

                HoverHandler {
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }
    }
}
