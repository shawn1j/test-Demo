import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Pane {
    property color color: 'white'
    property int duration: 0
    property font textFont: ({
            "family": "Arial",
            "pointSize": 9
        })
    width: 80
    height: 50
    background: Rectangle {
        color: color
    }

    Row {
        id: row
        anchors.fill: parent
        layoutDirection: Qt.LeftToRight
        spacing: 5

        Text {
            id: hour
            text: duration.toString()
            anchors.verticalCenter: parent.verticalCenter
            font: textFont
        }

        Text {
            id: _separator_1
            text: qsTr(":")
            anchors.verticalCenter: parent.verticalCenter
            font: textFont
        }

        Text {
            id: minute
            text: duration.toString()
            anchors.verticalCenter: parent.verticalCenter
            font: textFont
        }

        Text {
            id: _separator_2
            text: qsTr(":")
            anchors.verticalCenter: parent.verticalCenter
            font: textFont
        }

        Text {
            id: second
            text: duration.toString()
            anchors.verticalCenter: parent.verticalCenter
            font: textFont
        }
    }
}
