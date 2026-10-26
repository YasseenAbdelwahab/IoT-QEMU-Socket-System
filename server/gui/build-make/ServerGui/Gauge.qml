// Gauge.qml
import QtQuick
import QtQuick.Shapes

Item {
    id: root
    property real value: 0
    property real minValue: -50
    property real maxValue: 150
    readonly property real startAngle: 135       // clockwise from 3 o'clock
    readonly property real sweep: 270
    readonly property real frac: Math.max(0, Math.min(1,
        (value - minValue) / (maxValue - minValue)))
    implicitWidth: 340
    implicitHeight: 340

    Shape {
        anchors.fill: parent
        layer.enabled: true
        layer.samples: 4

        ShapePath {                               // background track
            strokeWidth: 18; strokeColor: "#55ffffff"
            fillColor: "transparent"; capStyle: ShapePath.RoundCap
            PathAngleArc {
                centerX: root.width / 2; centerY: root.height / 2
                radiusX: root.width / 2 - 24; radiusY: root.height / 2 - 24
                startAngle: root.startAngle; sweepAngle: root.sweep
            }
        }
        ShapePath {                               // value arc
            strokeWidth: 18
            strokeColor: root.frac > 0.66 ? "#e74c3c"
                        : root.frac > 0.33 ? "#f1c40f" : "#2ecc71"
            fillColor: "transparent"; capStyle: ShapePath.RoundCap
            PathAngleArc {
                centerX: root.width / 2; centerY: root.height / 2
                radiusX: root.width / 2 - 24; radiusY: root.height / 2 - 24
                startAngle: root.startAngle; sweepAngle: root.sweep * root.frac
            }
        }
    }

    Item {                                        // needle
        x: root.width / 2; y: root.height / 2
        width: 0; height: 0
        rotation: root.startAngle + root.sweep * root.frac
        Rectangle {
            x: 0; y: -2
            width: root.width / 2 - 50; height: 4; radius: 2
            color: "white"
        }
    }
    Rectangle {                                   // hub
        anchors.centerIn: parent
        width: 20; height: 20; radius: 10; color: "white"
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 60
        text: root.value.toFixed(1) + " °C"
        color: "white"; font.pixelSize: 30; font.bold: true
    }
    Text {
        x: 18; y: parent.height - 34
        text: root.minValue; color: "#cccccc"; font.pixelSize: 14
    }
    Text {
        x: parent.width - width - 18; y: parent.height - 34
        text: root.maxValue; color: "#cccccc"; font.pixelSize: 14
    }
}