/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick              2.3
import QtLocation           5.3
import QtPositioning        5.3
import QtGraphicalEffects   1.0

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Vehicle       1.0
import QGroundControl.Controls      1.0

/// Marker for displaying a vehicle location on the map
MapQuickItem {
    property var    vehicle                                                         /// Vehicle object, undefined for ADSB vehicle
    property var    map
    property double altitude:       Number.NaN                                      ///< NAN to not show
    property string callsign:       ""                                              ///< Vehicle callsign
    property double heading:        vehicle ? vehicle.heading.value : Number.NaN    ///< Vehicle heading, NAN for none
    property real   size:           _adsbVehicle ? _adsbSize : _uavSize             /// Size for icon
    property bool   alert:          false                                           /// Collision alert

    anchorPoint.x:  vehicleItem.width  / 2
    anchorPoint.y:  vehicleItem.height / 2
    visible:        coordinate.isValid

    property bool   _adsbVehicle:   vehicle ? false : true
    property real   _uavSize:       ScreenTools.defaultFontPixelHeight * 5
    property real   _adsbSize:      ScreenTools.defaultFontPixelHeight * 2.5
    property var    _map:           map
    property bool   _multiVehicle:  QGroundControl.multiVehicleManager.vehicles.count > 1

    property int    _cameraYawAngle:          0
    property bool   _camera:                  false                  
    property int    _cameraType:              _activeVehicle ? _activeVehicle.cameraType : 0

    on_CameraTypeChanged: {
        _cameraType == 0 ? _camera = false : (
        _cameraType == 8 ? _camera = false : (
        _cameraType == 9 ? _camera = false : (
        _cameraType == 10 ? _camera = false : _camera = true)))
    }

    sourceItem: Item {
        id:         vehicleItem
        width:      vehicleIcon.width
        height:     vehicleIcon.height
        opacity:    vehicle ? (vehicle.active ? 1.0 : 0.5) : 1.0

        Rectangle {
            id:                 vehicleShadow
            anchors.fill:       vehicleIcon
            color:              Qt.rgba(1,1,1,1)
            radius:             width * 0.5
            visible:            false
        }
        DropShadow {
            anchors.fill:       vehicleShadow
            visible:            vehicleIcon.visible && _adsbVehicle
            horizontalOffset:   4
            verticalOffset:     4
            radius:             32.0
            samples:            65
            color:              Qt.rgba(0.94,0.91,0,0.5)
            source:             vehicleShadow
        }



        Image{
            id:                                 gimbalDir
            opacity:                            (!_adsbVehicle && _camera) ? 1.0 : 0.0
            anchors.horizontalCenter:           vehicleIcon.horizontalCenter
            anchors.bottom:                     vehicleIcon.verticalCenter
            source:                             "qrc:/res/aaGimbalDirection.svg"
            fillMode:                           Image.PreserveAspectFit
            height:                             vehicleIcon.height * 1.5
            width:                              height
            smooth:                             true
            antialiasing:                       true
            mipmap:                             true

            transform: [
                Rotation {
                    origin.x: gimbalDir.width/2
                    origin.y: gimbalDir.height
                    angle:    isNaN(heading) ? 0 : heading + _cameraYawAngle
                }
            ]
        }


        
        Image {
            id:                 vehicleIcon
            source:             _adsbVehicle ? (alert ? "/qmlimages/AlertAircraft.svg" : "/qmlimages/AwarenessAircraft.svg") : vehicle.vehicleImageOpaque
            mipmap:             true
            width:              size
            sourceSize.width:   size
            fillMode:           Image.PreserveAspectFit
            transform: Rotation {
                origin.x:       vehicleIcon.width  / 2
                origin.y:       vehicleIcon.height / 2
                angle:          isNaN(heading) ? 0 : heading
            }
        }

        QGCMapLabel {
            id:                         vehicleLabel
            anchors.top:                parent.bottom
            anchors.horizontalCenter:   parent.horizontalCenter
            map:                        _map
            text:                       vehicleLabelText
            font.pointSize:             _adsbVehicle ? ScreenTools.defaultFontPointSize : ScreenTools.smallFontPointSize
            visible:                    _adsbVehicle ? !isNaN(altitude) : _multiVehicle
            property string vehicleLabelText: visible ?
                                                  (_adsbVehicle ?
                                                       QGroundControl.metersToAppSettingsDistanceUnits(altitude).toFixed(0) + " " + QGroundControl.appSettingsDistanceUnitsString :
                                                       (_multiVehicle ? qsTr("Vehicle %1").arg(vehicle.id) : "")) :
                                                  ""

        }
    }
}
