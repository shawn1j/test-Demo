import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls 2.3
import QtCore

import AvCom

ApplicationWindow {
    id: appWindow
    title: qsTr("MavPlayer")
    visible: true
    width: 1600
    height: 920
    color: 'white'

    menuBar: MenuBar {
        Menu {

            title: qsTr("file")
            MenuItem {
                text: qsTr("open")

            }
        }
    }

    GLVideoWidget {
        id: videoView

        anchors.left: appWindow.left
        anchors.right: appWindow.right
        anchors.top: appWindow.menuBar.bottom
        anchors.bottom: play_progress.top
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        anchors.topMargin: 5
        anchors.bottomMargin: 5

    }

    Slider {
        id: play_progress

        value: 0.5
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: displayControlBar.top
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        anchors.bottomMargin: 1
    }

    Rectangle {
        id: displayControlBar

        height: 50
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 0
        anchors.rightMargin: 0

        anchors.bottomMargin: 20


        Button {
                   id: privious_button
                   width: 60
                   text: qsTr("Button")
                   anchors.right: parent.right
                   anchors.top: parent.top
                   anchors.bottom: parent.bottom
                   anchors.rightMargin: 0
                   anchors.topMargin: 0
                   anchors.bottomMargin: 0
                   hoverEnabled: true
                   icon.cache: true
                   display: AbstractButton.IconOnly

               }

    }
}
