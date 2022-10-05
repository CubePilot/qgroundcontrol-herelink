import QtQuick              2.11
import QtGraphicalEffects   1.0

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Controllers   1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Palette       1.0
import QGroundControl.FlightMap     1.0

Item {
    id: root

    property bool showPitch:    true
    property var  vehicle:      null
    property real size
    property bool showHeading:  true
    property real _rollAngle:   vehicle ? vehicle.roll.rawValue  : 0
    property real _pitchAngle:  vehicle ? vehicle.pitch.rawValue : 0
    property int _cameraYawAngle:     0
    property bool _camera:      true

    width:  size
    height: size

    Item {
        id:             instrument
        anchors.fill:   parent
        visible:        false
        //----------------------------------------------------
        //-- Artificial Horizon
        AscentArtificialHorizon {
            rollAngle:          _rollAngle
            pitchAngle:         _pitchAngle
            skyColor1:          "#b0b0b0"
            skyColor2:          "#b0b0b0"
            groundColor1:       qgcPal.brandingPurple
            groundColor2:       qgcPal.brandingPurple
            anchors.fill:       parent

        }
    }

    Rectangle {
        id:             mask
        anchors.fill:   instrument
        radius:         width / 2
        color:          "black"
        visible:        false
    }

    OpacityMask {
        anchors.fill:   instrument
        source:         instrument
        maskSource:     mask
    }

    Rectangle {
        id:             borderRect
        anchors.fill:   parent
        radius:         width / 2
        color:          Qt.rgba(0,0,0,0)
        border.color:   "#000"
        border.width:   1
        opacity:        0.7
    }
    Item {
        id: ascentCompass
        property int _heading:              vehicle ? vehicle.heading.rawValue.toFixed(0) : 0
        anchors.fill:                           instrument
        rotation: _heading * -1
        opacity: 1.0
        z: -1

        Image{
            anchors.centerIn:                   parent
            source:                             "qrc:/res/aaCompassTicks.svg"
            fillMode:                           Image.PreserveAspectFit
            height:                             instrument.height * 1.1
            width:                              height
            smooth:                             true
            antialiasing:                       true
            mipmap:                             true
        }

        Rectangle{
            id: northPoint
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.top
            anchors.bottomMargin: ScreenTools.defaultFontPixelHeight * 0.45
            width: ScreenTools.defaultFontPixelHeight * 0.5
            height: ScreenTools.defaultFontPixelHeight * 0.5
            color: qgcPal.colorGreen
            visible: false
        }
        Rectangle{
            id: southPoint
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.bottom
            anchors.topMargin: ScreenTools.defaultFontPixelHeight * 0.45
            width: ScreenTools.defaultFontPixelHeight * 0.5
            height: ScreenTools.defaultFontPixelHeight * 0.5
            color: qgcPal.colorGreen
            visible: false
        }
        Rectangle{
            id: eastPoint
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.right
            anchors.leftMargin: ScreenTools.defaultFontPixelHeight * 0.45
            width: ScreenTools.defaultFontPixelHeight * 0.5
            height: ScreenTools.defaultFontPixelHeight * 0.5
            color: qgcPal.colorGreen
            visible: false
        }
        Rectangle{
            id: westPoint
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.left
            anchors.rightMargin: ScreenTools.defaultFontPixelHeight * 0.45
            width: ScreenTools.defaultFontPixelHeight * 0.5
            height: ScreenTools.defaultFontPixelHeight * 0.5
            color: qgcPal.colorGreen
            visible: false
        }

        Image{
            anchors.centerIn: northPoint
            source:                             "qrc:/res/aaNorth.svg"
            fillMode:                           Image.PreserveAspectFit
            height:                             instrument.height/7
            width:                              height
            smooth:                             true
            antialiasing:                       true
            mipmap:                             true
            rotation: ascentCompass._heading
        }


        Image{
            anchors.centerIn: southPoint
            source:                             "qrc:/res/aaSouth.svg"
            fillMode:                           Image.PreserveAspectFit
            height:                             instrument.height/9
            width:                              height
            smooth:                             true
            antialiasing:                       true
            mipmap:                             true
            rotation: ascentCompass._heading
        }
        Image{
            anchors.centerIn: eastPoint
            source:                             "qrc:/res/aaEast.svg"
            fillMode:                           Image.PreserveAspectFit
            height:                             instrument.height/9
            width:                              height
            smooth:                             true
            antialiasing:                       true
            mipmap:                             true
            rotation: ascentCompass._heading
        }
        Image{
            anchors.centerIn: westPoint
            source:                             "qrc:/res/aaWest.svg"
            fillMode:                           Image.Stretch 
            height:                             instrument.height/9
            width:                              height
            smooth:                             true
            antialiasing:                       true
            mipmap:                             true
            rotation: ascentCompass._heading
        }
    }

    Image{
        id:                                 gimbalDir
        anchors.horizontalCenter:           parent.horizontalCenter
        anchors.bottom:                     parent.verticalCenter
        source:                             "qrc:/res/aaGimbalDirection.svg"
        fillMode:                           Image.PreserveAspectFit
        height:                             instrument.height/2
        width:                              height
        smooth:                             true
        antialiasing:                       true
        mipmap:                             true
        visible:                            _camera

        transform: [
            Rotation {
                origin.x: gimbalDir.width/2
                origin.y: gimbalDir.height
                angle:    _cameraYawAngle
            }
        ]
    }
    
}