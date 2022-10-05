import QtQuick          2.11
import QtQuick.Controls 1.2
import QtQuick.Dialogs  1.3
import QtQuick.Layouts  1.2

import QtQuick.Window           2.2
import QtQml.Models             2.1

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controls      1.0
import QGroundControl.Controllers   1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FlightDisplay 1.0
import QGroundControl.Palette       1.0
import QGroundControl.Vehicle       1.0
import QGroundControl.FlightMap     1.0

Item{
    id:                                       _root
    property var _activeVehicle:              QGroundControl.multiVehicleManager.activeVehicle
    property string _altitude:          _activeVehicle ? (isNaN(_activeVehicle.altitudeRelative.value) ? "0.0" : _activeVehicle.altitudeRelative.value.toFixed(1)) + ' ' + _activeVehicle.altitudeRelative.units : "0.0"
    property string _verticalSpeed:     _activeVehicle ? (_activeVehicle.climbRate.value >=0 ? "+" + _activeVehicle.climbRate.value.toFixed(1)+ ' ' +_activeVehicle.climbRate.units : "" + _activeVehicle.climbRate.value.toFixed(1) + ' ' +_activeVehicle.climbRate.units) : "0.0"
    property string _horizontalSpeed:   _activeVehicle ? _activeVehicle.groundSpeed.value.toFixed(1) + ' ' + _activeVehicle.groundSpeed.units : "0.0"
    property string _distHome:          _activeVehicle ? ('00000' + _activeVehicle.distanceToHome.value.toFixed(0)).slice(-5) + ' ' + _activeVehicle.distanceToHome.units : "00000"
    property var    _imageSize:         height/5
    property var    _fontSize:          ScreenTools.mediumFontPointSize * 1.25

    property var metricAlt:        _activeVehicle ? QGroundControl.appSettingsDistanceUnitsToMeters(_activeVehicle.altitudeRelative.value) : 0.0
    property var metricDistHome:   _activeVehicle ? QGroundControl.appSettingsDistanceUnitsToMeters(_activeVehicle.distanceToHome.value) : 0
    property var metricVS:         _activeVehicle ? (_activeVehicle.climbRate.units == "m/s" ? _activeVehicle.climbRate.value : (_activeVehicle.climbRate.units == "ft/s" ? _activeVehicle.climbRate.value/3.281 : (_activeVehicle.climbRate.units == "mph" ? _activeVehicle.climbRate.value/2.237 : (_activeVehicle.climbRate.units == "km/h" ? _activeVehicle.climbRate.value/3.6 : (_activeVehicle.climbRate.units == "kn" ? _activeVehicle.climbRate.value/1.944 : 0.0))))): 0.0
    property var metricHS:          _activeVehicle ? (_activeVehicle.groundSpeed.units == "m/s" ? _activeVehicle.groundSpeed.value : (_activeVehicle.groundSpeed.units == "ft/s" ? _activeVehicle.groundSpeed.value/3.281 : (_activeVehicle.groundSpeed.units == "mph" ? _activeVehicle.groundSpeed.value/2.237 : (_activeVehicle.groundSpeed.units == "km/h" ? _activeVehicle.groundSpeed.value/3.6 : (_activeVehicle.groundSpeed.units == "kn" ? _activeVehicle.groundSpeed.value/1.944 : 0.0))))): 0.0
    
    property var impAlt:            _activeVehicle ? metricAlt * 3.281 : 0.0
    property var impDistHome:       _activeVehicle ? metricDistHome * 3.281 : 0
    property var impVS:             _activeVehicle ? metricVS * 2.237 : 0.0
    property var impHS:             _activeVehicle ? metricHS * 2.237 : 0.0

    property var metricVS2:         _activeVehicle ? metricVS * 3.6 : 0.0
    property var metricHS2:         _activeVehicle ? metricHS * 3.6 : 0.0

    property bool isAltMetric: true
    property bool isDistHomeMetric: true
    property int _HSUnit: 0
    property int _VSUnit: 0

    Rectangle{
        anchors.fill:                       parent
        color:                              qgcPal.window
        opacity:                            0.6
        border.color:                       qgcPal.text
        border.width:                       1
    }

    GridLayout {
        anchors.fill: parent
        anchors.margins: ScreenTools.defaultFontPixelWidth * 5
        columnSpacing:    ScreenTools.defaultFontPixelWidth * 5
        columns: 2
        rows: 6

        //Horizontal Speed Image
        Image{
            source:                             "qrc:/res/HSpeed.svg"
            smooth:                             true
            antialiasing:                       true
            mipmap:                             true
            fillMode:                           Image.PreserveAspectFit
            Layout.preferredHeight:             _imageSize
            Layout.preferredWidth:              _imageSize
            Layout.alignment:                   Qt.AlignHCenter | Qt.AlignVCenter

            MouseArea {
                anchors.fill: parent
                onClicked: _HSUnit = (_HSUnit + 1) % 3
            }
        }

        //Veritcal Speed Image
        Image{
            source:                             "qrc:/res/VSpeed.svg"
            smooth:                             true
            antialiasing:                       true
            mipmap:                             true
            fillMode:                           Image.PreserveAspectFit
            Layout.preferredHeight:             _imageSize
            Layout.preferredWidth:              _imageSize
            Layout.alignment:                   Qt.AlignHCenter | Qt.AlignVCenter

            MouseArea {
                anchors.fill: parent
                onClicked: _VSUnit = (_VSUnit + 1) % 3
            }
        }

        //Horizontal Speed Label
        QGCLabel {
            text:                   _HSUnit == 0 ? qsTr("%1 m/s").arg(metricHS.toFixed(1)) : (_HSUnit == 1 ? qsTr("%1 mph").arg(impHS.toFixed(1)) : qsTr("%1 km/hr").arg(metricHS2.toFixed(1)))
            color:                  qgcPal.text
            horizontalAlignment:    Text.AlignHCenter
            font.pointSize:         _fontSize
            font.family:                        ScreenTools.demiboldFontFamily * 0.75
            font.bold:                          true
            Layout.alignment:                   Qt.AlignHCenter | Qt.AlignVCenter
        }

        //Veritcal Speed Label
        QGCLabel {
            text:                   _VSUnit == 0 ? qsTr("%1 m/s").arg(metricVS.toFixed(1)) : (_VSUnit == 1 ? qsTr("%1 mph").arg(impVS.toFixed(1)) : qsTr("%1 km/hr").arg(metricVS2.toFixed(1)))
            color:                  qgcPal.text
            horizontalAlignment:    Text.AlignHCenter
            font.pointSize:         _fontSize
            font.family:                        ScreenTools.demiboldFontFamily * 0.75
            font.bold:                          true
            Layout.alignment:                   Qt.AlignHCenter | Qt.AlignVCenter
        }

        //Grid Layout Spacer Items
        Item { Layout.fillHeight: true }
        Item { Layout.fillHeight: true }

        //Altitude Image
        Image{
            source:                             "qrc:/res/aaAltitude.svg"
            smooth:                             true
            antialiasing:                       true
            mipmap:                             true
            fillMode:                           Image.PreserveAspectFit
            Layout.preferredHeight:             _imageSize
            Layout.preferredWidth:              _imageSize
            Layout.alignment:                   Qt.AlignHCenter | Qt.AlignVCenter

            MouseArea {
                anchors.fill: parent
                onClicked: isAltMetric = !isAltMetric
            }
        }

        //Distance to Home Image
        Image{
            source:                             "qrc:/res/DTH.svg"
            smooth:                             true
            antialiasing:                       true
            mipmap:                             true
            fillMode:                           Image.PreserveAspectFit
            Layout.preferredHeight:             _imageSize
            Layout.preferredWidth:              _imageSize
            Layout.alignment:                   Qt.AlignHCenter | Qt.AlignVCenter

            MouseArea {
                anchors.fill: parent
                onClicked: isDistHomeMetric = !isDistHomeMetric
            }
        }

        //Altitude Label
        QGCLabel {
            text:                   isAltMetric ? qsTr("%1 m").arg(metricAlt.toFixed(1)) : qsTr("%1 ft").arg(impAlt.toFixed(1))
            color:                  qgcPal.text
            horizontalAlignment:    Text.AlignHCenter
            verticalAlignment:      Text.AlignVCenter
            font.pointSize:         _fontSize
            font.family:                        ScreenTools.demiboldFontFamily * 0.75
            font.bold:                          true
            Layout.alignment:                   Qt.AlignHCenter | Qt.AlignVCenter
        }

        //Distance to home label
        QGCLabel {
            text:                   isDistHomeMetric ? qsTr("%1 m").arg(metricDistHome.toFixed(0)) : qsTr("%1 ft").arg(impDistHome.toFixed(0))
            color:                  qgcPal.text
            horizontalAlignment:    Text.AlignHCenter
            verticalAlignment:      Text.AlignVCenter
            font.pointSize:         _fontSize
            font.family:                        ScreenTools.demiboldFontFamily * 0.75
            font.bold:                          true
            Layout.alignment:                   Qt.AlignHCenter | Qt.AlignVCenter
        }
    }
}