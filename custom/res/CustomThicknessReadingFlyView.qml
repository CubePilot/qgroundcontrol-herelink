import QtQuick                  2.11
import QtQuick.Controls         2.4
import QtQuick.Layouts          1.11
import QtQuick.Dialogs          1.3
import Qt.labs.platform         1.1
import Qt.labs.settings         1.0

import QGroundControl               1.0
import QGroundControl.Palette       1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0

Item {
    property var _activeVehicle:    QGroundControl.multiVehicleManager.activeVehicle
    property var _utgManager:       _activeVehicle ? _activeVehicle.utgManager : null
    property var _utgSettings:      QGroundControl.settingsManager.utgSettings
    property bool _utgEnabled:      _utgSettings ? _utgSettings.enabled.rawValue : false
    property bool _utgConnected:    _utgManager ? _utgManager.connected : false
    property real _currentThickness: _utgManager ? _utgManager.currentThickness : 0.0
    property bool _measuring:       _utgManager ? _utgManager.measuring : false

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

    visible: _utgEnabled

    Component.onCompleted: {
    }

    Rectangle {
        id: mainThicknessRectangle

        x: 140
        y: 100
        color: qgcPal.window
        radius: 12
        width: 320
        height: 280
        clip: true

        border.color: _utgConnected ? qgcPal.colorGreen : (_utgEnabled ? qgcPal.colorOrange : qgcPal.colorGrey)
        border.width: 3

        MouseArea {
            anchors.fill: parent
            drag.target: mainThicknessRectangle
            drag.axis: Drag.XAndYAxis
        }

        Column {
            anchors.fill: parent
            anchors.margins: ScreenTools.defaultFontPixelWidth
            spacing: ScreenTools.defaultFontPixelHeight * 0.5

            // Header
            Row {
                width: parent.width
                spacing: ScreenTools.defaultFontPixelWidth

                QGCLabel {
                    text: qsTr("UTG Thickness")
                    font.family: ScreenTools.demiboldFontFamily
                    font.pointSize: ScreenTools.mediumFontPointSize
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    width: 12
                    height: 12
                    radius: 6
                    color: _utgConnected ? qgcPal.colorGreen : (_utgEnabled ? qgcPal.colorOrange : qgcPal.colorGrey)
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            // Main reading display
            Rectangle {
                width: parent.width
                height: 80
                color: qgcPal.windowShade
                radius: 6

                QGCLabel {
                    id: readingValue
                    anchors.centerIn: parent
                    text: {
                        if (!_utgEnabled) return qsTr("Disabled")
                        if (!_utgConnected) return qsTr("Disconnected")
                        var unit = _utgSettings.measurementUnit.enumStringValue
                        var precision = _utgSettings.displayPrecision.rawValue
                        return _currentThickness.toFixed(precision) + " " + unit
                    }
                    font.pointSize: ScreenTools.largeFontPointSize
                    font.bold: true
                    color: _utgConnected ? qgcPal.text : qgcPal.colorGrey
                }
            }

            // Status and controls
            Row {
                width: parent.width
                spacing: ScreenTools.defaultFontPixelWidth

                QGCLabel {
                    text: _utgManager ? _utgManager.statusText : qsTr("No UTG")
                    font.pointSize: ScreenTools.smallFontPointSize
                    color: qgcPal.colorGrey
                    anchors.verticalCenter: parent.verticalCenter
                }

                Item { Layout.fillWidth: true; height: 1 } // Spacer
            }

            // Control buttons
            Row {
                width: parent.width
                spacing: ScreenTools.defaultFontPixelWidth * 0.5

                QGCButton {
                    text: _measuring ? qsTr("Stop") : qsTr("Measure")
                    enabled: _utgConnected
                    onClicked: {
                        if (_measuring) {
                            _utgManager.stopMeasurement()
                        } else {
                            _utgManager.startSingleMeasurement()
                        }
                    }
                }

                QGCButton {
                    text: qsTr("Continuous")
                    enabled: _utgConnected && !_measuring
                    onClicked: _utgManager.startContinuousMeasurement()
                }

                QGCButton {
                    text: qsTr("Settings")
                    onClicked: settingsDialog.open()
                }
            }

            // Quick settings
            Row {
                width: parent.width
                spacing: ScreenTools.defaultFontPixelWidth * 0.5
                visible: _utgConnected

                QGCLabel {
                    text: qsTr("Gain:")
                    font.pointSize: ScreenTools.smallFontPointSize
                    anchors.verticalCenter: parent.verticalCenter
                }

                QGCTextField {
                    width: ScreenTools.defaultFontPixelWidth * 4
                    text: _utgSettings ? _utgSettings.gain.rawValue : "50"
                    inputMethodHints: Qt.ImhDigitsOnly
                    onEditingFinished: {
                        if (_utgManager && text !== "") {
                            _utgManager.setGain(parseInt(text))
                            _utgSettings.gain.rawValue = parseInt(text)
                        }
                    }
                }

                QGCLabel {
                    text: qsTr("Velocity:")
                    font.pointSize: ScreenTools.smallFontPointSize
                    anchors.verticalCenter: parent.verticalCenter
                }

                QGCTextField {
                    width: ScreenTools.defaultFontPixelWidth * 6
                    text: _utgSettings ? _utgSettings.soundVelocity.rawValue : "5920"
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onEditingFinished: {
                        if (_utgManager && text !== "") {
                            _utgManager.setSoundVelocity(parseFloat(text))
                            _utgSettings.soundVelocity.rawValue = parseFloat(text)
                        }
                    }
                }
            }
        }
    }

    // UTG Settings Dialog
    Dialog {
        id: settingsDialog
        title: qsTr("UTG Settings")
        standardButtons: StandardButton.Ok | StandardButton.Cancel
        modal: true

        width: Math.min(parent.width * 0.8, ScreenTools.defaultFontPixelWidth * 50)
        height: Math.min(parent.height * 0.8, ScreenTools.defaultFontPixelHeight * 40)

        onAccepted: {
            // Apply settings
            if (_utgManager && _utgConnected) {
                _utgManager.setGain(_utgSettings.gain.rawValue)
                _utgManager.setSoundVelocity(_utgSettings.soundVelocity.rawValue)
                _utgManager.setZeroOffset(_utgSettings.zeroOffset.rawValue)
                _utgManager.setThreshold(_utgSettings.threshold.rawValue)
                _utgManager.setMeasurementUnit(_utgSettings.measurementUnit.rawValue)
                _utgManager.setMeasurementMode(_utgSettings.measurementMode.rawValue)
            }
        }

        ScrollView {
            anchors.fill: parent

            Column {
                width: parent.width
                spacing: ScreenTools.defaultFontPixelHeight

                GroupBox {
                    title: qsTr("Connection")
                    width: parent.width

                    GridLayout {
                        columns: 2
                        width: parent.width

                        QGCLabel { text: qsTr("Status:") }
                        QGCLabel {
                            text: _utgManager ? _utgManager.statusText : qsTr("No UTG")
                            color: _utgConnected ? qgcPal.colorGreen : qgcPal.colorOrange
                        }

                        QGCLabel { text: qsTr("Serial Port:") }
                        QGCComboBox {
                            Layout.fillWidth: true
                            model: QGroundControl.linkManager.serialPortStrings
                            currentIndex: {
                                if (!_utgSettings) return 0
                                var index = find(_utgSettings.serialPort.rawValue)
                                return index === -1 ? 0 : index
                            }
                            onActivated: {
                                if (index != -1 && _utgSettings) {
                                    _utgSettings.serialPort.rawValue = QGroundControl.linkManager.serialPorts[index]
                                }
                            }
                        }

                        QGCButton {
                            text: _utgConnected ? qsTr("Disconnect") : qsTr("Connect")
                            Layout.columnSpan: 2
                            onClicked: {
                                if (_utgManager) {
                                    if (_utgConnected) {
                                        _utgManager.disconnectFromUTG()
                                    } else {
                                        _utgManager.connectToUTG()
                                    }
                                }
                            }
                        }
                    }
                }

                GroupBox {
                    title: qsTr("Measurement")
                    width: parent.width
                    enabled: _utgConnected

                    GridLayout {
                        columns: 2
                        width: parent.width

                        QGCLabel { text: qsTr("Unit:") }
                        FactComboBox {
                            Layout.fillWidth: true
                            fact: _utgSettings ? _utgSettings.measurementUnit : null
                        }

                        QGCLabel { text: qsTr("Mode:") }
                        FactComboBox {
                            Layout.fillWidth: true
                            fact: _utgSettings ? _utgSettings.measurementMode : null
                        }

                        QGCLabel { text: qsTr("Sound Velocity:") }
                        FactTextField {
                            Layout.fillWidth: true
                            fact: _utgSettings ? _utgSettings.soundVelocity : null
                        }

                        QGCLabel { text: qsTr("Zero Offset:") }
                        FactTextField {
                            Layout.fillWidth: true
                            fact: _utgSettings ? _utgSettings.zeroOffset : null
                        }
                    }
                }

                GroupBox {
                    title: qsTr("Signal Parameters")
                    width: parent.width
                    enabled: _utgConnected

                    GridLayout {
                        columns: 2
                        width: parent.width

                        QGCLabel { text: qsTr("Gain (%):") }
                        FactTextField {
                            Layout.fillWidth: true
                            fact: _utgSettings ? _utgSettings.gain : null
                        }

                        QGCLabel { text: qsTr("Threshold (%):") }
                        FactTextField {
                            Layout.fillWidth: true
                            fact: _utgSettings ? _utgSettings.threshold : null
                        }

                        QGCLabel { text: qsTr("Frequency (MHz):") }
                        FactTextField {
                            Layout.fillWidth: true
                            fact: _utgSettings ? _utgSettings.frequency : null
                        }

                        QGCLabel { text: qsTr("Pulse Width (ns):") }
                        FactTextField {
                            Layout.fillWidth: true
                            fact: _utgSettings ? _utgSettings.pulseWidth : null
                        }
                    }
                }

                GroupBox {
                    title: qsTr("Calibration")
                    width: parent.width
                    enabled: _utgConnected

                    Column {
                        width: parent.width
                        spacing: ScreenTools.defaultFontPixelHeight * 0.5

                        GridLayout {
                            columns: 2
                            width: parent.width

                            QGCLabel { text: qsTr("Material:") }
                            FactComboBox {
                                Layout.fillWidth: true
                                fact: _utgSettings ? _utgSettings.materialType : null
                            }

                            QGCLabel { text: qsTr("Calibration Mode:") }
                            FactComboBox {
                                Layout.fillWidth: true
                                fact: _utgSettings ? _utgSettings.calibrationMode : null
                            }

                            QGCLabel { text: qsTr("Reference Value:") }
                            FactTextField {
                                Layout.fillWidth: true
                                fact: _utgSettings ? _utgSettings.calibrationValue : null
                            }
                        }

                        Row {
                            spacing: ScreenTools.defaultFontPixelWidth

                            QGCButton {
                                text: qsTr("Zero Calibration")
                                onClicked: {
                                    if (_utgManager) {
                                        _utgManager.performZeroCalibration()
                                    }
                                }
                            }

                            QGCButton {
                                text: qsTr("Velocity Calibration")
                                onClicked: {
                                    if (_utgManager && _utgSettings) {
                                        _utgManager.performVelocityCalibration(_utgSettings.calibrationValue.rawValue)
                                    }
                                }
                            }

                            QGCButton {
                                text: qsTr("Reset UTG")
                                onClicked: {
                                    if (_utgManager) {
                                        _utgManager.resetUTG()
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
