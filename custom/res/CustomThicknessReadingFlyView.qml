import QtQuick                  2.11
import QtQuick.Controls         2.4
import QtQuick.Layouts          1.11
import QtQuick.Dialogs          1.3
import Qt.labs.platform         1.1
import Qt.labs.settings         1.0

import QGroundControl           1.0
import QGroundControl.Palette   1.0

Item {
    Connections {
        target: QGroundControl.multiVehicleManager
        onActiveVehicleChanged: {
            customPlugin.connectContext
        }
    }

    anchors.fill: parent

    MessageDialog {
        id: messageDialog
    }

    QGCPalette { id: qgcPal }

    visible: true//customPlugin.getThicknessGaugeEnabled

    Component.onCompleted: {
    }

    Rectangle {
        id: mainThicknessRectangle
        
        x: 140
        y: 100
        color: Qt.rgba(255,255,255,255)
        radius: 240
        width: 240
        height: 240
        clip: true

        border.color: customPlugin.getBorderColor
        border.width: 8

        MouseArea {
            anchors.fill: parent
            drag.target: mainThicknessRectangle
            drag.axis: Drag.XAndYAxis
        }

        Item {
            id: thicknessText
            width: 240
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.leftMargin: 8
            anchors.rightMargin: 8
            anchors.topMargin: 8
            anchors.bottomMargin: 8

            Text {
                id: readingValue
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                text: '%1 mm'.arg(customPlugin.getThicknessReading.toFixed(2))
                font.pixelSize: 40
                font.bold: true
                color: "#000"
            }
        }
    }

}
