import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtLocation
import QtPositioning

Item {
    id: carNavigationView

    anchors.fill: parent

    // Demo coordinates similar to the route shown in the reference screenshot.
    // Replace these with the actual coordinates supplied by your route backend.
    // Bangalore (Bengaluru)
    property var startCoordinate: QtPositioning.coordinate(12.9716, 77.5946)

    // Kolkata
    property var endCoordinate:QtPositioning.coordinate(22.5726, 88.3639)

    // Initially, vehicle is at Bangalore
    property var currentCoordinate: QtPositioning.coordinate(12.9716, 77.5946)

    property var routeCoordinates: [
        // Bangalore
        QtPositioning.coordinate(12.9716, 77.5946),

        // Approximate intermediate waypoints
        QtPositioning.coordinate(14.6819, 77.6006),  // Anantapur area
        QtPositioning.coordinate(17.3850, 78.4867),  // Hyderabad
        QtPositioning.coordinate(21.1458, 79.0882),  // Nagpur
        QtPositioning.coordinate(21.2514, 81.6296),  // Raipur
        QtPositioning.coordinate(21.4669, 83.9756),  // Sambalpur area
        QtPositioning.coordinate(22.3460, 87.2320),  // Kharagpur area

        // Kolkata
        QtPositioning.coordinate(22.5726, 88.3639)
    ]

    property bool navigationStarted: false

    // Simulation settings
    property real demoSpeedKmh: 3600
    property int currentSegmentIndex: 0
    property real segmentProgressKm: 0

    // Demonstration instructions for each route segment.
    // These are simulated maneuvers, not real road directions.
    property var maneuverInstructions: [
        "Continue straight",
        "Turn right",
        "Turn left",
        "Continue straight",
        "Turn right",
        "Turn left",
        "Continue straight"
    ]

    // Named destination indices in routeCoordinates:
    // Bengaluru = 0, Hyderabad = 2, Nagpur = 3, Kolkata = 7.
    property int nextWaypointIndex: {
        if (currentSegmentIndex < 2)
            return 2; // Hyderabad

        if (currentSegmentIndex < 3)
            return 3; // Nagpur

        return routeCoordinates.length - 1; // Kolkata
    }

    property string nextWaypointName: {
        if (nextWaypointIndex === 2)
            return "Hyderabad";

        if (nextWaypointIndex === 3)
            return "Nagpur";

        return "Kolkata";
    }

    property string currentManeuver: {
        if (currentSegmentIndex >= maneuverInstructions.length)
            return "Destination reached";

        return maneuverInstructions[currentSegmentIndex];
    }

    property real distanceToNextWaypointKm:
        distanceAlongRouteToWaypoint(nextWaypointIndex)

    property real totalRemainingDistanceKm:
        distanceAlongRouteToWaypoint(routeCoordinates.length - 1)


    // Simulated vehicle movement timer
    Timer {
        id: navigationTimer
        interval: 250
        repeat: true
        running: carNavigationView.navigationStarted

        onTriggered: {
            carNavigationView.advanceNavigationDemo()
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#080808"
        radius: 28
        clip: true
    }

    Rectangle {
        id: panel
        anchors.fill: parent
        anchors.margins: 10
        radius: 22
        color: "#171717"
        clip: true
    }

    ColumnLayout {
        anchors.fill: panel
        anchors.margins: 10
        spacing: 0

        Rectangle {
            id: header
            Layout.fillWidth: true
            Layout.preferredHeight: 64
            color: "#242424"
            radius: 10

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 16
                anchors.rightMargin: 16
                spacing: 12

                Text {
                    text: {
                        if (!carNavigationView.navigationStarted)
                            return "Welcome";

                        if (carNavigationView.currentManeuver.toLowerCase().includes("left"))
                            return "↰ Turn left";

                        if (carNavigationView.currentManeuver.toLowerCase().includes("right"))
                            return "↱ Turn right";

                        return "↑ " + carNavigationView.currentManeuver;
                    }

                    color: "white"
                    font.pixelSize: 20
                    font.bold: true
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                Text {
                    visible: carNavigationView.navigationStarted

                    text: carNavigationView.nextWaypointName
                          + " • "
                          + carNavigationView.distanceToNextWaypointKm.toFixed(1)
                          + " km"

                    color: "#00AEEF"
                    font.pixelSize: 15
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        Item {
            id: mapArea
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            Plugin {
                id: mapPlugin
                name: "osm"
            }

            Map {
                id: navigationMap
                anchors.fill: parent
                plugin: mapPlugin
                center: QtPositioning.coordinate(
                            (startCoordinate.latitude + endCoordinate.latitude) / 2,
                            (startCoordinate.longitude + endCoordinate.longitude) / 2)
                zoomLevel: 5
                minimumZoomLevel: 2
                maximumZoomLevel: 18

                // Demo route line. A real route should use coordinates from a routing service.
                MapPolyline {
                    id: routeLine
                    line.width: 5
                    line.color: "#159447"
                    path: carNavigationView.routeCoordinates
                    visible: carNavigationView.routeCoordinates.length > 1
                }

                // The three marker Components are instantiated as map items below.
                Component {
                    id: locMarker_Start
                    MapQuickItem {
                        id: startMarker
                        anchorPoint.x: startMarkerVisual.width / 2
                        anchorPoint.y: startMarkerVisual.height
                        sourceItem: Item {
                            id: startMarkerVisual
                            width: 34
                            height: 42

                            Rectangle {
                                anchors.horizontalCenter: parent.horizontalCenter
                                y: 1
                                width: 27
                                height: 27
                                radius: 14
                                color: "#E53935"
                                border.color: "white"
                                border.width: 2

                                Text {
                                    anchors.centerIn: parent
                                    text: "S"
                                    color: "white"
                                    font.bold: true
                                    font.pixelSize: 14
                                }
                            }

                            Rectangle {
                                anchors.horizontalCenter: parent.horizontalCenter
                                y: 24
                                width: 3
                                height: 14
                                color: "#E53935"
                            }
                        }
                    }
                }

                Component {
                    id: locMarker_End
                    MapQuickItem {
                        id: endMarker
                        anchorPoint.x: endMarkerVisual.width / 2
                        anchorPoint.y: endMarkerVisual.height
                        sourceItem: Item {
                            id: endMarkerVisual
                            width: 34
                            height: 42

                            Rectangle {
                                anchors.horizontalCenter: parent.horizontalCenter
                                y: 1
                                width: 27
                                height: 27
                                radius: 14
                                color: "#168EF0"
                                border.color: "white"
                                border.width: 2

                                Text {
                                    anchors.centerIn: parent
                                    text: "D"
                                    color: "white"
                                    font.bold: true
                                    font.pixelSize: 14
                                }
                            }

                            Rectangle {
                                anchors.horizontalCenter: parent.horizontalCenter
                                y: 24
                                width: 3
                                height: 14
                                color: "#168EF0"
                            }
                        }
                    }
                }

                Component {
                    id: locMarker_Current
                    MapQuickItem {
                        id: currentMarker
                        anchorPoint.x: currentMarkerVisual.width / 2
                        anchorPoint.y: currentMarkerVisual.height / 2
                        sourceItem: Item {
                            id: currentMarkerVisual
                            width: 34
                            height: 34

                            Rectangle {
                                anchors.centerIn: parent
                                width: 28
                                height: 28
                                radius: 14
                                color: "#111111"
                                border.color: "#F44336"
                                border.width: 3
                            }

                            Rectangle {
                                anchors.centerIn: parent
                                width: 12
                                height: 12
                                radius: 6
                                color: "#F44336"
                            }
                        }
                    }
                }

                // Create map items and add them once the Map has completed.
                Component.onCompleted: {
                    carNavigationView.setLocationMarking_Start_Function(
                                carNavigationView.startCoordinate.latitude,
                                carNavigationView.startCoordinate.longitude)
                    carNavigationView.setLocationMarking_End_Function(
                                carNavigationView.endCoordinate.latitude,
                                carNavigationView.endCoordinate.longitude)
                    carNavigationView.setLocationMarking_Current_Function(
                                carNavigationView.currentCoordinate.latitude,
                                carNavigationView.currentCoordinate.longitude)
                }
            }

            // Zoom and fit controls, aligned vertically on the right.
            Column {
                anchors.right: parent.right
                anchors.rightMargin: 14
                anchors.verticalCenter: parent.verticalCenter
                spacing: 10

                Repeater {
                    model: [
                        { symbol: "+", action: "in" },
                        { symbol: "−", action: "out" },
                        { symbol: "⛶", action: "fit" }
                    ]

                    delegate: Rectangle {
                        required property var modelData
                        width: 48
                        height: 48
                        radius: 24
                        color: "#171717"
                        border.color: "#555555"
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            text: modelData.symbol
                            color: "white"
                            font.pixelSize: modelData.action === "fit" ? 23 : 30
                            font.bold: true
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (modelData.action === "in") {
                                    navigationMap.zoomLevel = Math.min(
                                                navigationMap.maximumZoomLevel,
                                                navigationMap.zoomLevel + 1)
                                } else if (modelData.action === "out") {
                                    navigationMap.zoomLevel = Math.max(
                                                navigationMap.minimumZoomLevel,
                                                navigationMap.zoomLevel - 1)
                                } else {
                                    navigationMap.zoomLevel = 5
                                    navigationMap.center = QtPositioning.coordinate(
                                                (carNavigationView.startCoordinate.latitude
                                                 + carNavigationView.endCoordinate.latitude) / 2,
                                                (carNavigationView.startCoordinate.longitude
                                                 + carNavigationView.endCoordinate.longitude) / 2)
                                }
                            }
                        }
                    }
                }
            }
        }

        Button {
            id: startNavigationButton
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            Layout.topMargin: 4

            text: navigationStarted ? "Stop Navigation" : "Start Navigation"

            background: Rectangle {
                color: "#171717"
                border.color: "#A8A8A8"
                border.width: 1
                radius: 3
            }

            contentItem: Text {
                text: startNavigationButton.text
                color: "#FFFFFF"
                font.pixelSize: 20
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: {
                if (carNavigationView.navigationStarted) {
                    carNavigationView.navigationStarted = false
                } else {
                    if (carNavigationView.routeCoordinates.length < 2) {
                        console.log("Add at least two route coordinates first")
                        return
                    }

                    if (carNavigationView.currentSegmentIndex
                            >= carNavigationView.routeCoordinates.length - 1) {
                        carNavigationView.resetNavigationDemo()
                    }

                    carNavigationView.navigationStarted = true
                    navigationMap.center = carNavigationView.currentCoordinate
                    navigationMap.zoomLevel = 7
                }
            }
        }
    }

    // Marker references are retained so each marker can be removed and replaced safely.
    property var mapItem_Start: null
    property var mapItem_End: null
    property var mapItem_Current: null

    function setLocationMarking_Start_Function(lati, longi) {
        if (mapItem_Start !== null) {
            navigationMap.removeMapItem(mapItem_Start)
            mapItem_Start.destroy()
        }

        mapItem_Start = locMarker_Start.createObject(navigationMap, {
            coordinate: QtPositioning.coordinate(lati, longi)
        })

        if (mapItem_Start !== null)
            navigationMap.addMapItem(mapItem_Start)
    }

    function setLocationMarking_End_Function(lati, longi) {
        if (mapItem_End !== null) {
            navigationMap.removeMapItem(mapItem_End)
            mapItem_End.destroy()
        }

        mapItem_End = locMarker_End.createObject(navigationMap, {
            coordinate: QtPositioning.coordinate(lati, longi)
        })

        if (mapItem_End !== null)
            navigationMap.addMapItem(mapItem_End)
    }

    function setLocationMarking_Current_Function(lati, longi) {
        if (mapItem_Current !== null) {
            navigationMap.removeMapItem(mapItem_Current)
            mapItem_Current.destroy()
        }

        mapItem_Current = locMarker_Current.createObject(navigationMap, {
            coordinate: QtPositioning.coordinate(lati, longi)
        })

        if (mapItem_Current !== null)
            navigationMap.addMapItem(mapItem_Current)
    }

    function clearPath_Function() {
        routeCoordinates = []
    }

    function setRouteCoordinates(coordinates) {
        routeCoordinates = coordinates
    }


    function coordinateDistanceKm(a, b) {
        if (!a || !b || !a.isValid || !b.isValid)
            return 0

        var earthRadiusKm = 6371
        var toRadians = Math.PI / 180

        var lat1 = a.latitude * toRadians
        var lat2 = b.latitude * toRadians
        var deltaLat = (b.latitude - a.latitude) * toRadians
        var deltaLon = (b.longitude - a.longitude) * toRadians

        var h = Math.sin(deltaLat / 2) * Math.sin(deltaLat / 2)
                + Math.cos(lat1) * Math.cos(lat2)
                * Math.sin(deltaLon / 2) * Math.sin(deltaLon / 2)

        h = Math.max(0, Math.min(1, h))
        return 2 * earthRadiusKm * Math.asin(Math.sqrt(h))
    }

    function distanceAlongRouteToWaypoint(targetIndex) {
        if (routeCoordinates.length < 2)
            return 0

        targetIndex = Math.max(
            0,
            Math.min(targetIndex, routeCoordinates.length - 1)
        )

        if (currentSegmentIndex >= targetIndex)
            return 0

        var distance = coordinateDistanceKm(
            currentCoordinate,
            routeCoordinates[currentSegmentIndex + 1]
        )

        for (var i = currentSegmentIndex + 1; i < targetIndex; ++i) {
            distance += coordinateDistanceKm(
                routeCoordinates[i],
                routeCoordinates[i + 1]
            )
        }

        return distance
    }


    function resetNavigationDemo() {
        if (routeCoordinates.length < 2)
            return

        currentSegmentIndex = 0
        segmentProgressKm = 0
        currentCoordinate = routeCoordinates[0]

        if (mapItem_Current)
            mapItem_Current.coordinate = currentCoordinate

        navigationMap.center = currentCoordinate
        navigationMap.zoomLevel = 7
    }

    function advanceNavigationDemo() {
        if (routeCoordinates.length < 2)
            return

        var distanceToTravel =
                demoSpeedKmh * navigationTimer.interval / 3600000

        while (distanceToTravel > 0
               && currentSegmentIndex < routeCoordinates.length - 1) {

            var start = routeCoordinates[currentSegmentIndex]
            var end = routeCoordinates[currentSegmentIndex + 1]
            var segmentLength = coordinateDistanceKm(start, end)

            if (segmentLength < 0.000001) {
                currentSegmentIndex++
                segmentProgressKm = 0
                continue
            }

            var remaining = segmentLength - segmentProgressKm

            if (distanceToTravel >= remaining) {
                distanceToTravel -= remaining
                currentSegmentIndex++
                segmentProgressKm = 0
                currentCoordinate = end
            } else {
                segmentProgressKm += distanceToTravel
                distanceToTravel = 0

                var fraction = segmentProgressKm / segmentLength

                currentCoordinate = QtPositioning.coordinate(
                    start.latitude
                        + (end.latitude - start.latitude) * fraction,
                    start.longitude
                        + (end.longitude - start.longitude) * fraction
                )
            }
        }

        if (mapItem_Current)
            mapItem_Current.coordinate = currentCoordinate

        navigationMap.center = currentCoordinate

        if (currentSegmentIndex >= routeCoordinates.length - 1) {
            navigationStarted = false
            navigationTimer.stop()
        }
    }

}
