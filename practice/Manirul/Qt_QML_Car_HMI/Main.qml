import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "."

Window {
    id: window

    width: 1200
    height: 600
    minimumHeight: 600
    maximumHeight: 600
    minimumWidth: 1200
    maximumWidth: 1200

    visible: true
    title: qsTr("Qt Car HMI")

    Component.onCompleted: window_Visibility(carMainRectangle)

    Rectangle {
        id: mainBackground
        anchors.fill: parent
        color: "#000000"
        radius: 20

        ColumnLayout {
            id: clMainControls
            spacing: 40
            anchors.verticalCenter: parent.verticalCenter

            Image {
                id: imageMainCar
                source: "qrc:/Images/Resources/Car_Default.png"
                Layout.leftMargin: 20
                Layout.preferredWidth: 60
                Layout.preferredHeight: 60
                fillMode: Image.PreserveAspectFit

                MouseArea {
                    anchors.fill: parent
                    onClicked: window_Visibility(carMainRectangle)
                }
            }

            Image {
                id: imageMainNavigation
                source: "qrc:/Images/Resources/Navigation_Default.png"
                Layout.leftMargin: 20
                Layout.preferredWidth: 60
                Layout.preferredHeight: 60
                fillMode: Image.PreserveAspectFit

                MouseArea {
                    anchors.fill: parent
                    onClicked: window_Visibility(navigationMainRectangle)
                }
            }

            Image {
                id: imageMainHome
                source: "qrc:/Images/Resources/Home_Default.png"
                Layout.leftMargin: 20
                Layout.preferredWidth: 60
                Layout.preferredHeight: 60
                fillMode: Image.PreserveAspectFit

                MouseArea {
                    anchors.fill: parent
                    onClicked: window_Visibility(homeMainRectangle)
                }
            }

            Image {
                id: imageMainMusic
                source: "qrc:/Images/Resources/Music_Default.png"
                Layout.leftMargin: 20
                Layout.preferredWidth: 60
                Layout.preferredHeight: 60
                fillMode: Image.PreserveAspectFit

                MouseArea {
                    anchors.fill: parent
                    onClicked: window_Visibility(musicMainRectangle)
                }
            }

            Image {
                id: imageMainMenu
                source: "qrc:/Images/Resources/Menu_Default.png"
                Layout.leftMargin: 20
                Layout.preferredWidth: 60
                Layout.preferredHeight: 60
                fillMode: Image.PreserveAspectFit

                MouseArea {
                    anchors.fill: parent
                    onClicked: window_Visibility(menuMainRectangle)
                }
            }
        }
    }

    Rectangle {
        id: carMainRectangle
        color: "#161616"
        x: clMainControls.width + 20
        y: (parent.height - height) / 2
        width: (parent.width - 10) - (clMainControls.width + 20)
        height: parent.height - 20
        radius: 30

        Car_Main_View {
            anchors.fill: parent
        }
    }

    Rectangle {
        id: navigationMainRectangle
        color: "#161616"
        x: clMainControls.width + 20
        y: (parent.height - height) / 2
        width: (parent.width - 10) - (clMainControls.width + 20)
        height: parent.height - 20
        radius: 30

        Car_Navigation_View {
            id: carNavigationView
            anchors.fill: parent
        }
    }

    Rectangle {
        id: homeMainRectangle
        color: "#161616"
        x: clMainControls.width + 20
        y: (parent.height - height) / 2
        width: (parent.width - 10) - (clMainControls.width + 20)
        height: parent.height - 20
        radius: 30

        Car_Home_View {
            anchors.fill: parent
        }
    }

    Rectangle {
        id: musicMainRectangle
        color: "#161616"
        x: clMainControls.width + 20
        y: (parent.height - height) / 2
        width: (parent.width - 10) - (clMainControls.width + 20)
        height: parent.height - 20
        radius: 30

        Car_Music_View {
            anchors.fill: parent
        }
    }

    Rectangle {
        id: menuMainRectangle
        color: "#161616"
        x: clMainControls.width + 20
        y: (parent.height - height) / 2
        width: (parent.width - 10) - (clMainControls.width + 20)
        height: parent.height - 20
        radius: 30

        Car_Menu_View {
            anchors.fill: parent
        }
    }

    function window_Visibility(item) {
        if (item === carMainRectangle) {
            carMainRectangle.z = 1
            carMainRectangle.enabled = true
            imageMainCar.source =
                    "qrc:/Qt_QML_Car_HMI/Images/Resources/Car_Selected.png"
        } else {
            carMainRectangle.z = 0
            carMainRectangle.enabled = false
            imageMainCar.source =
                    "qrc:/Qt_QML_Car_HMI/Images/Resources/Car_Default.png"
        }

        if (item === navigationMainRectangle) {
            navigationMainRectangle.z = 1
            navigationMainRectangle.enabled = true
            imageMainNavigation.source =
                    "qrc:/Qt_QML_Car_HMI/Images/Resources/Navigation_Selected.png"
        } else {
            navigationMainRectangle.z = 0
            navigationMainRectangle.enabled = false
            imageMainNavigation.source =
                    "qrc:/Qt_QML_Car_HMI/Images/Resources/Navigation_Default.png"
        }

        if (item === homeMainRectangle) {
            homeMainRectangle.z = 1
            homeMainRectangle.enabled = true
            imageMainHome.source =
                    "qrc:/Qt_QML_Car_HMI/Images/Resources/Home_Selected.png"
        } else {
            homeMainRectangle.z = 0
            homeMainRectangle.enabled = false
            imageMainHome.source =
                    "qrc:/Qt_QML_Car_HMI/Images/Resources/Home_Default.png"
        }

        if (item === musicMainRectangle) {
            musicMainRectangle.z = 1
            musicMainRectangle.enabled = true
            imageMainMusic.source =
                    "qrc:/Qt_QML_Car_HMI/Images/Resources/Music_Selected.png"
        } else {
            musicMainRectangle.z = 0
            musicMainRectangle.enabled = false
            imageMainMusic.source =
                    "qrc:/Qt_QML_Car_HMI/Images/Resources/Music_Default.png"
        }

        if (item === menuMainRectangle) {
            menuMainRectangle.z = 1
            menuMainRectangle.enabled = true
            imageMainMenu.source =
                    "qrc:/Qt_QML_Car_HMI/Images/Resources/Menu_Selected.png"
        } else {
            menuMainRectangle.z = 0
            menuMainRectangle.enabled = false
            imageMainMenu.source =
                    "qrc:/Qt_QML_Car_HMI/Images/Resources/Menu_Default.png"
        }
    }
}
