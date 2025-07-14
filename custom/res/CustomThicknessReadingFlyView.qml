import QtQuick                  2.11
import QtQuick.Controls         2.4
import QtQuick.Layouts          1.11
import QtQuick.Dialogs          1.2

import QGroundControl               1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0
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
    property bool _compactMode:     false

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
        width: _compactMode ? Math.min(parent.width * 0.25, 250) : Math.min(parent.width * 0.4, 400)
        height: _compactMode ? Math.min(parent.height * 0.3, 180) : Math.min(parent.height * 0.6, 350)
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
            anchors.margins: ScreenTools.defaultFontPixelWidth * 0.8
            spacing: ScreenTools.defaultFontPixelHeight * 0.3

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

                Item { width: ScreenTools.defaultFontPixelWidth; height: 1 } // Spacer

                QGCButton {
                    text: _compactMode ? "+" : "-"
                    width: ScreenTools.defaultFontPixelHeight * 1.5
                    height: ScreenTools.defaultFontPixelHeight * 1.5
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: _compactMode = !_compactMode
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
                        if (!_utgSettings) return qsTr("No Settings")
                        var unit = _utgSettings.measurementUnit ? _utgSettings.measurementUnit.enumStringValue : "mm"
                        var precision = _utgSettings.displayPrecision ? _utgSettings.displayPrecision.rawValue : 2
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

            // Control buttons - Compact grid layout
            GridLayout {
                width: parent.width
                columns: _compactMode ? 3 : 2
                columnSpacing: ScreenTools.defaultFontPixelWidth * 0.5
                rowSpacing: ScreenTools.defaultFontPixelHeight * 0.3

                QGCButton {
                    text: _measuring ? qsTr("Stop") : qsTr("Measure")
                    enabled: _utgConnected
                    Layout.fillWidth: true
                    Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.8
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
                    Layout.fillWidth: true
                    Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.8
                    onClicked: _utgManager.startContinuousMeasurement()
                }

                QGCButton {
                    text: _compactMode ? qsTr("Set") : qsTr("Settings")
                    Layout.fillWidth: true
                    Layout.columnSpan: _compactMode ? 1 : 2
                    Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.8
                    onClicked: settingsDialog.open()
                }
            }

            // Quick settings - Compact layout
            GridLayout {
                width: parent.width
                columns: 4
                columnSpacing: ScreenTools.defaultFontPixelWidth * 0.3
                rowSpacing: ScreenTools.defaultFontPixelHeight * 0.2
                visible: _utgConnected && !_compactMode

                QGCLabel {
                    text: qsTr("Gain:")
                    font.pointSize: ScreenTools.smallFontPointSize
                }

                QGCTextField {
                    Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 3.5
                    text: _utgSettings ? _utgSettings.gain.rawValue : "50"
                    inputMethodHints: Qt.ImhDigitsOnly
                    onEditingFinished: {
                        if (_utgManager && _utgSettings && _utgSettings.gain && text !== "") {
                            _utgManager.setGain(parseInt(text))
                            _utgSettings.gain.rawValue = parseInt(text)
                        }
                    }
                }

                QGCLabel {
                    text: qsTr("Velocity:")
                    font.pointSize: ScreenTools.smallFontPointSize
                }

                QGCTextField {
                    Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 5
                    text: _utgSettings ? _utgSettings.soundVelocity.rawValue : "5920"
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onEditingFinished: {
                        if (_utgManager && _utgSettings && _utgSettings.soundVelocity && text !== "") {
                            _utgManager.setSoundVelocity(parseFloat(text))
                            _utgSettings.soundVelocity.rawValue = parseFloat(text)
                        }
                    }
                }
            }
        }
    }

    // UTG Settings Popup Background
    Rectangle {
        id: settingsDialogBackground
        visible: settingsDialog.visible
        anchors.fill: parent
        color: "black"
        opacity: 0.5
        z: 999

        MouseArea {
            anchors.fill: parent
            onClicked: settingsDialog.visible = false
        }
    }

    // UTG Settings Popup
    Rectangle {
        id: settingsDialog
        visible: false
        anchors.centerIn: parent
        width: Math.min(parent.width * 0.8, ScreenTools.defaultFontPixelWidth * 50)
        height: Math.min(parent.height * 0.8, ScreenTools.defaultFontPixelHeight * 40)
        color: qgcPal.window
        border.color: qgcPal.text
        border.width: 1
        radius: ScreenTools.defaultFontPixelHeight * 0.5
        z: 1000

        function open() { visible = true }
        function closeDialog() { visible = false }

        function applySettings() {
            // Apply settings
            if (_utgManager && _utgConnected && _utgSettings) {
                if (_utgSettings.gain) _utgManager.setGain(_utgSettings.gain.rawValue)
                if (_utgSettings.soundVelocity) _utgManager.setSoundVelocity(_utgSettings.soundVelocity.rawValue)
                if (_utgSettings.zeroOffset) _utgManager.setZeroOffset(_utgSettings.zeroOffset.rawValue)
                if (_utgSettings.threshold) _utgManager.setThreshold(_utgSettings.threshold.rawValue)
                if (_utgSettings.measurementUnit) _utgManager.setMeasurementUnit(_utgSettings.measurementUnit.rawValue)
                if (_utgSettings.measurementMode) _utgManager.setMeasurementMode(_utgSettings.measurementMode.rawValue)
            }
            settingsDialog.visible = false
        }

        Column {
            anchors.fill: parent
            anchors.margins: ScreenTools.defaultFontPixelHeight

            // Title
            Column {
                id: titleSection
                width: parent.width

                QGCLabel {
                    text: qsTr("UTG Settings")
                    font.family: ScreenTools.demiboldFontFamily
                    font.pointSize: ScreenTools.mediumFontPointSize
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: qgcPal.text
                }
            }

            // Content
            ScrollView {
                width: parent.width
                height: parent.height - titleSection.height - buttonSection.height
                clip: true

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

            // Buttons
            Rectangle {
                id: buttonSection
                width: parent.width
                height: ScreenTools.defaultFontPixelHeight * 3
                color: "transparent"

                Row {
                    anchors.centerIn: parent
                    spacing: ScreenTools.defaultFontPixelWidth

                    QGCButton {
                        text: qsTr("OK")
                        onClicked: applySettings()
                    }

                    QGCButton {
                        text: qsTr("Cancel")
                        onClicked: settingsDialog.visible = false
                    }
                }
            }
        }
    }
}
