import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtGraphs

ApplicationWindow {
    id: win
    width: 720
    height: 720
    minimumWidth: 720
    minimumHeight: 720
    maximumWidth: 720
    maximumHeight: 720
    visible: true
    title: "IoT Server"
    Component.onCompleted: {
        if (autoStart)
            backend.start(protoBox.currentIndex === 0, portBox.value, thresholdBox.value)
    }
    font.family: "Segoe UI"
    font.pixelSize: 14

    background: Rectangle {
        color: "#1b6b96"
    }

    // --------------------------------------------------
    // Top control toolbar
    // --------------------------------------------------
    header: ToolBar {
        height: 58
        background: Rectangle { color: "#182832" }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16
            spacing: 16

            Label { text: "Protocol"; color: "white" }

            ComboBox {
                id: protoBox
                currentIndex: startProtoIndex
                Layout.preferredWidth: 110
                model: ["TCP", "UDP"]
                enabled: !backend.running
            }

            Label { text: "Port"; color: "white" }

            SpinBox {
                id: portBox
                Layout.preferredWidth: 110
                from: 1024
                to: 65535
                value: startPort
                editable: true
                enabled: !backend.running
                textFromValue: function(v) { return v.toString() }
            }

            Button {
                text: "Start"
                Layout.preferredWidth: 90
                enabled: !backend.running
                background: Rectangle {
                    implicitHeight: 36
                    radius: 6
                    color: parent.down ? "#2ecc71" : (parent.enabled ? "#27ae60" : "#555555")
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: backend.start(protoBox.currentIndex === 0, portBox.value, thresholdBox.value)
            }

            Button {
                text: "Stop"
                Layout.preferredWidth: 90
                enabled: backend.running
                background: Rectangle {
                    implicitHeight: 36
                    radius: 6
                    color: parent.down ? "#c0392b" : (parent.enabled ? "#e74c3c" : "#555555")
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: backend.stop()
            }

            Item { Layout.fillWidth: true }

            Rectangle {
                Layout.preferredWidth: 18
                Layout.preferredHeight: 18
                radius: 9
                color: backend.ledOn ? "#2ecc71" : "#7f8c8d"
            }

            Label {
                text: backend.ledOn ? "LED: ON" : "LED: OFF"
                color: "white"
                font.bold: true
                font.pixelSize: 15
            }
        }
    }

    // --------------------------------------------------
    // Bottom status bar
    // --------------------------------------------------
    footer: ToolBar {
        height: 34
        background: Rectangle { color: "#182832" }
        Label {
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            text: backend.status
            color: "white"
            font.pixelSize: 12
        }
    }

    // --------------------------------------------------
    // Main application layout
    // --------------------------------------------------
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Navigation tabs – full-width, clean style matching first screenshot
        TabBar {
            id: bar
            Layout.fillWidth: true
            Layout.preferredHeight: 42
            background: Rectangle { color: "#243640" }

            // shared style for every tab
            component StyledTab: TabButton {
                width: Math.max(implicitWidth + 28, 140)
                height: bar.height
                background: Rectangle {
                    color: "transparent"
                    Rectangle {                         // blue underline
                        width: parent.width
                        height: 3
                        anchors.bottom: parent.bottom
                        color: parent.parent.checked ? "#4fc3f7" : "transparent"
                    }
                }
                contentItem: Text {
                    text: parent.text
                    color: parent.checked ? "white" : "#aaaaaa"
                    font.bold: parent.checked
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            StyledTab { text: "Real Time Monitor" }
            StyledTab { text: "Historical Analysis" }
            StyledTab { text: "Configuration" }
            StyledTab { text: "Quick Access" }
        }

        // Content area with the complete background image
        Item {
            id: pageArea
            Layout.fillWidth: true
            Layout.fillHeight: true

            Image {
                id: backgroundImage
                anchors.fill: parent
                source: "images/Background.jpg"
                fillMode: Image.PreserveAspectFit
                smooth: true
            }

            StackLayout {
                id: pages
                anchors.fill: parent
                currentIndex: bar.currentIndex

                // ------------------------------------------
                // Tab 1: Real Time Monitor
                // ------------------------------------------
                Item {
                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 16

                        Gauge {
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredWidth: 400
                            Layout.preferredHeight: 400
                            value: backend.temperature
                        }

                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: "Threshold: " + backend.threshold + " °C"
                            color: "white"
                            font.pixelSize: 20
                            font.bold: true
                        }
                    }
                }

                // ------------------------------------------
                // Tab 2: Historical Analysis
                // ------------------------------------------
                Item {
                    id: historyTab
                    property int sampleIndex: 0

                    Column {
                        anchors.centerIn: parent
                        spacing: 8

                        // Graph title
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "Historical Temperature Data - Interactive View"
                            color: "white"
                            font.pixelSize: 18
                            font.bold: true
                        }

                        GraphsView {
                            id: graph
                            width: 550
                            height: 380

                            axisX: ValueAxis {
                                id: xAxis
                                min: 0
                                max: 60
                                titleText: "Time"
                                titleVisible: true
                                titleColor: "white"
                                labelsVisible: true
                            }

                            axisY: ValueAxis {
                                min: -50
                                max: 150
                                titleText: "Temperature (°C)"
                                titleVisible: true
                                titleColor: "white"
                                labelsVisible: true
                            }

                            LineSeries {
                                id: line
                                color: "#4fc3f7"
                                width: 3
                            }
                        }
                    }

                    // Clear button (positioned relative to the graph)
                    Button {
                        anchors.right: parent.right
                        anchors.rightMargin: 80
                        anchors.top: parent.top
                        anchors.topMargin: 50
                        text: "Clear"
                        background: Rectangle {
                            implicitHeight: 32
                            implicitWidth: 80
                            radius: 6
                            color: parent.down ? "#555555" : "#333333"
                            border.color: "#777777"
                        }
                        contentItem: Text {
                            text: parent.text
                            color: "white"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            line.clear()
                            historyTab.sampleIndex = 0
                            xAxis.min = 0
                            xAxis.max = 60
                        }
                    }

                    Connections {
                        target: backend
                        function onSample(v) {
                            line.append(historyTab.sampleIndex, v)
                            historyTab.sampleIndex++
                            if (historyTab.sampleIndex > xAxis.max) {
                                xAxis.min = historyTab.sampleIndex - 60
                                xAxis.max = historyTab.sampleIndex
                            }
                        }
                    }
                }

                // ------------------------------------------
                // Tab 3: Configuration
                // ------------------------------------------
                Item {
                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 16

                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: "Critical temperature threshold (°C)"
                            color: "white"
                            font.pixelSize: 20
                            font.bold: true
                        }

                        SpinBox {
                            id: thresholdBox
                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredWidth: 140
                            from: -100
                            to: 200
                            value: 30
                            editable: true
                        }

                        Button {
                            Layout.alignment: Qt.AlignHCenter
                            text: "Apply threshold"
                            enabled: backend.running
                            background: Rectangle {
                                implicitHeight: 36
                                implicitWidth: 140
                                radius: 6
                                color: parent.down ? "#2980b9" : (parent.enabled ? "#3498db" : "#555555")
                            }
                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            onClicked: backend.applyThreshold(thresholdBox.value)
                        }
                    }
                }
                // ------------------------------------------
                // Tab 4: Quick Access
                // ------------------------------------------
                Item {
                    // helper component – blue background, white text/icons
                    component SocialButton: Button {
                        id: control
                        property string iconSource
                        property string url

                        background: Rectangle {
                            implicitWidth: 150
                            implicitHeight: 40
                            radius: 4
                            color: control.down ? "#155a80" : "#1a7db8"   // same blue as page
                            border.color: "#ffffff"
                            border.width: 1.5
                        }
                        contentItem: Row {
                            spacing: 8
                            anchors.centerIn: parent

                        Image {
                                source: control.iconSource
                                width: 22
                                height: 22
                                fillMode: Image.PreserveAspectFit
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Text {
                                text: control.text
                                color: "white"
                                font.pixelSize: 14
                                font.bold: true
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                        onClicked: if (url) Qt.openUrlExternally(url)
                    }

                    Column {
                        anchors.centerIn: parent
                        spacing: 14

                        Row {
                            spacing: 16
                            anchors.horizontalCenter: parent.horizontalCenter

                            SocialButton {
                                text: "Facebook"
                                iconSource: "images/facebook-app-symbol.png"
                                url: "https://www.facebook.com/share/19hfjncKaN/"
                            }
                            SocialButton {
                                text: "LinkedIn"
                                iconSource: "images/linkedin.png"
                                url: "https://www.linkedin.com/company/edges-for-training/"
                            }
                            SocialButton {
                                text: "Instagram"
                                iconSource: "images/instagram.png"
                                url: "https://www.instagram.com/edgesfortraining"
                            }
                        }

                        SocialButton {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "Yasseen Abdelwahab"
                            iconSource: "images/linkedin.png"
                            url: "https://www.linkedin.com/in/yasseen-abdelwahab-b3669219b"
                        }
                    }
                }
            }
        }
    }
}