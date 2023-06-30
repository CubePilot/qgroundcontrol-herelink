import QtQuick                  2.3
import QtPositioning            5.2
import QtQuick.Layouts          1.2
import QtQuick.Controls         1.4
import QtQuick.Dialogs          1.2
import QtGraphicalEffects       1.0

import QGroundControl                   1.0
import QGroundControl.ScreenTools       1.0
import QGroundControl.Controls          1.0
import QGroundControl.Palette           1.0
import QGroundControl.Vehicle           1.0
import QGroundControl.Controllers       1.0
import QGroundControl.FactSystem        1.0
import QGroundControl.FactControls      1.0

Item {
    property Fact _showMenuNavWidget:           QGroundControl.settingsManager.igcSettings.showMenuNavWidget

    id: container
    width: container.childrenRect.width
    height: container.childrenRect.height
    x: parent.width - width - 10
    y: parent.height - height - 10

    ColumnLayout {
        id: col
        Layout.fillWidth: true
        Layout.fillHeight: true
        spacing: 10
        anchors.bottom: parent.bottom

        x: parent.width - width - 500

        Rectangle {
            color: "lightblue"
            width: parent.width
            height: parent.height
            z: -1
        }

        RowLayout {
            QGCButton {
                text: "Up"
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true

                onClicked: {
                    activeVehicle.wiris.MOVE_UP();
                }
            }
        }
        RowLayout {
            QGCButton {
                text: "Left"
                Layout.alignment: Qt.AlignVCenter

                onClicked: {
                    activeVehicle.wiris.MOVE_CANCEL();
                }
            }
            QGCButton {
                text: "Right"
                Layout.alignment: Qt.AlignVCenter

                onClicked: {
                    activeVehicle.wiris.MOVE_OK();
                }
            }
        }
        RowLayout {
            QGCButton {
                text: "Down"
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true

                onClicked: {
                    activeVehicle.wiris.MOVE_DOWN();
                }
            }
        }
    }
}
