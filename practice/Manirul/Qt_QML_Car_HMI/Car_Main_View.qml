import QtQuick
import QtQuick.Controls

Item {
    id: root
    width: 1280
    height: 720

    // Demo values. Replace these with real vehicle/CAN data when available.
    property real speedValue: 0
    property real rpmValue: 1.0
    property real fuelValue: 0.50
    property string selectedGear: "D"
    property bool speedIncreasing: true
    property bool rpmIncreasing: true

    readonly property color blueColor: "#079fe8"
    readonly property color whiteColor: "#f7f7f7"
    readonly property color guideColor: "#fff000"

    // All three gauges share the same centre so the side arcs align with
    // the main speedometer. Adjust these values together if your view differs.
    readonly property real gaugeCenterX: width / 2
    readonly property real gaugeCenterY: height / 2 + 8
    readonly property real sideArcRadius: 330

    function degreeToRadians(degree) {
        return degree * Math.PI / 180.0
    }

    // LEFT ARC: RPM scale, shaped like the reference image.
    // The arc runs from upper-left to lower-left around the outside of the center gauge.
    Canvas {
        id: rpmCanvas
        anchors.fill: parent
        antialiasing: true

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()

            var cx = root.gaugeCenterX
            var cy = root.gaugeCenterY
            var radius = root.sideArcRadius
            var startAngle = root.degreeToRadians(135) // lower-left
            var endAngle = root.degreeToRadians(225)   // upper-left, through the left side
            var sweep = endAngle - startAngle
            var progress = Math.max(0, Math.min(1, (root.rpmValue - 1) / 4))

            // Thin white full arc.
            ctx.beginPath()
            ctx.arc(cx, cy, radius, startAngle, endAngle, false)
            ctx.strokeStyle = root.guideColor
            ctx.lineWidth = 2.5
            ctx.stroke()

            // Blue section grows upward from the lower-left endpoint.
            ctx.beginPath()
            ctx.arc(cx, cy, radius, startAngle, startAngle + sweep * progress, false)
            ctx.strokeStyle = root.blueColor
            ctx.lineWidth = 24
            ctx.lineCap = "butt"
            ctx.stroke()
        }
    }

    // RPM labels follow the curve of the left arc (1 at bottom, 5 at top).
    Repeater {
        model: 5
        delegate: Text {
            required property int index
            readonly property real labelAngle: root.degreeToRadians(135 + index * 22.5)
            readonly property real labelRadius: root.sideArcRadius + 24

            x: root.gaugeCenterX + Math.cos(labelAngle) * labelRadius - width / 2
            y: root.gaugeCenterY + Math.sin(labelAngle) * labelRadius - height / 2
            width: 36
            height: 36
            text: index + 1
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: root.blueColor
            font.pixelSize: 28
            font.bold: true
        }
    }

    // RPM label beside the lower endpoint of the left arc.
    Text {
        id: rpmTitle
        x: root.gaugeCenterX - root.sideArcRadius + 108
        y: root.gaugeCenterY + root.sideArcRadius * 0.70
        text: "RPM"
        color: root.guideColor
        font.pixelSize: 20
        font.bold: true
    }

    // CENTER SPEEDOMETER: outer thick ring, blue progress ring, inner thin ring.
    Canvas {
        id: speedometerCanvas
        x: root.gaugeCenterX - width / 2
        y: root.gaugeCenterY - height / 2
        width: 600
        height: 560
        antialiasing: true

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()

            var cx = width / 2
            var cy = height / 2
            var outerRadius = 218
            var middleRadius = 190
            var innerRadius = 164
            var startAngle = root.degreeToRadians(135)
            var endAngle = root.degreeToRadians(405)
            var sweep = endAngle - startAngle
            var progress = Math.max(0, Math.min(1, root.speedValue / 240))

            ctx.beginPath()
            ctx.arc(cx, cy, outerRadius, startAngle, endAngle, false)
            ctx.strokeStyle = root.whiteColor
            ctx.lineWidth = 20
            ctx.lineCap = "butt"
            ctx.stroke()

            ctx.beginPath()
            ctx.arc(cx, cy, middleRadius, startAngle, startAngle + sweep * progress, false)
            ctx.strokeStyle = root.blueColor
            ctx.lineWidth = 18
            ctx.lineCap = "butt"
            ctx.stroke()

            ctx.beginPath()
            ctx.arc(cx, cy, innerRadius, startAngle, endAngle, false)
            ctx.strokeStyle = root.whiteColor
            ctx.lineWidth = 4
            ctx.stroke()
        }
    }

    // Digital speed readout.
    Rectangle {
        x: root.gaugeCenterX - 100
        y: root.gaugeCenterY - 93
        width: 200
        height: 4
        color: root.whiteColor
    }

    Text {
        id: speedDataText
        x: root.gaugeCenterX - 125
        y: root.gaugeCenterY - 80
        width: 250
        height: 92
        text: Math.round(root.speedValue)
        color: root.whiteColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 82
        font.bold: true
        font.family: "Monospace"
    }

    Rectangle {
        x: root.gaugeCenterX - 100
        y: root.gaugeCenterY + 29
        width: 200
        height: 4
        color: root.whiteColor
    }

    Text {
        id: unitDataText
        x: root.gaugeCenterX - 100
        y: root.gaugeCenterY + 37
        width: 200
        text: "mph"
        color: root.whiteColor
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 38
        font.bold: true
    }

    // Fixed gear selector for the demo: P R < D > N L.
    Row {
        id: gearSelector
        x: root.gaugeCenterX - width / 2
        y: root.gaugeCenterY + 111
        spacing: 14

        Text {
            text: "P"
            color: root.blueColor
            font.pixelSize: 18
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            text: "R"
            color: root.blueColor
            font.pixelSize: 18
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            text: "‹"
            color: root.whiteColor
            font.pixelSize: 34
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            id: selectedGearText
            text: root.selectedGear
            color: root.whiteColor
            font.pixelSize: 68
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            text: "›"
            color: root.whiteColor
            font.pixelSize: 34
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            text: "N"
            color: root.blueColor
            font.pixelSize: 18
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            text: "L"
            color: root.blueColor
            font.pixelSize: 18
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    // RIGHT ARC: fuel scale, matching the reference shape.
    // Thin white outline; blue fill represents the fuel level from EMPTY upward.
    Canvas {
        id: fuelCanvas
        anchors.fill: parent
        antialiasing: true

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()

            var cx = root.gaugeCenterX
            var cy = root.gaugeCenterY
            var radius = root.sideArcRadius
            var startAngle = root.degreeToRadians(315) // upper-right
            var endAngle = root.degreeToRadians(405)   // lower-right
            var sweep = endAngle - startAngle
            var progress = Math.max(0, Math.min(1, root.fuelValue))

            // Thin white arc from upper-right to lower-right via the right side.
            ctx.beginPath()
            ctx.arc(cx, cy, radius, startAngle, endAngle, false)
            ctx.strokeStyle = root.guideColor
            ctx.lineWidth = 2.5
            ctx.stroke()

            // Blue segment begins at the bottom (EMPTY) and grows upward.
            ctx.beginPath()
            ctx.arc(cx, cy, radius, endAngle - sweep * progress, endAngle, false)
            ctx.strokeStyle = root.blueColor
            ctx.lineWidth = 24
            ctx.lineCap = "butt"
            ctx.stroke()

            // Short white end markers, matching the reference.
            var angles = [startAngle, endAngle]
            for (var i = 0; i < angles.length; ++i) {
                var a = angles[i]
                ctx.beginPath()
                ctx.moveTo(cx + Math.cos(a) * (radius - 14),
                           cy + Math.sin(a) * (radius - 14))
                ctx.lineTo(cx + Math.cos(a) * (radius + 14),
                           cy + Math.sin(a) * (radius + 14))
                ctx.strokeStyle = root.guideColor
                ctx.lineWidth = 3
                ctx.stroke()
            }
        }
    }

    // Fuel labels follow the right arc and stay inside the window.
    Text {
        id: fullDataText
        x: root.gaugeCenterX + root.sideArcRadius * 0.70 + 36
        y: root.gaugeCenterY - root.sideArcRadius * 0.70 - 16
        text: "FULL"
        color: root.blueColor
        font.pixelSize: 28
        font.bold: true
    }

    Text {
        id: halfDataText
        x: root.gaugeCenterX + root.sideArcRadius + 16
        y: root.gaugeCenterY - height / 2
        text: "HALF"
        color: root.blueColor
        font.pixelSize: 28
        font.bold: true
    }

    Text {
        id: emptyDataText
        x: root.gaugeCenterX + root.sideArcRadius * 0.70 + 32
        y: root.gaugeCenterY + root.sideArcRadius * 0.70 - 20
        text: "LOW"
        color: root.blueColor
        font.pixelSize: 28
        font.bold: true
    }

    // FUEL label beside the lower-right endpoint, kept close to LOW.
    Text {
        id: fuelTitle
        x: root.gaugeCenterX + root.sideArcRadius * 0.70 - 44
        y: root.gaugeCenterY + root.sideArcRadius * 0.70
        text: "FUEL"
        color: root.guideColor
        font.pixelSize: 20
        font.bold: true
    }

    // Timer simulates speed and RPM. Fuel stays at 50% for now.
    Timer {
        id: clusterTimer
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

            if (root.rpmIncreasing) {
                root.rpmValue += 0.04
                if (root.rpmValue >= 5.0) {
                    root.rpmValue = 5.0
                    root.rpmIncreasing = false
                }
            } else {
                root.rpmValue -= 0.04
                if (root.rpmValue <= 1.0) {
                    root.rpmValue = 1.0
                    root.rpmIncreasing = true
                }
            }

            speedometerCanvas.requestPaint()
            rpmCanvas.requestPaint()
            fuelCanvas.requestPaint()
        }
    }

    Component.onCompleted: {
        speedometerCanvas.requestPaint()
        rpmCanvas.requestPaint()
        fuelCanvas.requestPaint()
    }
}
