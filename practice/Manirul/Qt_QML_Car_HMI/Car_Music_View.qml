import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

Item {
    id: car_Music_View
    anchors.fill: parent

    property url videoSource: "qrc:/Qt_QML_Car_HMI/Images/Resources/Srivalli_pushpa.mp4"
    property bool isMuted: false
    property bool isShuffled: false
    property string songTitle: "Srivalli"
    property string artistName: "Pushpa • Allu Arjun"
    property string albumName: "Pushpa: The Rise"
    property string playbackErrorText: ""
    readonly property bool isPlaying: mediaPlayer.playbackState === MediaPlayer.PlayingState

    function formatTime(milliseconds) {
        var totalSeconds = Math.floor(Math.max(0, milliseconds) / 1000)
        var hours = Math.floor(totalSeconds / 3600)
        var minutes = Math.floor((totalSeconds % 3600) / 60)
        var seconds = totalSeconds % 60
        if (hours > 0)
            return hours.toString().padStart(2, "0") + ":" +
                   minutes.toString().padStart(2, "0") + ":" +
                   seconds.toString().padStart(2, "0")
        return minutes.toString().padStart(2, "0") + ":" +
               seconds.toString().padStart(2, "0")
    }

    MediaPlayer {
        id: mediaPlayer
        source: car_Music_View.videoSource
        videoOutput: videoOutput
        audioOutput: AudioOutput {
            id: audioOutput
            volume: volumeSlider.value
            muted: car_Music_View.isMuted
        }
        // Metadata is optional. If the MP4 contains these tags, use them;
        // otherwise the default title and artist above remain visible.
        onMetaDataChanged: {
            const titleValue = metaData.value(MediaMetaData.Title)
            const artistValue = metaData.value(MediaMetaData.ContributingArtist)
            const albumValue = metaData.value(MediaMetaData.AlbumTitle)

            if (titleValue !== undefined && String(titleValue).length > 0)
                car_Music_View.songTitle = String(titleValue)
            if (artistValue !== undefined && String(artistValue).length > 0) {
                car_Music_View.artistName = Array.isArray(artistValue)
                        ? artistValue.join(", ") : String(artistValue)
            }
            if (albumValue !== undefined && String(albumValue).length > 0)
                car_Music_View.albumName = String(albumValue)

            console.log("Media metadata updated:", car_Music_View.songTitle,
                        car_Music_View.artistName, car_Music_View.albumName)
        }
        onErrorOccurred: function(error, errorString) {
            car_Music_View.playbackErrorText = errorString
            console.error("Music video playback error:", error, errorString,
                          "Source:", source)
        }
        onMediaStatusChanged: {
            if (mediaStatus === MediaPlayer.LoadedMedia) {
                car_Music_View.playbackErrorText = ""
                console.log("Media loaded. Duration:", duration, "ms")
            }
            console.log("Media status:", mediaStatus)
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#101010"
        radius: 28
        clip: true
    }

    Rectangle {
        id: videoPanel
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 24
        anchors.rightMargin: 24
        anchors.topMargin: 18
        height: parent.height * 0.61
        color: "#080808"
        radius: 18
        clip: true

        VideoOutput {
            id: videoOutput
            anchors.fill: parent
            anchors.margins: 4
            fillMode: VideoOutput.PreserveAspectFit
        }

        Column {
            anchors.centerIn: parent
            spacing: 10
            visible: mediaPlayer.playbackState !== MediaPlayer.PlayingState &&
                     mediaPlayer.mediaStatus !== MediaPlayer.LoadedMedia &&
                     mediaPlayer.mediaStatus !== MediaPlayer.Buffered

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "♫"
                color: "#777777"
                font.pixelSize: 56
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: car_Music_View.songTitle
                color: "#ffffff"
                font.pixelSize: 25
                font.bold: true
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Press Play to start the video"
                color: "#bbbbbb"
                font.pixelSize: 15
            }
        }

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 66
            gradient: Gradient {
                GradientStop { position: 0.0; color: "transparent" }
                GradientStop { position: 1.0; color: "#cc000000" }
            }
            visible: mediaPlayer.playbackState === MediaPlayer.PlayingState ||
                     mediaPlayer.playbackState === MediaPlayer.PausedState
            Column {
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.leftMargin: 20
                anchors.bottomMargin: 12
                spacing: 3
                Text {
                    text: car_Music_View.songTitle
                    color: "white"
                    font.pixelSize: 21
                    font.bold: true
                }
                Text {
                    text: car_Music_View.albumName + " • " + car_Music_View.artistName
                    color: "#dddddd"
                    font.pixelSize: 14
                }
            }
        }
    }

    Rectangle {
        id: controlsPanel
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 18
        anchors.rightMargin: 18
        anchors.bottomMargin: 16
        height: Math.max(170, parent.height * 0.29)
        radius: 26
        color: "#303030"

        ColumnLayout {
            anchors.fill: parent
            anchors.leftMargin: 18
            anchors.rightMargin: 18
            anchors.topMargin: 12
            anchors.bottomMargin: 12
            spacing: 8

            RowLayout {
                Layout.fillWidth: true
                spacing: 12
                Text {
                    text: formatTime(mediaPlayer.position)
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                }
                Slider {
                    id: progressSlider
                    Layout.fillWidth: true
                    from: 0
                    to: Math.max(1, mediaPlayer.duration)
                    value: mediaPlayer.position
                    enabled: mediaPlayer.seekable
                    onMoved: mediaPlayer.setPosition(value)
                    background: Rectangle {
                        x: progressSlider.leftPadding
                        y: progressSlider.topPadding + progressSlider.availableHeight / 2 - height / 2
                        width: progressSlider.availableWidth
                        height: 6
                        radius: 3
                        color: "#555555"
                        Rectangle {
                            width: progressSlider.visualPosition * parent.width
                            height: parent.height
                            radius: 3
                            color: "#00a9e8"
                        }
                    }
                    handle: Rectangle {
                        x: progressSlider.leftPadding + progressSlider.visualPosition *
                           (progressSlider.availableWidth - width)
                        y: progressSlider.topPadding + progressSlider.availableHeight / 2 - height / 2
                        width: 14
                        height: 14
                        radius: 7
                        color: "#ffffff"
                        border.color: "#00a9e8"
                        border.width: 2
                    }
                }
                Text {
                    text: formatTime(mediaPlayer.duration)
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                }
            }

            Item { Layout.fillHeight: true }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 20

                RoundButton {
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 60
                    checkable: true
                    checked: car_Music_View.isShuffled
                    text: "🔀"
                    font.pixelSize: 22
                    onToggled: car_Music_View.isShuffled = checked
                    background: Rectangle {
                        radius: width / 2
                        color: parent.checked ? "#00a9e8" : "#626262"
                    }
                    ToolTip.visible: parent.hovered
                    ToolTip.text: "Shuffle"
                }
                RoundButton {
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 60
                    text: "◀|"
                    font.pixelSize: 22
                    onClicked: mediaPlayer.setPosition(0)
                    background: Rectangle { radius: width / 2; color: "#626262" }
                    ToolTip.visible: parent.hovered
                    ToolTip.text: "Restart video"
                }
                RoundButton {
                    Layout.preferredWidth: 74
                    Layout.preferredHeight: 74
                    text: car_Music_View.isPlaying ? "Ⅱ" : "▶"
                    font.pixelSize: 30
                    onClicked: {
                        if (mediaPlayer.playbackState === MediaPlayer.PlayingState)
                            mediaPlayer.pause()
                        else {
                            if (mediaPlayer.mediaStatus === MediaPlayer.EndOfMedia)
                                mediaPlayer.setPosition(0)
                            mediaPlayer.play()
                        }
                    }
                    background: Rectangle { radius: width / 2; color: "#777777" }
                    ToolTip.visible: parent.hovered
                    ToolTip.text: car_Music_View.isPlaying ? "Pause" : "Play"
                }
                RoundButton {
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 60
                    text: "|▶"
                    font.pixelSize: 22
                    onClicked: {
                        mediaPlayer.setPosition(0)
                        mediaPlayer.play()
                    }
                    background: Rectangle { radius: width / 2; color: "#626262" }
                    ToolTip.visible: parent.hovered
                    ToolTip.text: "Play from beginning"
                }
                RoundButton {
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 60
                    text: car_Music_View.isMuted ? "🔇" : "🔊"
                    font.pixelSize: 22
                    onClicked: car_Music_View.isMuted = !car_Music_View.isMuted
                    background: Rectangle { radius: width / 2; color: parent.hovered ? "#777777" : "#626262" }
                    ToolTip.visible: parent.hovered
                    ToolTip.text: car_Music_View.isMuted ? "Unmute" : "Mute"
                }
            }

            Item { Layout.fillHeight: true }

            RowLayout {
                Layout.fillWidth: true
                spacing: 12
                Text {
                    text: "VOLUME"
                    color: "#cccccc"
                    font.pixelSize: 12
                    font.bold: true
                }
                Slider {
                    id: volumeSlider
                    Layout.preferredWidth: 150
                    from: 0
                    to: 1
                    value: 0.8
                    ToolTip.visible: parent.hovered
                    ToolTip.text: Math.round(value * 100) + "%"
                }
                Item { Layout.fillWidth: true }
                Text {
                    text: mediaPlayer.error !== MediaPlayer.NoError
                          ? "Playback error: " + (car_Music_View.playbackErrorText || "check media source/backend")
                          : (mediaPlayer.mediaStatus === MediaPlayer.LoadingMedia
                             ? "Loading video…"
                             : "SRIVALLI • MP4 VIDEO")
                    color: mediaPlayer.error !== MediaPlayer.NoError ? "#ff7777" : "#aaaaaa"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }
            }
        }
    }

    Component.onCompleted: {
        console.log("Music screen ready. Source:", mediaPlayer.source.toString(),
                    "Metadata will be read when available.")
        mediaPlayer.playbackStateChanged.connect(function() {
            console.log("Playback state:", mediaPlayer.playbackState)
        })
    }
}
