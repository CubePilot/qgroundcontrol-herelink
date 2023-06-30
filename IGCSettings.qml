import QtQuick          2.11
import QtQuick.Controls 2.4
import QtQuick.Dialogs  1.3
import QtQuick.Layouts  1.11
import QtQuick.Window   2.11

import QGroundControl                       1.0
import QGroundControl.Controls 1.0
import QGroundControl.Palette 1.0
import QGroundControl.Controllers 1.0
import QGroundControl.ScreenTools 1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0
import QGroundControl.SettingsManager       1.0

// check settings


Rectangle {
    id: page
    width: 320
    height: 480
    color:              qgcPal.window

    property Fact _isEthernetCommEnabled:       QGroundControl.settingsManager.igcSettings.isEthernetCommEnabled
    property Fact _cameraIpAddress:             QGroundControl.settingsManager.igcSettings.cameraIpAddress
    property Fact _cameraInfoUri:               QGroundControl.settingsManager.igcSettings.cameraInfoUri
    property Fact _showMenuNavWidget:           QGroundControl.settingsManager.igcSettings.showMenuNavWidget
    property bool _hasActiveVehicle:            activeVehicle
    property var   _dynamicCameras:             activeVehicle ? activeVehicle.dynamicCameras : null
    property var  _showHidden:                  false
    property var  _ethernetIsConnected:         activeVehicle.wiris.isConnected()

    Component.onCompleted: {
        // Add your code here
        console.log("Page loaded! " + activeVehicle == null)

    }

    Text {
        id: titleText
        text: "Inspired Ground Control Settings"
        y: 30
        anchors.horizontalCenter: page.horizontalCenter
        font.pointSize: 24
        font.bold: true
    }

    Rectangle {
        width: 200
        height: 200
        anchors.right: parent.right
        anchors.top: parent.top

        MouseArea {
            anchors.fill: parent
            onClicked: {
                _showHidden = !_showHidden;
            }
        }
    }

    Rectangle {
        property int padding: 10

        //Layout.preferredWidth:  buttonColumn.width + (_margins * 2)
        //Layout.preferredHeight: buttonColumn.height  + (_margins * 2)
        Layout.fillWidth:       true
        color:                  qgcPal.windowShade
        y: 120
        width: childrenRect.width + padding
        height: childrenRect.height + padding

        ColumnLayout {
            id:         buttonColumn
            x: 100
            //y: 120
            spacing:    _verticalMargin
            width: parent.width - padding
            height: parent.height - padding
            anchors.centerIn: parent

            readonly property real _defaultTextHeight:  ScreenTools.defaultFontPixelHeight
            readonly property real _verticalMargin:     _defaultTextHeight / 2

            RowLayout {
                QGCLabel {
                    id: result
                }
            }

            RowLayout {
                FactCheckBox {
                    text:       qsTr("Enable ethernet camera communication")
                    fact:       _isEthernetCommEnabled
                    visible:    _isEthernetCommEnabled.visible
                }
            }

            RowLayout {
                QGCLabel {
                    id: ipLabel
                    text: "Camera IP Address"
                }
                QGCTextField {
                    id: ipText
                    text: _cameraIpAddress.valueString
                }
            }

            RowLayout {
                QGCLabel {
                    id: cameraInfoUriLabel
                    text: "Camera Info Uri"
                }
                Item {
                    width: 1000
                    height: cameraInfoUriText.height

                    QGCTextField {
                        id: cameraInfoUriText
                        text: _cameraInfoUri.valueString
                        width: 1000
                    }
                }
            }

            RowLayout {
                QGCButton {
                    text: "Connect"
                    enabled: _hasActiveVehicle
                    onClicked: {
                        result.text = "connect";

                        _cameraIpAddress.rawValue = ipText.text;

                        //activeVehicle.wiris.set_ip_address(ipText.text);
                        activeVehicle.wiris.connect();
                    }
                }
                QGCLabel {
                    id: connectionStateLabel
                    text: _ethernetIsConnected ? "connected" : "disconnected";
                    color: _ethernetIsConnected ? "green" : "red";
                }
            }

    //        QGCButton {
    //            text: "zoom in"
    //            enabled: _hasActiveVehicle
    //            onClicked: {
    //                result.text = "zoom in";
    //                activeVehicle.wiris.SZIN();
    //            }
    //        }

    //        QGCButton {
    //            text: "zoom out"
    //            enabled: _hasActiveVehicle
    //            onClicked: {
    //                result.text = "zoom out";
    //                activeVehicle.wiris.SZOT();
    //            }
    //        }

    //        QGCButton {
    //            text: "trigger camera"
    //            enabled: _hasActiveVehicle
    //            onClicked: {
    //                result.text = "trigger camera";
    //                activeVehicle.wiris.CPTR();
    //            }
    //        }

            RowLayout {
                QGCButton {
                    text: "Reboot Camera"
                    enabled: _hasActiveVehicle
                    onClicked: {
                        result.text = "reboot";
                        activeVehicle.wiris.REBT();
                    }
                }
                QGCLabel {
                    id: labelReboot
                    text: "WARNING: a camera reboot can take several minutes"
                }
            }

            RowLayout {
                FactCheckBox {
                    text:       qsTr("Show menu nav widget")
                    fact:       _showMenuNavWidget
                    visible:    _showMenuNavWidget.visible
                }
            }

            RowLayout {
                visible: _showHidden

                QGCButton {
                    text: "Rediscover Camera"
                    enabled: _hasActiveVehicle
                    onClicked: {
                        result.text = "rediscover";
                        _dynamicCameras.requestCameraInfo();
                    }
                }
                QGCLabel {
                    id: labelRediscover
                    text: "Try this after the camera has rebooted"
                }
            }
            property var    _dynamicCameras:        activeVehicle ? activeVehicle.dynamicCameras : null

            RowLayout {
                visible: _showHidden

                QGCLabel {
                    id: commandLabel
                    text: "Command"
                }
                QGCTextField {
                    id: commandText
                    text: ""
                }
                QGCButton {
                    text: "Send"
                    enabled: _hasActiveVehicle
                    onClicked: {
                        result.text = "Send " + commandText.text;
                        activeVehicle.wiris.write_command(commandText.text + "\n");
                    }
                }
            }
        }
    }
}
