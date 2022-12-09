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
    property var cameraModels:                  [ qsTr("~Waiting~"), qsTr("Q10F"), qsTr("Q10T"), qsTr("Z10TIR") , qsTr("Z40K") , qsTr("Z40TIR") , qsTr("H30T"), qsTr("Z10TIR Mini"), qsTr("NightHawk"), qsTr("DragonEye"), qsTr("Raptor")]
    property int _currentCamera:                _activeVehicle ? _activeVehicle.cameraType : 0
    property int _yawAngle:                     _activeVehicle ? controller.cameraYaw : 0
    property int _pitchAngle:                   0
    property bool _selectingFollowTarget:       false
    property bool _recording:                   false
    property real _margins:                     ScreenTools.defaultFontPixelWidth / 2
    property var _currentPage:                  qsTr("Common Controls")
    property int min: 0
    property int sec: 0
    property bool _eoIsMain:                    controller.eoIsMain
    property bool _isPip:                       controller.isPiP
    property int hereLinkPipX: 1258 //Need to change for skynav (misnomer, should be viewpro)
    property int hereLinkPipY: 439  //Need to change for skynav
    property int nextVisPipX: 695
    property int nextVisPipY: 400
    property bool debugMode:                    false

    Component.onCompleted: {
        commonControls.visible      = true
        irPage.visible              = false
        nextVisionPage.visible      = false
        debugPage.visible           = false
        _currentPage            = qsTr("Common Controls")
    }

    on_CurrentCameraChanged: {
        commonControls.visible      = true
        irPage.visible              = false
        debugPage.visible           = false
        _currentPage            = qsTr("Common Controls")
        console.log("CURRENT CAMERA CHANGED")
    }

    function toggleRecording(){
        if(!_recording){controller.record()}
        else if(_recording){controller.stopRecording()}
        _recording = !_recording
        min = 0
        sec = 0
    }

    function incPage(){
        if(commonControls.visible){
            commonControls.visible  = false
            if(cameraModels[_currentCamera] == "Q10F" || cameraModels[_currentCamera] == "Z40K"){
                if(debugMode){
                debugPage.visible       = true
                _currentPage            = qsTr("Debug")
                }
            }
            else{
                irPage.visible          = true
                _currentPage            = qsTr("IR")
            }
        }

        else if(irPage.visible){
            irPage.visible              = false
            if(cameraModels[_currentCamera] == "NightHawk" || cameraModels[_currentCamera] == "DragonEye" || cameraModels[_currentCamera] == "Raptor"){
                nextVisionPage.visible  = true
                _currentPage            = qsTr("NextVision Controls")  
            }
            else if(debugMode){
                debugPage.visible       = true
                _currentPage            = qsTr("Debug")
            }
            else{
                commonControls.visible  = true
                _currentPage            = qsTr("Common Controls")
            }
        }

        else if(nextVisionPage.visible){
            nextVisionPage.visible = false
            if(debugMode){
                debugPage.visible       = true
                _currentPage            = qsTr("Debug")
            }
            else{
                commonControls.visible  = true
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
            else if(cameraModels[_currentCamera] == "NightHawk" || cameraModels[_currentCamera] == "DragonEye" || cameraModels[_currentCamera] == "Raptor"){
                nextVisionPage.visible  = true
                _currentPage            = qsTr("NextVision Controls") 
            }
            else if(cameraModels[_currentCamera] == "Q10F"){
                //Can't go anywhere
            }
            else{
                irPage.visible = true
                _currentPage = qsTr("IR")
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
        else if(debugPage.visible){
            debugPage.visible       = false
            if(cameraModels[_currentCamera] == "NightHawk" || cameraModels[_currentCamera] == "DragonEye" || cameraModels[_currentCamera] == "Raptor"){
                nextVisionPage.visible  = true
                _currentPage            = qsTr("NextVision Controls") 
            }
            else if(cameraModels[_currentCamera] == "Q10F"){
                commonControls.visible  = true
                _currentPage            = qsTr("Common Controls")
            }
            else {
                irPage.visible = true
                _currentPage = qsTr("IR")
            }
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
        color: qgcPal.brandingPurple
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
            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.margins: _margins
                color: qgcPal.brandingPurple

                QGCLabel{
                    anchors.fill: parent
                    text: qsTr(_currentPage)
                    font.bold: true
                    font.pointSize:         ScreenTools.mediumFontPointSize * 1.1
                    font.family:            ScreenTools.demiboldFontFamily
                    horizontalAlignment:    Text.AlignHCenter
                    verticalAlignment:      Text.AlignVCenter
                    Layout.alignment:       Qt.AlignHCenter | Qt.AlignVCenter
                }
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
                    backRadius: height/8
                    pointSize: ScreenTools.defaultFontPointSize * 2
                }
                QGCButton {
                    text: qsTr("LOOK\nDOWN")
                    Layout.fillWidth: true
                    Layout.preferredHeight: ascentTools.width / 4
                    Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * (3)
                    onClicked: controller.lookDown();
                    backRadius: height/8
                    pointSize: ScreenTools.defaultFontPointSize * 2
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
                        toggleRecording()
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
                    backRadius: height/8
                    pointSize: ScreenTools.defaultFontPointSize * 2
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * (3)
                    onClicked: {
                        console.log("Activate EO")
                        controller.eo()
                        controller.digiZoom1()
                        controller.eoIsMain = true
                        controller.isPiP = false
                    }
                }
                QGCButton{
                    id: irButton
                    text: qsTr("IR")
                    backRadius: height/8
                    pointSize: ScreenTools.defaultFontPointSize * 2
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * (3)
                    onClicked: {
                        console.log("Activate IR")
                        controller.ir()
                        controller.eoIsMain = false
                        controller.isPiP = false
                    }
                }
                Item{Layout.fillWidth: true}
            }
            RowLayout {
                Layout.fillHeight: true
                Item{Layout.fillWidth: true}
                QGCButton{
                    text: qsTr("Picture in\n  Picture")
                    backRadius: height/8
                    pointSize: ScreenTools.defaultFontPointSize * 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * (3)
                    onClicked: {
                        if (_eoIsMain && !_isPip) {
                            console.log("Activate EO_IR")
                            controller.eoir()
                            controller.isPiP = true
                        }
                        else if (!_eoIsMain && !_isPip) {
                            controller.ireo()
                            console.log("Activate IR_EO")
                            controller.isPiP = true
                        }
                        else if (_eoIsMain && _isPip) {
                            controller.ireo()
                            console.log("Activate IR_EO")
                            controller.eoIsMain = false
                        }
                        else if (!_eoIsMain && _isPip) {
                            console.log("Activate EO_IR")
                            controller.eoir()
                            controller.eoIsMain = true
                        }
                    }
                }
                QGCButton{
                    text: qsTr("  Color\nPalette")
                    backRadius: height/8
                    pointSize: ScreenTools.defaultFontPointSize * 2
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * (3) 
                    onClicked: {
                        console.log("Next Palette")
                        controller.nextPalette()
                    }
                }
                Item{Layout.fillWidth: true}
            }
            RowLayout {
                visible: (cameraModels[_currentCamera] == "NightHawk" || cameraModels[_currentCamera] == "DragonEye" || cameraModels[_currentCamera] == "Raptor") ? false : true
                Item{Layout.fillWidth: true}
                QGCLabel{text: qsTr("Digital Zoom")}
                Item{Layout.fillWidth: true}
            }
            RowLayout {
                visible: (cameraModels[_currentCamera] == "NightHawk" || cameraModels[_currentCamera] == "DragonEye" || cameraModels[_currentCamera] == "Raptor") ? false : true
                Layout.fillHeight: true
                Item{Layout.fillWidth: true}
                QGCButton{
                    text: qsTr("x1")
                    backRadius: height/8
                    pointSize: ScreenTools.defaultFontPointSize * 2
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    onClicked: controller.digiZoom1()
                }
                QGCButton{
                    text: qsTr("x2")
                    backRadius: height/8
                    pointSize: ScreenTools.defaultFontPointSize * 2
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    onClicked: controller.digiZoom2()
                }
                QGCButton{
                    text: qsTr("x4")
                    backRadius: height/8
                    pointSize: ScreenTools.defaultFontPointSize * 2
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
                    backRadius: height/8
                    pointSize: ScreenTools.defaultFontPointSize * 2
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
                    backRadius: height/8
                    pointSize: ScreenTools.defaultFontPointSize * 2
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    onClicked: controller.nuc();
                }
                Item{Layout.fillWidth: true}
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


            else if((cameraModels[_currentCamera] == "NightHawk" || cameraModels[_currentCamera] == "DragonEye" || cameraModels[_currentCamera] == "Raptor") && _trackingPoint.mouseX >= nextVisPipX && _trackingPoint.mouseY >= nextVisPipY){
                if (_eoIsMain) {
                    console.log("Activate IR_EO")
                    controller.ireo()
                    controller.eoIsMain = false
                }
                else {
                    console.log("Activate EO_IR")
                    controller.eoir()
                    controller.eoIsMain = true
                }
            }

            else if(_trackingPoint.mouseX >= hereLinkPipX && _trackingPoint.mouseY <= hereLinkPipY){
                if (_eoIsMain) {
                    console.log("Activate IR_EO")
                    controller.ireo()
                    controller.eoIsMain = false
                }
                else {
                    console.log("Activate EO_IR")
                    controller.eoir()
                    controller.eoIsMain = true
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