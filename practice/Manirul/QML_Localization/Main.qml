import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    width: 800
    height: 600
    visible: true
    title: "Login Application"

    // Current theme
    property string currentTheme: "default"

    // Theme colors
    property color backgroundColor:  currentTheme === "dark" ? "#202020" : currentTheme === "light" ? "#f5f5f5" : "#404040"

    property color textColor: currentTheme === "light" ? "#202020" : "white"

    property color inputColor: currentTheme === "dark" ? "#303030" : currentTheme === "light" ? "white" : "#505050"

    color: backgroundColor

    // ==================================================
    // CUSTOM SIGNAL
    // ==================================================

    signal themeChanged(string theme)

    // ==================================================
    // HANDLE CUSTOM SIGNAL
    // ==================================================
    onThemeChanged: {
        currentTheme = theme
        console.log("Theme changed to:", theme)
    }

    ComboBox {
        id: languageBox

        anchors.top: parent.top
        anchors.right: parent.right

        anchors.topMargin: 15
        anchors.rightMargin: 15

        //Layout.fillWidth: true

        model: [
            "English",
            "বাংলা",
            "हिन्दी"
        ]

        currentIndex: 0

        onCurrentIndexChanged: {
            if (currentIndex === 0) {
                Qt.uiLanguage = ""
            } else if(currentIndex === 1) {
                Qt.uiLanguage = "bn_BD"
            } else if(currentIndex === 2){
                Qt.uiLanguage = "hi_IN"
            }

            console.log("Current index:", currentIndex)
            console.log("UI language:", Qt.uiLanguage)
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        width: 350
        spacing: 15

        Text {
            text: qsTr("Login")
            color: textColor
            font.pixelSize: 32
            font.bold: true
        }

        Text {
            text: qsTr("Username")
            color: textColor
        }

        TextField {
            id: usernameField
            Layout.fillWidth: true

            placeholderText: qsTr("Enter username")
        }

        Text {
            text: qsTr("Password")
            color: textColor
        }

        TextField {
            id: passwordField
            Layout.fillWidth: true

            placeholderText: qsTr("Enter password")

            echoMode: TextInput.Password
        }

        Button {
            Layout.fillWidth: true

            text: qsTr("Login")

            onClicked: {
                console.log("Login clicked")
            }
        }
    }
}