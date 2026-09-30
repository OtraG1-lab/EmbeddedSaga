import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    width: 300
    height: 300
    color: "#FF0000"
    anchors.centerIn: parent


    ColumnLayout{
        anchors.fill: parent
        spacing: 20
        Text{
            id: myText_1
            text: qsTr("This is first Page")
            font.pointSize: 20
            font.bold: true
        }

        Button{
            id: myButton_1
            text: "Goto Page 2"

            onClicked: {
                myStackView.push("Page_2.qml")
            }
        }

        Button{
            id: myButton_2
            text: "Goto Page 3"

            onClicked:
            {
                myStackView.push("Page_3.qml")
            }
        }
    }
}
