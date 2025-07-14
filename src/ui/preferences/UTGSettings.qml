/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick          2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts  1.2
import QtQuick.Dialogs  1.2

import QGroundControl                       1.0
import QGroundControl.FactSystem            1.0
import QGroundControl.FactControls          1.0
import QGroundControl.Controls              1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.Palette               1.0
import QGroundControl.SettingsManager       1.0

Rectangle {
    id:                 _root
    color:              qgcPal.window
    anchors.fill:       parent
    anchors.margins:    ScreenTools.defaultFontPixelWidth

    property var _activeVehicle:    QGroundControl.multiVehicleManager.activeVehicle
    property var _utgSettings:      null
    property real _margins:         ScreenTools.defaultFontPixelHeight
    property real _panelWidth:      _root.width * 0.8

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    // Safely get UTG settings
    Component.onCompleted: {
        try {
            _utgSettings = QGroundControl.settingsManager.utgSettings
        } catch (error) {
            console.log("Error accessing UTG settings:", error)
        }
    }

    QGCFlickable {
        clip:               true
        anchors.fill:       parent
        contentHeight:      settingsColumn.height
        contentWidth:       settingsColumn.width

        Column {
            id:                 settingsColumn
            width:              _root.width
            spacing:            _margins

            QGCLabel {
                text:           qsTr("UTG (Ultrasonic Thickness Gauge) Settings")
                font.family:    ScreenTools.demiboldFontFamily
            }



            Rectangle {
                width:  _panelWidth
                height: enabledColumn.height + (_margins * 2)
                color:  qgcPal.windowShade
                visible: _utgSettings !== null

                Column {
                    id:                 enabledColumn
                    anchors.margins:    _margins
                    anchors.top:        parent.top
                    anchors.left:       parent.left
                    anchors.right:      parent.right
                    spacing:            _margins

                    QGCLabel {
                        text:           qsTr("General Settings")
                        font.family:    ScreenTools.demiboldFontFamily
                    }

                    FactCheckBox {
                        text:       qsTr("Enable UTG")
                        fact:       _utgSettings ? _utgSettings.enabled : null
                        visible:    _utgSettings && _utgSettings.enabled ? _utgSettings.enabled.visible : false
                    }

                    FactCheckBox {
                        text:       qsTr("Auto Connect")
                        fact:       _utgSettings ? _utgSettings.autoConnect : null
                        visible:    _utgSettings && _utgSettings.autoConnect ? _utgSettings.autoConnect.visible : false
                        enabled:    _utgSettings && _utgSettings.enabled ? _utgSettings.enabled.rawValue : false
                    }
                }
            }

            Rectangle {
                width:  _panelWidth
                height: serialColumn.height + (_margins * 2)
                color:  qgcPal.windowShade
                visible: _utgSettings && _utgSettings.enabled ? _utgSettings.enabled.rawValue : false

                Column {
                    id:                 serialColumn
                    anchors.margins:    _margins
                    anchors.top:        parent.top
                    anchors.left:       parent.left
                    anchors.right:      parent.right
                    spacing:            _margins

                    QGCLabel {
                        text:           qsTr("Serial Port Configuration")
                        font.family:    ScreenTools.demiboldFontFamily
                    }

                    GridLayout {
                        columns: 2
                        columnSpacing: _margins
                        rowSpacing: _margins / 2

                        QGCLabel { text: qsTr("Serial Port:") }
                        QGCComboBox {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 15
                            model: QGroundControl.linkManager.serialPortStrings
                            currentIndex: {
                                var index = find(_utgSettings.serialPort.rawValue)
                                return index === -1 ? 0 : index
                            }
                            onActivated: {
                                if (index != -1) {
                                    _utgSettings.serialPort.rawValue = QGroundControl.linkManager.serialPorts[index]
                                }
                            }
                        }

                        QGCLabel { text: qsTr("Baud Rate:") }
                        FactComboBox {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 10
                            fact: _utgSettings.baudRate
                        }

                        QGCLabel { text: qsTr("Data Bits:") }
                        FactComboBox {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 8
                            fact: _utgSettings.dataBits
                        }

                        QGCLabel { text: qsTr("Stop Bits:") }
                        FactComboBox {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 8
                            fact: _utgSettings.stopBits
                        }

                        QGCLabel { text: qsTr("Parity:") }
                        FactComboBox {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 8
                            fact: _utgSettings.parity
                        }

                        QGCLabel { text: qsTr("Flow Control:") }
                        FactCheckBox {
                            fact: _utgSettings.flowControl
                        }
                    }
                }
            }

            Rectangle {
                width:  _panelWidth
                height: measurementColumn.height + (_margins * 2)
                color:  qgcPal.windowShade
                visible: _utgSettings && _utgSettings.enabled ? _utgSettings.enabled.rawValue : false

                Column {
                    id:                 measurementColumn
                    anchors.margins:    _margins
                    anchors.top:        parent.top
                    anchors.left:       parent.left
                    anchors.right:      parent.right
                    spacing:            _margins

                    QGCLabel {
                        text:           qsTr("Measurement Settings")
                        font.family:    ScreenTools.demiboldFontFamily
                    }

                    GridLayout {
                        columns: 2
                        columnSpacing: _margins
                        rowSpacing: _margins / 2

                        QGCLabel { text: qsTr("Unit:") }
                        FactComboBox {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 10
                            fact: _utgSettings.measurementUnit
                        }

                        QGCLabel { text: qsTr("Mode:") }
                        FactComboBox {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 15
                            fact: _utgSettings.measurementMode
                        }

                        QGCLabel { text: qsTr("Sound Velocity (m/s):") }
                        FactTextField {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 10
                            fact: _utgSettings.soundVelocity
                        }

                        QGCLabel { text: qsTr("Zero Offset:") }
                        FactTextField {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 10
                            fact: _utgSettings.zeroOffset
                        }

                        QGCLabel { text: qsTr("Gain (%):") }
                        FactTextField {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 8
                            fact: _utgSettings.gain
                        }

                        QGCLabel { text: qsTr("Threshold (%):") }
                        FactTextField {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 8
                            fact: _utgSettings.threshold
                        }

                        QGCLabel { text: qsTr("Pulse Width (ns):") }
                        FactTextField {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 8
                            fact: _utgSettings.pulseWidth
                        }

                        QGCLabel { text: qsTr("Frequency (MHz):") }
                        FactTextField {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 8
                            fact: _utgSettings.frequency
                        }

                        QGCLabel { text: qsTr("Display Precision:") }
                        FactTextField {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 6
                            fact: _utgSettings.displayPrecision
                        }
                    }
                }
            }

            Rectangle {
                width:  _panelWidth
                height: materialColumn.height + (_margins * 2)
                color:  qgcPal.windowShade
                visible: _utgSettings && _utgSettings.enabled ? _utgSettings.enabled.rawValue : false

                Column {
                    id:                 materialColumn
                    anchors.margins:    _margins
                    anchors.top:        parent.top
                    anchors.left:       parent.left
                    anchors.right:      parent.right
                    spacing:            _margins

                    QGCLabel {
                        text:           qsTr("Material & Calibration")
                        font.family:    ScreenTools.demiboldFontFamily
                    }

                    GridLayout {
                        columns: 2
                        columnSpacing: _margins
                        rowSpacing: _margins / 2

                        QGCLabel { text: qsTr("Material Type:") }
                        FactComboBox {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 12
                            fact: _utgSettings.materialType
                        }

                        QGCLabel { text: qsTr("Calibration Mode:") }
                        FactComboBox {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 12
                            fact: _utgSettings.calibrationMode
                        }

                        QGCLabel { text: qsTr("Calibration Value:") }
                        FactTextField {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 10
                            fact: _utgSettings.calibrationValue
                        }

                        QGCLabel { text: qsTr("Temperature Compensation:") }
                        FactCheckBox {
                            fact: _utgSettings.temperatureCompensation
                        }
                    }
                }
            }

            Rectangle {
                width:  _panelWidth
                height: alertColumn.height + (_margins * 2)
                color:  qgcPal.windowShade
                visible: _utgSettings && _utgSettings.enabled ? _utgSettings.enabled.rawValue : false

                Column {
                    id:                 alertColumn
                    anchors.margins:    _margins
                    anchors.top:        parent.top
                    anchors.left:       parent.left
                    anchors.right:      parent.right
                    spacing:            _margins

                    QGCLabel {
                        text:           qsTr("Alerts & Logging")
                        font.family:    ScreenTools.demiboldFontFamily
                    }

                    GridLayout {
                        columns: 2
                        columnSpacing: _margins
                        rowSpacing: _margins / 2

                        QGCLabel { text: qsTr("Enable Alerts:") }
                        FactCheckBox {
                            fact: _utgSettings.alertEnabled
                        }

                        QGCLabel { text: qsTr("Alert Sound:") }
                        FactCheckBox {
                            fact: _utgSettings.alertSound
                            enabled: _utgSettings.alertEnabled.rawValue
                        }

                        QGCLabel { text: qsTr("Min Thickness Alert:") }
                        FactTextField {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 10
                            fact: _utgSettings.minThicknessAlert
                            enabled: _utgSettings.alertEnabled.rawValue
                        }

                        QGCLabel { text: qsTr("Max Thickness Alert:") }
                        FactTextField {
                            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 10
                            fact: _utgSettings.maxThicknessAlert
                            enabled: _utgSettings.alertEnabled.rawValue
                        }

                        QGCLabel { text: qsTr("Log Measurements:") }
                        FactCheckBox {
                            fact: _utgSettings.logMeasurements
                        }

                        QGCLabel { text: qsTr("Auto Save:") }
                        FactCheckBox {
                            fact: _utgSettings.autoSaveMeasurements
                            enabled: _utgSettings.logMeasurements.rawValue
                        }
                    }
                }
            }

            // Fallback message when UTG settings are not available
            Rectangle {
                width:  _panelWidth
                height: fallbackColumn.height + (_margins * 2)
                color:  qgcPal.windowShade
                visible: _utgSettings === null

                Column {
                    id:                 fallbackColumn
                    anchors.margins:    _margins
                    anchors.top:        parent.top
                    anchors.left:       parent.left
                    anchors.right:      parent.right
                    spacing:            _margins

                    QGCLabel {
                        text:           qsTr("UTG Settings Not Available")
                        font.family:    ScreenTools.demiboldFontFamily
                        color:          qgcPal.warningText
                    }

                    QGCLabel {
                        text:           qsTr("UTG settings could not be loaded. This may indicate a build configuration issue.")
                        wrapMode:       Text.WordWrap
                        width:          parent.width
                    }
                }
            }
        }
    }
}
