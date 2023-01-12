import QtQuick              2.11
import QtQuick.Layouts      1.11
import QtGraphicalEffects   1.0

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Controllers   1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Palette       1.0
import QGroundControl.FlightMap     1.0

Item {
    id: _root

    AscentAlertsController {id: controller}

    property var _activeVehicle:                QGroundControl.multiVehicleManager.activeVehicle
    property bool communicationLost:            activeVehicle ? activeVehicle.connectionLost : false
    property var _flightMode:                   _activeVehicle ? _activeVehicle.flightMode : qsTr("No Active Vehicle")
    property var _lastFlightMode:               ""
    property real _margins:                     ScreenTools.defaultFontPixelWidth
    property bool _isMinimized:                 true
    property bool _noActionNeeded:              true

    //Alert Flags
    property bool _batteryFailsafe:             _activeVehicle ? controller.batteryFailsafe : false
    property bool _criticalBattery:             _activeVehicle ? controller.critBatteryFailsafe : false
    property bool _lossOfLink:                  communicationLost
    property bool _gpsGlitch:                   _activeVehicle ? controller.gpsFailsafe : false
    property bool _ekfFailsafe:                 _activeVehicle ? controller.ekfFailsafe : false
    property bool _radioFailsafe:               _activeVehicle ? controller.radioFailsafe : false

    property bool _recovered:                   false
    property int  _recoveredFrom:               AscentAlerts.Alerts.Normal
    property int  _gcsFailsafe:                 _activeVehicle ? _activeVehicle.gcsFS : 0
    property int  _thrFailsafe:                 _activeVehicle ? _activeVehicle.thrFS : 0
    property bool _blink:                       false
    property bool _blinkFlag:                   false
    property bool _messageAvailable:            false
    property var _accentColor:                  _blink ? (_blinkFlag ? "#ffffff" : _severityColor ) : _severityColor
    property int _severity:                     AscentAlerts.Severity.Normal
    property var _severityColor:                _severity === AscentAlerts.Severity.Warning ? "#ffe600" : (_severity === AscentAlerts.Severity.Critical ? qgcPal.colorRed : (_severity === AscentAlerts.Severity.Recovered ? qgcPal.colorGreen : qgcPal.text))
    
    property int _debugcounter:                0

    enum Severity {
        Normal,
        Recovered,
        Warning,
        Critical
    }

    enum Alerts {
        Normal,
        BatteryFailsafe,
        LossOfLink,
        GpsGlitch,
        EkfFailsafe,
        RadioFailsafe
    }

    state: "minimized"
    states: [
        State{name: "minimized"; AnchorChanges{target: ascentAlerts; anchors.bottom: minimizedAlertsSpace.bottom}},
        State{name: "open"     ; AnchorChanges{target: ascentAlerts; anchors.bottom: minimizedAlertsSpace.top}}
    ]
    transitions: Transition {
        // smoothly reanchor myRect and move into new position
        AnchorAnimation { duration: 250 }
    }
    
    Timer {
        id: debugTimer
        interval: 8000
        repeat: true
        running: false
        onTriggered: {
            if(_debugcounter == 0){
                _batteryFailsafe = true;
            } else if(_debugcounter == 1) {
                _criticalBattery = true;
            } else if(_debugcounter == 2) {
                _gpsGlitch = true;
            } else if(_debugcounter == 3) {
                _ekfFailsafe = true
            }  else if(_debugcounter == 4) {
                _lossOfLink = true
            } else if(_debugcounter == 5){
                _lossOfLink = false;
            } else if(_debugcounter == 6) {
                _ekfFailsafe = false;
            } else if(_debugcounter == 7) {
                _gpsGlitch = false
            } 

            _debugcounter++
        }
    }

    Timer {id: blinkTimer
        interval: 600
        repeat: true
        running: _blink
        onTriggered: {_blinkFlag = !_blinkFlag}
    }

    function nextAlert() {
        _recovered = false;
        if(_lossOfLink){
            _blink = true;
            _severity = AscentAlerts.Severity.Critical;
        }
        else if(_ekfFailsafe){
            _blink = true;
            _severity = AscentAlerts.Severity.Critical;
        }
        else if(_radioFailsafe){
            _blink = true;
            _severity = AscentAlerts.Severity.Critical;
        }
        else if(_gpsGlitch){
            _blink = true;
            _severity = AscentAlerts.Severity.Warning;
        }
        else if(_criticalBattery){
            _blink = true;
            _severity = AscentAlerts.Severity.Critical;
        }
        else if(_batteryFailsafe){
            _blink = true;
            _severity = AscentAlerts.Severity.Warning;
        }
        else{
            _severity = AscentAlerts.Severity.Normal;
            _noActionNeeded = true; 
            _messageAvailable = false;
            if(ascentAlerts.state == "open"){ascentAlerts.state ="minimized"}
        }
    }

    function resetBatteryFailsafes(){
        _batteryFailsafe = false;
        _criticalBattery = false;
        _severity = AscentAlerts.Severity.Normal;
        _noActionNeeded = true; 
        _messageAvailable = false;
        if(ascentAlerts.state == "open"){ascentAlerts.state ="minimized"}
    }

    Timer {id: recoveryTimer
        interval: 3000
        onTriggered: {
            nextAlert()
        }
    }

    on_BatteryFailsafeChanged: {
        if(_batteryFailsafe){
            _noActionNeeded = false;
            _messageAvailable = true;
            _blink = true;
            _severity = AscentAlerts.Severity.Warning;
        }
    }

    on_CriticalBatteryChanged: {
        if(_batteryFailsafe && _criticalBattery){
            _noActionNeeded = false
            _messageAvailable = true;
            _blink = true;
            _severity = AscentAlerts.Severity.Critical;
        }
    }

    on_LossOfLinkChanged: {
        if(_lossOfLink){
            if(!_activeVehicle.flying && !_activeVehicle.landing && (_batteryFailsafe || _criticalBattery)){
                resetBatteryFailsafes();
            }
            _noActionNeeded = false;
            _messageAvailable = true;
            _blink = true;
            _severity = AscentAlerts.Severity.Critical;
        }
        else{
            _messageAvailable = true;
            _blink = false;
            if(!_activeVehicle){
                _severity = AscentAlerts.Severity.Critical;
                _recovered = true;
                _recoveredFrom = AscentAlerts.Alerts.LossOfLink;
               recoveryTimer.restart();
            }
            else{
                _severity = AscentAlerts.Severity.Recovered;
                _recovered = true;
                _recoveredFrom = AscentAlerts.Alerts.LossOfLink;
                recoveryTimer.restart()
            }
        }
    }

    on_RadioFailsafeChanged: {
        if(_radioFailsafe){
            _noActionNeeded = false
            _messageAvailable = true;
            _blink = true;
            _severity = AscentAlerts.Severity.Critical;
        }
        else{
            _messageAvailable = true;
            _blink = false;
            _severity = AscentAlerts.Severity.Recovered;
            _recovered = true;
            _recoveredFrom = AscentAlerts.Alerts.RadioFailsafe
            recoveryTimer.restart()
        }
    }

    on_GpsGlitchChanged: {
        if(_gpsGlitch){
            _noActionNeeded = false
            _messageAvailable = true;
            _blink = true;
            _severity = AscentAlerts.Severity.Warning;
        }
        else{
            _messageAvailable = true;
            _blink = false;
            _severity = AscentAlerts.Severity.Recovered;
            _recovered = true;
            _recoveredFrom = AscentAlerts.Alerts.GpsGlitch
            recoveryTimer.restart()
        }
    }

    on_EkfFailsafeChanged: {
        if(_ekfFailsafe){
            _noActionNeeded = false
            _messageAvailable = true;
            _blink = true;
            _severity = AscentAlerts.Severity.Critical;
        }
        else{
            _messageAvailable = true;
            _blink = false;
            _severity = AscentAlerts.Severity.Recovered;
            _recovered = true;
            _recoveredFrom = AscentAlerts.Alerts.EkfFailsafe
            recoveryTimer.restart()
        }
    }

    on_FlightModeChanged: {
        if(_activeVehicle){
            if(_flightMode != "RTL" && _flightMode != "Land") {
                _lastFlightMode = _flightMode
            }
        }
    }

    on_MessageAvailableChanged: {
        if(ascentAlerts.state == "minimized" && _messageAvailable){
            ascentAlerts.state ="open"
        }
    }

    Rectangle {
        id:                 alertWindow
        anchors.fill:       parent
        color:              qgcPal.brandingPurple
        border.color:      _accentColor
        border.width:       3
        opacity:            0.9
    }

    /*
            FAILSAFE VIEWING PRIORITY
            -------------------------
            1) Loss of Link
            2) EKF Failsafe
            3) Radio Failsafe
            4) GPS Glitch
            5) Battery levels
    */

    ColumnLayout { id: batteryFailsafeView
        anchors.fill: alertWindow
        anchors.margins: _margins
        visible: (_batteryFailsafe || _criticalBattery) && !_lossOfLink && !_gpsGlitch && !_ekfFailsafe && !_radioFailsafe && !_recovered
        QGCLabel{
            text: _criticalBattery ? qsTr("CRITICALLY LOW\nBATTERY") : qsTr("LOW BATTERY")
            color:       _accentColor
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: ScreenTools.largeFontPointSize
            font.family: ScreenTools.demiboldFontFamily
            font.bold: true
            Layout.fillWidth: true
        }
        QGCLabel{
            text: _activeVehicle ? (_activeVehicle.flightMode == "RTL" ? qsTr("Vehicle Returning Home") : 
                                   (_activeVehicle.flightMode == "Land" ? qsTr("Vehicle Landing") : 
                                   (_criticalBattery ? qsTr("Land immediately") :  
                                    qsTr("Return Home")))) : ""
            font.underline: true
            font.pointSize: ScreenTools.mediumFontPointSize
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }
        SliderSwitch{
            confirmText:    qsTr("Slide To Cancel")
            Layout.preferredWidth: parent.width * 0.9
            Layout.alignment: Qt.AlignHCenter
            visible: _activeVehicle ? (_activeVehicle.flightMode == "RTL" || _activeVehicle.flightMode == "Land") : false
            onAccept: {
                if(_activeVehicle){_activeVehicle.flightMode = _lastFlightMode}
                if(ascentAlerts.state == "open"){ascentAlerts.state ="minimized"}
                _messageAvailable = false;
            }
        }
    }

    ColumnLayout { id: lossOfLinkView
        anchors.fill: alertWindow
        anchors.margins: _margins
        visible: _lossOfLink && !_recovered
        QGCLabel{
            text: qsTr("CONNECTION LOST")
            color:       _accentColor
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: ScreenTools.largeFontPointSize
            font.family: ScreenTools.demiboldFontFamily
            font.bold: true
            Layout.fillWidth: true
        }
        QGCLabel{
            text: (!_activeVehicle.flying && !_activeVehicle.landing) ?  qsTr("Vehicle on Ground") :
                 (_gcsFailsafe == 5 ? qsTr("Vehicle Landing") : 
                 (_gcsFailsafe == 4 ? qsTr("Vehicle Either Landing or SmartRTLing") : 
                 (_gcsFailsafe == 3 ? qsTr("Vehicle Returning Home") : 
                 (_gcsFailsafe == 2 ? qsTr("Vehicle Returning Home") : 
                 (_gcsFailsafe == 1 ? qsTr("Vehicle Returning Home") : 
                 (_gcsFailsafe == 0 ? qsTr("No Failsafe set") : ""))))))
            horizontalAlignment: Text.AlignHCenter
            font.family: ScreenTools.demiboldFontFamily
            font.bold: true
            Layout.fillWidth: true
        }
    }

    ColumnLayout { id: radioFailsafeView
        anchors.fill: alertWindow
        anchors.margins: _margins
        visible: _radioFailsafe && !_lossOfLink && !_ekfFailsafe && !_recovered
        QGCLabel{
            text: qsTr("RADIO LINK LOST")
            color:       _accentColor
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: ScreenTools.largeFontPointSize
            font.family: ScreenTools.demiboldFontFamily
            font.bold: true
            Layout.fillWidth: true
        }
        QGCLabel{
            text: (!_activeVehicle.flying && !_activeVehicle.landing) ?  qsTr("Vehicle on Ground") :
                 (_thrFailsafe == 5 ? qsTr("Vehicle Either\nLanding or SmartRTL") : 
                 (_thrFailsafe == 4 ? qsTr("Vehicle Either RTL\nor SmartRTL") : 
                 (_thrFailsafe == 3 ? qsTr("Vehicle Landing") : 
                 (_thrFailsafe == 2 ? qsTr("Vehicle Continuing\nMission") : 
                 (_thrFailsafe == 1 ? qsTr("Vehicle Returning Home") : 
                 (_thrFailsafe == 0 ? qsTr("No Failsafe set") : ""))))))
            horizontalAlignment: Text.AlignHCenter
            font.family: ScreenTools.demiboldFontFamily
            font.bold: true
            Layout.fillWidth: true
        }
    }

    ColumnLayout { id: gpsGlitchView
        anchors.fill: alertWindow
        anchors.margins: _margins
        visible: _gpsGlitch && !_ekfFailsafe && !_lossOfLink && !_radioFailsafe && !_recovered
        QGCLabel{
            text: qsTr("GPS FAILING")
            color:       _accentColor
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: ScreenTools.largeFontPointSize
            font.family: ScreenTools.demiboldFontFamily
            font.bold: true
            Layout.fillWidth: true
        }
        QGCLabel{
            text: qsTr("Check Overhead Obstructions")
            font.underline: true
            font.pointSize: ScreenTools.mediumFontPointSize * 0.9
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }
        QGCLabel{
            text: qsTr("Consider Manual Return")
            font.pointSize: ScreenTools.mediumFontPointSize
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }
        SliderSwitch{
            confirmText:    qsTr("Manual Control")
            Layout.preferredWidth: parent.width * 0.9
            Layout.alignment: Qt.AlignHCenter
            visible: _activeVehicle ? _activeVehicle.flightMode != "Altitude Hold" : false
            onAccept: {
                if(_activeVehicle){_activeVehicle.flightMode = "Altitude Hold"}
                if(ascentAlerts.state == "open"){ascentAlerts.state ="minimized"}
                _messageAvailable = false;
            }
        }
    }

    ColumnLayout { id: ekfFailsafeView
        anchors.fill: alertWindow
        anchors.margins: _margins
        visible: _ekfFailsafe && !_lossOfLink && !_recovered
        QGCLabel{
            text: qsTr("VEHICLE POSITION\nUNCERTAIN")
            color:       _accentColor
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: ScreenTools.largeFontPointSize
            font.family: ScreenTools.demiboldFontFamily
            font.bold: true
            Layout.fillWidth: true
        }
        QGCLabel{
            text: qsTr("Pilot Take Control")
            font.underline: true
            font.pointSize: ScreenTools.mediumFontPointSize
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }
        Item{Layout.fillHeight: true}
        QGCLabel{
            text: _activeVehicle ? (_activeVehicle.flightMode == "Altitude Hold" ? qsTr("Flight mode switched to: Alt Hold") : qsTr("Flight mode switched to: Land")) : qsTr("Flight mode switched to: Land")
            font.underline: true
            font.pointSize: ScreenTools.mediumFontPointSize
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }
        Item{Layout.fillHeight: true}
    }

    Item { id: recoveryView
        anchors.fill: alertWindow
        visible: _recovered
        ColumnLayout{ //Loss of Link Recovery
            anchors.fill: parent
            anchors.margins: _margins
            visible: _recoveredFrom === AscentAlerts.Alerts.LossOfLink
            QGCLabel{
                text: _activeVehicle ? qsTr("CONNECTION RECOVERED") : qsTr("DISCONNECTED FROM\nVEHICLE")
                color:       _accentColor
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: ScreenTools.largeFontPointSize
                font.family: ScreenTools.demiboldFontFamily
                font.bold: true
                Layout.fillWidth: true
            }
            QGCLabel{
                text: qsTr("Current Flight Mode: ")
                font.underline: true
                font.pointSize: ScreenTools.mediumFontPointSize
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                visible: _activeVehicle
            }
            QGCLabel{
                text: _flightMode
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                visible: _activeVehicle
            }
        }

        ColumnLayout{ //Radio Failsafe Recovery
            anchors.fill: parent
            anchors.margins: _margins
            visible: _recoveredFrom === AscentAlerts.Alerts.RadioFailsafe
            QGCLabel{
                text: qsTr("RADIO LINK\nRECOVERED")
                color:       _accentColor
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: ScreenTools.largeFontPointSize
                font.family: ScreenTools.demiboldFontFamily
                font.bold: true
                Layout.fillWidth: true
            }
            QGCLabel{
                text: qsTr("Current Flight Mode: ")
                font.underline: true
                font.pointSize: ScreenTools.mediumFontPointSize
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                visible: _activeVehicle
            }
            QGCLabel{
                text: _flightMode
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                visible: _activeVehicle
            }
        }

        ColumnLayout{ //GPS Glitch Recovery
            anchors.fill: parent
            anchors.margins: _margins
            visible: _recoveredFrom === AscentAlerts.Alerts.GpsGlitch
            QGCLabel{
                text: qsTr("GPS Signal Restored")
                color:       _accentColor
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: ScreenTools.largeFontPointSize
                font.family: ScreenTools.demiboldFontFamily
                font.bold: true
                Layout.fillWidth: true
            }
        }
        ColumnLayout{ //EKF Failsafe Recovery
            anchors.fill: parent
            anchors.margins: _margins
            visible: _recoveredFrom === AscentAlerts.Alerts.EkfFailsafe
            QGCLabel{
                text: qsTr("Vehicle Position\nRecovered")
                color:       _accentColor
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: ScreenTools.largeFontPointSize
                font.family: ScreenTools.demiboldFontFamily
                font.bold: true
                Layout.fillWidth: true
            }
            QGCLabel{
                text: qsTr("All Flight Modes Available")
                font.underline: true
                font.pointSize: ScreenTools.mediumFontPointSize
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }
        }
    }

    ColumnLayout { id: noActionNeededView
        anchors.fill: parent
        anchors.margins: _margins
        visible: _noActionNeeded
        QGCLabel{
            text: qsTr("No Action Needed")
            color:       _accentColor
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: ScreenTools.largeFontPointSize
            font.family: ScreenTools.demiboldFontFamily
            font.bold: true
            Layout.fillWidth: true
        }
    }

    //////////////////////////
    //Minimize Window Button//
    //////////////////////////
    Rectangle {
        anchors.bottom:         parent.top
        anchors.horizontalCenter: ascentAlerts.horizontalCenter
        height:             parent.height / 6
        width:              height * 2
        color:              qgcPal.brandingPurple
        opacity: 0.75
        border.color:       _accentColor
        border.width:       3
        MouseArea {
            anchors.fill:       parent
            onClicked: {
                if(ascentAlerts.state == "minimized"){
                    ascentAlerts.state ="open"
                }
                else if(ascentAlerts.state == "open"){
                    ascentAlerts.state ="minimized"
                    _messageAvailable = false;
                } 
            }
        }
        Image{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                source:                            "qrc:/res/ExpandDown.svg"
                fillMode:                           Image.PreserveAspectFit
                width:                              parent.width * 0.8
                height:                             width
                smooth:                             true
                antialiasing:                       true
                mipmap:                             true
                visible:                            ascentAlerts.state == "open"
        }
        Image{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                source:                            "qrc:/res/ExpandUp.svg"
                fillMode:                           Image.PreserveAspectFit
                width:                              parent.width * 0.8
                height:                             width
                smooth:                             true
                antialiasing:                       true
                mipmap:                             true
                visible:                            ascentAlerts.state == "minimized"
        }
    }
}