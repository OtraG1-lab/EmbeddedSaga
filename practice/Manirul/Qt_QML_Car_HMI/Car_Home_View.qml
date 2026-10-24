import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    width: 1200
    height: 620
    property real speedValue: 0
    property bool speedIncreasing: true
    property string selectedGear: "D"
    property real cabinTemperature: 16
    property real fanLevel: 50
    property bool autoMode: false
    property bool acMode: true
    property bool recirculationMode: false
    property bool frontDefrostMode: false
    property bool rearDefrostMode: false
    readonly property color backgroundColor: "#090b0d"
    readonly property color cardColor: "#1c2226"
    readonly property color blue: "#08a9ed"
    readonly property color white: "#f4f6f8"
    readonly property color muted: "#9ba4aa"
    readonly property string resourceBase: "qrc:/Qt_QML_Car_HMI/Images/Resources/"

    Rectangle { anchors.fill: parent; radius: 28; color: root.backgroundColor; border.color: "#222a30"; border.width: 2 }
    Text {
        text: "Qt With Mani"; color: root.white; font.pixelSize: Math.max(17, root.width*0.022); font.bold: true
        anchors.right: parent.right; anchors.rightMargin: 24; anchors.top: parent.top; anchors.topMargin: 18
    }

    RowLayout {
        id: topPanels
        anchors.left: parent.left; anchors.right: parent.right
        anchors.leftMargin: 22; anchors.rightMargin: 22
        anchors.top: parent.top; anchors.topMargin: 72
        height: root.height * 0.43
        spacing: 18

        // LEFT PANEL: Ford image and mileage figures
        Rectangle {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 1.08
            radius: 18; color: "#101316"; border.color: "#29333b"; border.width: 2; clip: true
            Image {
                anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter
                width: parent.width*0.70; height: parent.height*0.88
                source: root.resourceBase + "Ford_S-max.jpg"
                fillMode: Image.PreserveAspectFit; smooth: true
                onStatusChanged: if (status === Image.Error) console.warn("Cannot load Ford_S-max.jpg:", source)
            }
            Column {
                anchors.right: parent.right; anchors.rightMargin: 14; anchors.verticalCenter: parent.verticalCenter; spacing: 17
                Repeater {
                    model: ["999 Km", "888 Km", "777 Km"]
                    delegate: Text { text: modelData; color: root.white; font.pixelSize: Math.max(18, parent.parent.width*0.045); font.bold: true }
                }
            }
        }

        // CENTER PANEL: compact speedometer adapted from Car_Main_View.qml
        Rectangle {
            id: speedCard
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 1.35
            radius: 18; color: root.cardColor; border.color: "#29333b"; border.width: 2; clip: true
            Canvas {
                id: speedCanvas
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top; anchors.topMargin: 7
                width: Math.min(parent.width*0.76, parent.height*1.18)
                height: parent.height*0.72
                antialiasing: true
                onPaint: {
                    var ctx = getContext("2d"); ctx.reset();
                    var cx = width / 2, cy = height * 0.82;
                    // Scaled equivalent of Car_Main_View.qml's outer, progress and inner arcs.
                    var outerRadius = Math.min(width * 0.46, height * 0.72);
                    var scale = outerRadius / 218.0;
                    var middleRadius = outerRadius * (190.0 / 218.0);
                    var innerRadius = outerRadius * (164.0 / 218.0);
                    var start = 135 * Math.PI / 180;
                    var end = 405 * Math.PI / 180;
                    var progress = Math.max(0, Math.min(1, root.speedValue / 240));
                    function arc(radius, lineWidth, color, fromAngle, toAngle) {
                        ctx.beginPath();
                        ctx.arc(cx, cy, radius, fromAngle, toAngle, false);
                        ctx.strokeStyle = color;
                        ctx.lineWidth = lineWidth;
                        ctx.lineCap = "butt";
                        ctx.stroke();
                    }
                    arc(outerRadius, Math.max(3, 20 * scale), root.white, start, end);
                    arc(middleRadius, Math.max(3, 18 * scale), root.blue, start, start + (end - start) * progress);
                    arc(innerRadius, Math.max(1.5, 4 * scale), root.white, start, end);
                }
                Connections { target: root; function onSpeedValueChanged() { speedCanvas.requestPaint() } }
                Component.onCompleted: requestPaint()
            }
            Column {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter; anchors.verticalCenterOffset: -2
                spacing: 0
                Text {
                    text: Math.round(root.speedValue); color: root.white
                    font.pixelSize: Math.min(speedCard.width*0.13, speedCard.height*0.22)
                    font.bold: true; anchors.horizontalCenter: parent.horizontalCenter
                }
                Text {
                    text: "mph"; color: root.white
                    font.pixelSize: Math.min(speedCard.width*0.065, speedCard.height*0.10)
                    font.bold: true; anchors.horizontalCenter: parent.horizontalCenter
                }
            }
            Row {
                anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom; anchors.bottomMargin: 8
                spacing: Math.max(7, parent.width*0.022)
                Repeater {
                    model: ["P", "R", "D", "N", "L"]
                    delegate: Rectangle {
                        width: 29; height: 29; radius: 7
                        color: root.selectedGear===modelData ? "#103347" : "transparent"
                        border.color: root.selectedGear===modelData ? root.blue : "transparent"; border.width: 2
                        Text { anchors.centerIn: parent; text: modelData; color: root.selectedGear===modelData ? root.white : root.muted; font.pixelSize: 16; font.bold: true }
                        MouseArea { anchors.fill: parent; onClicked: root.selectedGear=modelData }
                    }
                }
            }
        }

        // RIGHT PANEL: weather PNG and environmental information
        Rectangle {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 1.0
            radius: 18; color: root.cardColor; border.color: "#29333b"; border.width: 2; clip: true
            Row {
                anchors.left: parent.left; anchors.leftMargin: 16; anchors.right: parent.right; anchors.rightMargin: 12
                anchors.top: parent.top; anchors.topMargin: 16; spacing: 6
                Column {
                    spacing: 2
                    Text { text: "24°"; color: root.white; font.pixelSize: Math.min(38, parent.parent.width*0.15); font.bold: true }
                    Text { text: "Sunny"; color: root.white; font.pixelSize: 16; font.bold: true; anchors.horizontalCenter: parent.horizontalCenter }
                }
                Image {
                    source: root.resourceBase + "sunny.png"
                    width: Math.min(72, parent.parent.width*0.30); height: width
                    fillMode: Image.PreserveAspectFit
                    onStatusChanged: if (status === Image.Error) console.warn("Cannot load sunny.png:", source)
                }
            }
            Rectangle {
                anchors.left: parent.left; anchors.right: parent.right
                anchors.top: parent.top; anchors.topMargin: parent.height * 0.55
                anchors.leftMargin: 16; anchors.rightMargin: 16
                height: 1; color: "#59636b"
            }
            Column {
                anchors.left: parent.left; anchors.right: parent.right
                anchors.top: parent.top; anchors.topMargin: parent.height * 0.62
                anchors.leftMargin: 16; anchors.rightMargin: 16; spacing: 12
                RowLayout {
                    width: parent.width
                    Text { text: "Air Quality"; color: root.muted; font.pixelSize: Math.max(12, Math.min(15, parent.width * 0.055)); font.bold: true }
                    Item { Layout.fillWidth: true }
                    Text { text: "Good"; color: "#62e36c"; font.pixelSize: Math.max(12, Math.min(15, parent.width * 0.055)); font.bold: true }
                }
                RowLayout {
                    width: parent.width
                    Text { text: "UV Index"; color: root.muted; font.pixelSize: Math.max(12, Math.min(15, parent.width * 0.055)); font.bold: true }
                    Item { Layout.fillWidth: true }
                    Text { text: "Low"; color: root.blue; font.pixelSize: Math.max(12, Math.min(15, parent.width * 0.055)); font.bold: true }
                }
            }
        }
    }

    // Five circular climate controls
    Row {
        id: climateButtons
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: topPanels.bottom; anchors.topMargin: root.height*0.055
        spacing: Math.max(12, root.width*0.025)
        Repeater {
            model: [
                {image:"auto.png", key:"auto"}, {image:"ac.png", key:"ac"},
                {image:"recirculation.png", key:"recirc"},
                {image:"front_defrost.png", key:"front"}, {image:"rear_defrost.png", key:"rear"}
            ]
            delegate: Rectangle {
                width: Math.min(82, root.width*0.075); height: width; radius: width/2
                color: ((modelData.key==="auto" && root.autoMode) || (modelData.key==="ac" && root.acMode) || (modelData.key==="recirc" && root.recirculationMode) || (modelData.key==="front" && root.frontDefrostMode) || (modelData.key==="rear" && root.rearDefrostMode)) ? "#183544" : "#151b20"
                border.color: "#A9CCE8"; border.width: 2
                Image {
                    anchors.fill: parent; anchors.margins: 5
                    source: root.resourceBase + modelData.image; fillMode: Image.PreserveAspectFit
                    onStatusChanged: if (status === Image.Error) console.warn("Cannot load climate icon:", source)
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (modelData.key==="auto") root.autoMode=!root.autoMode
                        else if (modelData.key==="ac") root.acMode=!root.acMode
                        else if (modelData.key==="recirc") root.recirculationMode=!root.recirculationMode
                        else if (modelData.key==="front") root.frontDefrostMode=!root.frontDefrostMode
                        else root.rearDefrostMode=!root.rearDefrostMode
                    }
                }
            }
        }
    }

    // Bottom climate sliders: temperature and fan speed
    RowLayout {
        anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom
        anchors.leftMargin: 32; anchors.rightMargin: 32; anchors.bottomMargin: 25; spacing: 15
        Image {
            source: root.resourceBase + "temperature.png"
            Layout.preferredWidth: 30; Layout.preferredHeight: 44; fillMode: Image.PreserveAspectFit
            onStatusChanged: if (status === Image.Error) console.warn("Cannot load temperature.png:", source)
        }
        Slider {
            id: temperatureSlider; from: 16; to: 30; value: root.cabinTemperature
            Layout.fillWidth: true; Layout.preferredHeight: 36
            onMoved: root.cabinTemperature=Math.round(value)
        }
        Text { text: root.cabinTemperature + "°C"; color: root.white; font.pixelSize: 22; font.bold: true; Layout.preferredWidth: 64; horizontalAlignment: Text.AlignHCenter }
        Image {
            source: root.resourceBase + "fan.png"
            Layout.preferredWidth: 36; Layout.preferredHeight: 40; fillMode: Image.PreserveAspectFit
            onStatusChanged: if (status === Image.Error) console.warn("Cannot load fan.png:", source)
        }
        Slider {
            id: fanSlider; from: 0; to: 100; value: root.fanLevel
            Layout.fillWidth: true; Layout.preferredHeight: 36
            onMoved: root.fanLevel=Math.round(value)
        }
        Text { text: root.fanLevel + "%"; color: root.white; font.pixelSize: 22; font.bold: true; Layout.preferredWidth: 60; horizontalAlignment: Text.AlignHCenter }
    }

    // Timer animation copied from Car_Main_View.qml's speed simulation.
    // Speed rises from 0 to 220 mph, then decreases back to 0 and repeats.
    Timer {
        id: speedSimulationTimer
        interval: 100
        repeat: true
        running: true
        onTriggered: {
            if (root.speedIncreasing) {
                root.speedValue += 2
                if (root.speedValue >= 220) {
                    root.speedValue = 220
                    root.speedIncreasing = false
                }
            } else {
                root.speedValue -= 2
                if (root.speedValue <= 0) {
                    root.speedValue = 0
                    root.speedIncreasing = true
                }
            }
            speedCanvas.requestPaint()
        }
    }

    Component.onCompleted: speedCanvas.requestPaint()

}
