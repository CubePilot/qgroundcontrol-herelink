import QtQuick          2.11
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles  1.4
import QtQuick.Dialogs  1.3
import QtQuick.Layouts  1.2

import QtQuick.Window           2.2
import QtQml.Models             2.1

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controls      1.0
import QGroundControl.Controllers   1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0
import QGroundControl.FlightDisplay 1.0
import QGroundControl.Palette       1.0
import QGroundControl.Vehicle       1.0
import QGroundControl.FlightMap     1.0

Item{
    id:                                         _root

    AscentCC { id: controller }

    property var _activeVehicle:                QGroundControl.multiVehicleManager.activeVehicle
    property var _videoReceiver:                QGroundControl.videoManager.videoReceiver
    property bool _streamingEnabled:            QGroundControl.settingsManager.videoSettings.streamConfigured
    property bool _recordingVideo:              _videoReceiver && _videoReceiver.recording
    property bool _videoRunning:                _videoReceiver && _videoReceiver.videoRunning
    property var cameraModels:                  [ qsTr("~Waiting~"), qsTr("Q10F"), qsTr("Q10T"), qsTr("Z10TIR") , qsTr("Z40K") , qsTr("Z40TIR") , qsTr("H30T"), qsTr("Z10TIR Mini"), qsTr("NightHawk"), qsTr("DragonEye")]
    property int _currentCamera:                _activeVehicle ? _activeVehicle.cameraType : 0
    property int _yawAngle:                     _activeVehicle ? controller.cameraYaw : 0
    property int _pitchAngle:                   0
    property bool _selectingFollowTarget:       false
    property bool _recording:                   false
    property real _margins:                     ScreenTools.defaultFontPixelWidth / 2
    property var _currentPage:                  qsTr("Common Controls")
    property int min: 0
    property int sec: 0
    property bool _eoIsMain: true
    property bool _isPip:                       (cameraModels[_currentCamera] == "Q10F" || cameraModels[_currentCamera] == "Z40K") ? false : true
    property int hereLinkPipX: 1258
    property int hereLinkPipY: 439
    property int nextVisPipX: 1073
    property int nextVisPipY: 549
    property bool debugMode: QGroundControl.corePlugin.showAdvancedUI

    Component.onCompleted: {
        commonControls.visible      = true
        irPage.visible              = false
        nextVisionPage.visible      = false
        streamPage.visible          = false


        debugPage.visible           = false

        _currentPage            = qsTr("Common Controls")
    }

    on_CurrentCameraChanged: {
        commonControls.visible      = true
        irPage.visible              = false
        streamPage.visible          = false

        debugPage.visible           = false

        _currentPage            = qsTr("Common Controls")

    }

    function incPage(){
        if(commonControls.visible){
            commonControls.visible  = false
            if(cameraModels[_currentCamera] == "Q10F"){
                streamPage.visible      = true
                _currentPage            = qsTr("Herelink Stream")
            }
            else{
                irPage.visible          = true
                _currentPage            = qsTr("IR")
            }
        }
        else if(irPage.visible){
            irPage.visible              = false
            if(cameraModels[_currentCamera] == "NightHawk" || cameraModels[_currentCamera] == "DragonEye"){
                nextVisionPage.visible  = true
                _currentPage            = qsTr("NextVision Controls")  
            }
            else{
                streamPage.visible      = true
                _currentPage            = qsTr("Herelink Stream")
            }
        }
        else if(nextVisionPage.visible){
            nextVisionPage.visible = false
            streamPage.visible = true
            _currentPage = qsTr("Herelink Stream")
        }
        else if(streamPage.visible){
            streamPage.visible      = false
            if(debugMode){
                debugPage.visible       = true
                _currentPage            = qsTr("Debug")
            }
            else{
                commonControls.visible = true
                _currentPage            = qsTr("Common Controls")
            }
        }
        else if(debugPage.visible){
            debugPage.visible       = false
            commonControls.visible  = true
            _currentPage            = qsTr("Common Controls")
        }
    }
    
    function decPage(){
        if(commonControls.visible){
            commonControls.visible  = false
            if(debugMode){
                debugPage.visible       = true
                _currentPage            = qsTr("Debug")
            }
            else{
                streamPage.visible      = true
                _currentPage            = qsTr("Herelink Stream")
            }
        }
        else if(irPage.visible){
            irPage.visible          = false
            commonControls.visible  = true
            _currentPage            = qsTr("Common Controls")
        }
        else if(nextVisionPage.visible){
            nextVisionPage.visible = false
            irPage.visible = true
            _currentPage = qsTr("IR")
        }
        else if(streamPage.visible){
            streamPage.visible      = false
            if(cameraModels[_currentCamera] == "Q10F"){
                commonControls.visible  = true
                _currentPage            = qsTr("Common Controls")
            }
            else if(cameraModels[_currentCamera] == "NightHawk" || cameraModels[_currentCamera] == "DragonEye"){
                nextVisionPage.visible          = true
                _currentPage            = qsTr("NextVision Controls")
            }
            else{
                irPage.visible = true
                _currentPage = qsTr("IR")
            }
        }
        else if(debugPage.visible){
            debugPage.visible       = false
            streamPage.visible      = true
            _currentPage            = qsTr("Herelink Stream")
        }
    }

    Rectangle{
        anchors.fill:                       parent
        color:                              qgcPal.brandingPurple
        border.color:                       qgcPal.text
        border.width:                       1
        opacity:                            0.5
    }

    Rectangle { id: ccToolbar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: _root.height / 7
        color: qgcPal.colorGrey
        border.color: qgcPal.text
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.margins: _margins * 0.66
            spacing: 0
            layoutDirection: Qt.RightToLeft
            QGCButton {
                text: qsTr(">")
                Layout.fillHeight: true
                Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * 2
                onClicked: incPage()

            }
            QGCButton {
                text: qsTr("<")
                Layout.fillHeight: true
                Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * 2
                onClicked: decPage()

            }
            Item{Layout.fillWidth: true}
            QGCLabel{
                text: qsTr(_currentPage)
                Layout.fillHeight: true
                font.bold: true
                font.pointSize:         ScreenTools.mediumFontPointSize / 1
            }
        }
    }

    ///////////
    // PAGES //
    ///////////
    Item{
        anchors.top: ccToolbar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: _margins

        //////////////////////////
        // COMMON CONTROLS PAGE //
        //////////////////////////
        ColumnLayout { id: commonControls
            anchors.fill: parent
            spacing: _margins / 2
            //Center Cam
            RowLayout {
                spacing: _margins / 2
                Item{Layout.fillWidth: true}
                QGCButton {
                    text: qsTr("CENTER")
                    Layout.fillWidth: true
                    Layout.preferredHeight: ascentTools.width / 4
                    Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * (3)
                    onClicked: controller.centerCam()
                }
                QGCButton {
                    text: qsTr("LOOK\nDOWN")
                    Layout.fillWidth: true
                    Layout.preferredHeight: ascentTools.width / 4
                    Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * (3)
                    onClicked: controller.lookDown();
                }
                Item{Layout.fillWidth: true}
            }
            Rectangle {Layout.fillHeight: true}
            RowLayout {
                Item{Layout.fillWidth: true}
                Image{
                    source:  "qrc:/res/aaImageMode.svg"
                    Layout.preferredWidth: ascentTools.width / 7
                    Layout.preferredHeight: ascentTools.width / 7
                    fillMode:                           Image.PreserveAspectFit
                    smooth:                             true
                    antialiasing:                       true
                    mipmap:                             true
                }
                Item{Layout.fillWidth: true}
                Image{
                    source:  "qrc:/res/aaVideoMode.svg"
                    Layout.preferredWidth: ascentTools.width / 7
                    Layout.preferredHeight: ascentTools.width / 7
                    fillMode:                           Image.PreserveAspectFit
                    smooth:                             true
                    antialiasing:                       true
                    mipmap:                             true
                }
                Item{Layout.fillWidth: true}
            }
            //Record and take picture buttons
            RowLayout {
                Layout.fillHeight: true
                Item{Layout.fillWidth: true}
                MouseArea{
                    Layout.preferredWidth: ascentTools.width / 5
                    Layout.preferredHeight: ascentTools.width / 5
                    Image{
                        anchors.fill: parent
                        source:  "qrc:/res/aaTakePhoto.svg"
                        smooth:                             true
                        antialiasing:                       true
                        mipmap:                             true
                    }
                    onClicked: controller.takePicture()
                }
                Item{Layout.fillWidth: true}
                MouseArea{
                    Layout.preferredWidth: ascentTools.width / 5
                    Layout.preferredHeight: ascentTools.width / 5
                    Image{
                        anchors.fill: parent
                        source: _recording ? "qrc:/res/aaRecording.svg" : "qrc:/res/aaRecord.svg"
                        smooth:                             true
                        antialiasing:                       true
                        mipmap:                             true
                    }
                    onClicked: {
                        if(!_recording){controller.record()}
                        else if(_recording){controller.stopRecording()}
                        _recording = !_recording
                        min = 0
                        sec = 0
                    }
                }
                Item{Layout.fillWidth: true}
            }

            //Recording Clock
            RowLayout {
                Layout.fillHeight: true
                Timer {
                    interval: 1000
                    running: _recording
                    repeat: true
                    onTriggered: sec = (sec + 1)%60
                }
                Timer {
                    interval: 60000
                    running: _recording
                    repeat: true
                    onTriggered: min = (min + 1)%60
                }
                Item{Layout.fillWidth: true}
                QGCLabel {
                    id: recordDuration
                    visible: _recording
                    text: {
                        if(sec < 10 && min < 10){qsTr("0" + min + " : " + "0" + sec)}
                        else if(sec < 10){qsTr(min + " : " + "0" + sec)}
                        else if(min < 10){qsTr("0" + min + " : " + sec)}
                    }
                    font.pointSize:         ScreenTools.mediumFontPointSize
                    font.family:                        ScreenTools.demiboldFontFamily * 0.75
                    font.bold:                          true
                }
                Item{
                    visible: !_recording
                    Layout.preferredHeight: recordDuration.height
                }
                Item{Layout.fillWidth: true}
            }

            //Implement later for Non tracking cameras (hacked follow)
            RowLayout {
                Layout.fillHeight: true
                visible: false
                QGCButton{
                    text:                       _selectingFollowTarget ? qsTr("Cancel") : qsTr("Follow")
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    onClicked:                  _selectingFollowTarget = !_selectingFollowTarget
                }
            }
        }

        //////////////////////
        // IR CONTROLS PAGE //
        //////////////////////
        ColumnLayout { id: irPage
            anchors.fill: parent
            anchors.margins: _margins * 2
            RowLayout {
                Layout.fillHeight: true
                Item{Layout.fillWidth: true}
                QGCButton{
                    id: eoButton
                    text: qsTr("Day")
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * (3)
                    onClicked: {
                        console.log("Activate EO")
                        controller.eo()
                        controller.digiZoom1()
                        _eoIsMain = true
                        _isPip = false
                    }
                }
                QGCButton{
                    id: irButton
                    text: qsTr("IR")
                    
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * (3)
                    onClicked: {
                        console.log("Activate IR")
                        controller.ir()
                        _eoIsMain = false
                        _isPip = false
                    }
                }
                Item{Layout.fillWidth: true}
            }
            RowLayout {
                Layout.fillHeight: true
                Item{Layout.fillWidth: true}
                QGCButton{
                    text: qsTr("Picture in\n  Picture")
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * (3)
                    onClicked: {
                        if (_eoIsMain && !_isPip) {
                            console.log("Activate EO_IR")
                            controller.eoir()
                            _isPip = true
                        }
                        else if (!_eoIsMain && !_isPip) {
                            controller.ireo()
                            console.log("Activate IR_EO")
                            _isPip = true
                        }
                        else if (_eoIsMain && _isPip) {
                            controller.ireo()
                            console.log("Activate IR_EO")
                            _eoIsMain = false
                        }
                        else if (!_eoIsMain && _isPip) {
                            console.log("Activate EO_IR")
                            controller.eoir()
                            _eoIsMain = true
                        }
                    }
                }
                QGCButton{
                    text: qsTr("  Color\nPallette")
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * (3) 
                    onClicked: {
                        console.log("Next Pallette")
                        controller.nextPallette()
                    }
                }
                Item{Layout.fillWidth: true}
            }
            RowLayout {
                visible: (cameraModels[_currentCamera] == "NightHawk" || cameraModels[_currentCamera] == "DragonEye") ? false : true
                Item{Layout.fillWidth: true}
                QGCLabel{text: qsTr("Digital Zoom")}
                Item{Layout.fillWidth: true}
            }
            RowLayout {
                visible: (cameraModels[_currentCamera] == "NightHawk" || cameraModels[_currentCamera] == "DragonEye") ? false : true
                Layout.fillHeight: true
                Item{Layout.fillWidth: true}
                QGCButton{
                    text: qsTr("x1")
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    onClicked: controller.digiZoom1()
                }
                QGCButton{
                    text: qsTr("x2")
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    onClicked: controller.digiZoom2()
                }
                QGCButton{
                    text: qsTr("x4")
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    onClicked: controller.digiZoom4()
                }
                Item{Layout.fillWidth: true}
            }
        }


        //////////////////////////////
        // NextVision CONTROLS PAGE //
        //////////////////////////////
        ColumnLayout { id: nextVisionPage
            anchors.fill: parent
            anchors.margins: _margins * 2
            RowLayout {
                Layout.fillHeight: true
                Item{Layout.fillWidth: true}
                QGCButton{
                    text: qsTr("Toggle Heat")
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    onClicked: controller.toggleHeat()
                }
                Item{Layout.fillWidth: true}
            }
            RowLayout {
                Layout.fillHeight: true
                Item{Layout.fillWidth: true}
                QGCButton{
                    text: qsTr("NUC")
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    onClicked: controller.nuc();
                }
                Item{Layout.fillWidth: true}
            }
        }




        ///////////////////////
        // VIDEO STREAM PAGE //
        ///////////////////////
        ExclusiveGroup { id:cameraIdGroup }
        ColumnLayout { id: streamPage
            anchors.fill: parent
            anchors.margins: _margins * 2
            RowLayout {
                Layout.fillHeight: true
                Item{Layout.fillWidth: true}
                QGCRadioButtonOld {
                    Layout.alignment: Qt.AlignHCenter
                    exclusiveGroup: cameraIdGroup
                    text:           "Stream 1"
                    checked:        QGroundControl.settingsManager.videoSettings.cameraId.rawValue === 0
                    enabled:        !QGroundControl.videoManager.videoStreamControl.settingInProgress
                    onClicked:      QGroundControl.settingsManager.videoSettings.cameraId.rawValue = 0

                }
                Item{Layout.fillWidth: true}
                QGCRadioButtonOld {
                    Layout.alignment: Qt.AlignHCenter
                    exclusiveGroup: cameraIdGroup
                    text:           "Stream 2"
                    checked:        QGroundControl.settingsManager.videoSettings.cameraId.rawValue === 1
                    enabled:        !QGroundControl.videoManager.videoStreamControl.settingInProgress
                    onClicked:      QGroundControl.settingsManager.videoSettings.cameraId.rawValue = 1
                }
                Item{Layout.fillWidth: true}
            }
            // resolution
            QGCLabel {
                text:            qsTr("1080P video")
                Layout.alignment: Qt.AlignHCenter
            }
            QGCSwitch {
                id:             fhdSwitch
                Layout.alignment: Qt.AlignHCenter
                enabled:        !QGroundControl.videoManager.videoStreamControl.settingInProgress
                checked:        (QGroundControl.settingsManager.videoSettings.videoResolution.rawValue === 2)
                                || ((QGroundControl.settingsManager.videoSettings.videoResolution.rawValue === 0)
                                    &&(QGroundControl.videoManager.videoStreamControl.videoResolution === "1920x1080"))
                onCheckedChanged: {
                    QGroundControl.videoManager.videoStreamControl.fhdEnabledChanged(checked)
                }
            }
            Item{Layout.fillHeight: true}
            RowLayout {
                Layout.fillHeight: true
                QGCLabel {
                    text:               qsTr("File Name");
                    font.pointSize:     ScreenTools.smallFontPointSize
                    visible:            QGroundControl.videoManager.isGStreamer
                }
                QGCTextField {
                    id:                 videoFileName
                    Layout.fillWidth:   true
                    visible:            QGroundControl.videoManager.isGStreamer
                }
            }
            Item{Layout.fillHeight: true}
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillHeight: true
                QGCLabel {
                    text:            _recordingVideo ? qsTr("Stop Recording") : qsTr("Record Stream")
                    font.pointSize:  ScreenTools.smallFontPointSize
                    visible:         QGroundControl.videoManager.isGStreamer
                }
                Item {
                    anchors.margins:    ScreenTools.defaultFontPixelHeight / 2
                    height:             ScreenTools.defaultFontPixelHeight * 2
                    width:              height
                    Layout.alignment:   Qt.AlignHCenter
                    visible:            QGroundControl.videoManager.isGStreamer
                    Rectangle {
                        id:                 recordBtnBackground
                        anchors.top:        parent.top
                        anchors.bottom:     parent.bottom
                        width:              height
                        radius:             _recordingVideo ? 0 : height
                        color:              (_videoRunning && _streamingEnabled) ? "red" : "gray"
                        SequentialAnimation on opacity {
                            running:        _recordingVideo
                            loops:          Animation.Infinite
                            PropertyAnimation { to: 0.5; duration: 500 }
                            PropertyAnimation { to: 1.0; duration: 500 }
                        }
                    }
                    QGCColoredImage {
                        anchors.top:                parent.top
                        anchors.bottom:             parent.bottom
                        anchors.horizontalCenter:   parent.horizontalCenter
                        width:                      height * 0.625
                        sourceSize.width:           width
                        source:                     "/qmlimages/CameraIcon.svg"
                        visible:                    recordBtnBackground.visible
                        fillMode:                   Image.PreserveAspectFit
                        color:                      "white"
                    }
                    MouseArea {
                        anchors.fill:   parent
                        enabled:        _videoRunning && _streamingEnabled
                        onClicked: {
                            if (_recordingVideo) {
                                _videoReceiver.stopRecording()
                                // reset blinking animation
                                recordBtnBackground.opacity = 1
                            } else {
                                _videoReceiver.startRecording(videoFileName.text)
                            }
                        }
                    }
                }
            }
            Item{Layout.fillHeight: true}
            RowLayout {
                Layout.fillHeight: true
                QGCLabel {
                    text:               qsTr("Video Streaming Not Configured")
                    visible:            !_streamingEnabled
                    Layout.columnSpan:  2
                }
            }
        }

        ////////////////////
        // DEBUGGING PAGE //
        ////////////////////
        ColumnLayout { id: debugPage
            anchors.fill: parent
            anchors.margins: _margins * 2
            RowLayout {
                Layout.fillHeight: true
                Item{Layout.fillWidth: true}
                QGCLabel{text: cameraModels[_currentCamera]}
                Item{Layout.fillWidth: true}
            }
            RowLayout {
                Layout.fillHeight: true
                Item{Layout.fillWidth: true}
                QGCButton{
                    text: qsTr("Debug")
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    onClicked: {
                       controller.debug()
                       //console.log(_yawAngle)
                    }
                }
                Item{Layout.fillWidth: true}
            }
            RowLayout {
                Layout.fillHeight: true
                Item{Layout.fillWidth: true}
                QGCButton{
                    text: qsTr("NUC")
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    onClicked: controller.nuc();
                }
                Item{Layout.fillWidth: true}
            }
        }
    }





    Connections{
        target:         _trackingPoint
        onClicked: {
            if(!_isPip){
                console.log("X = ", _trackingPoint.mouseX,", Y = " ,_trackingPoint.mouseY)
                controller.trackPoint(_trackingPoint.mouseX, _trackingPoint.mouseY)
            }


            else if((cameraModels[_currentCamera] == "NightHawk" || cameraModels[_currentCamera] == "DragonEye") && _trackingPoint.mouseX >= nextVisPipX && _trackingPoint.mouseY >= nextVisPipY){
                if (_eoIsMain) {
                    console.log("Activate IR_EO")
                    controller.ireo()
                    _eoIsMain = false
                }
                else {
                    console.log("Activate EO_IR")
                    controller.eoir()
                    _eoIsMain = true
                }
            }

            else if(_trackingPoint.mouseX >= hereLinkPipX && _trackingPoint.mouseY <= hereLinkPipY){
                if (_eoIsMain) {
                    console.log("Activate IR_EO")
                    controller.ireo()
                    _eoIsMain = false
                }
                else {
                    console.log("Activate EO_IR")
                    controller.eoir()
                    _eoIsMain = true
                }
            }

            else {
                console.log("X = ", _trackingPoint.mouseX,", Y = " ,_trackingPoint.mouseY)
                controller.trackPoint(_trackingPoint.mouseX, _trackingPoint.mouseY)
            }
        }
    }

    //Select Target Prompt (Feature for non tracking cameras)
    Item{
        anchors.horizontalCenter:       parent.horizontalCenter
        anchors.top:                    parent.bottom
        anchors.margins:                _margins
        width:                          parent.width
        height:                         parent.height/4
        visible:                        _selectingFollowTarget
        Rectangle{
            anchors.fill:                       parent
            color:                              qgcPal.window
            opacity:                            0.6
            border.color:                       qgcPal.colorRed
            border.width:                       2
        }
        QGCLabel{
            text:                               qsTr("Select your target")
            color:                              qgcPal.colorRed
            anchors.verticalCenter:             parent.verticalCenter
            anchors.horizontalCenter:           parent.horizontalCenter
            font.pointSize:                     ScreenTools.mediumFontPointSize * 0.8
            font.family:                        ScreenTools.demiboldFontFamily
            font.bold:                          true
        }
    }

}