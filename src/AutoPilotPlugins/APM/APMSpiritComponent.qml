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
import QtQuick.Dialogs  1.2
import QtQuick.Layouts  1.2

import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0
import QGroundControl.Palette       1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0

SetupPage {
    id:             spiritPage
    pageComponent:  spiritPageComponent

    FactPanelController {
        id:         controller
    }

    property real _margins:         ScreenTools.defaultFontPixelHeight

    Component {
        id: spiritPageComponent

        Flow {
            id:         flowLayout
            width:      availableWidth
            spacing:    _margins

            property Fact battNumber:       controller.getParameterFact(-1, "SPIRIT_BATT_NUM"       , false)
            property Fact payloadWeight:    controller.getParameterFact(-1, "SPIRIT_PAYLD_WT"       , false)
            property Fact camType:          controller.getParameterFact(-1, "SPIRIT_CAM_TYPE"       , false)
            property Fact ser2Baud:         controller.getParameterFact(-1, "SERIAL2_BAUD"          , false)
            property Fact ser2Proto:        controller.getParameterFact(-1, "SERIAL2_PROTOCOL"      , false)
            property Fact mntType:          controller.getParameterFact(-1, "MNT_TYPE"              , false)

            QGCPalette { id: ggcPal; colorGroupEnabled: true }

            Column {
                spacing: 20
                visible: true

                Rectangle {
                    width:  availableWidth
                    height: spiritParamsGrid.y + spiritParamsGrid.height + _margins
                    color:  ggcPal.windowShade
                    
                    ColumnLayout {
                        id: spiritParamsGrid
                        spacing:  _margins
                        anchors.margins:    _margins

                        QGCLabel {
                            text:       qsTr("SPIRIT PARAMETERS")
                            font.family: ScreenTools.demiboldFontFamily
                            font.pointSize: 15
                        }

                        RowLayout{
                            QGCLabel {
                                id:             numberOfBattFieldBanner
                                text:           qsTr("Number of batteries:")
                            }

                            QGCComboBox {
                                id:             numberOfBattField
                                width:          ScreenTools.defaultFontPixelWidth * 15
                                model:          [ qsTr("1"), qsTr("2") ]
                                currentIndex:   battNumber.value - 1
                                onActivated:    battNumber.value = index + 1
                            }
                        }
                        QGCLabel {
                            text:           qsTr("WARNING - ONE battery shall be mounted on the top of the vehicle")
                        }

                        RowLayout{
                            QGCLabel {
                                id: camTypeFieldBanner
                                text:           qsTr("Camera type:")
                            }

                            QGCComboBox {
                                id:             camTypeField
                                implicitWidth:          ScreenTools.defaultFontPixelWidth * 20
                                model:          [ qsTr("None"), qsTr("Q10F"), qsTr("Q10T"), qsTr("Z10TIR-35") , qsTr("Z40K") , qsTr("Z40TIR"), qsTr("H30T"), qsTr("Z10TIR Mini"), qsTr("NightHawk"), qsTr("DragonEye"), qsTr("Raptor")]
                                
                                currentIndex:   camType.value
                                onActivated:    {
                                    camType.value = index
                                    if(index == 1 || index == 2 || index == 3 || index == 4 || index == 5 || index == 6 || index == 7){
                                        ser2Proto.value = 38;
                                        mntType.value = 6;
                                        ser2Baud.value = 115;
                                    }
                                    if(index == 8 || index == 9 || index == 10){
                                        ser2Proto.value = 2;
                                        mntType.value = 7;
                                        ser2Baud.value = 115;
                                    }
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    width:  availableWidth
                    height: spiritAdvParamsGrid.y + spiritAdvParamsGrid.height + _margins
                    color:  ggcPal.windowShade

                    ColumnLayout {
                        id: spiritAdvParamsGrid
                        anchors.margins:    _margins

                        QGCLabel {
                            text:       qsTr("ADVANCED PARAMETERS")
                            font.family: ScreenTools.demiboldFontFamily
                            font.pointSize: 15
                        }

                        QGCLabel {
                            id:         advancedUserCheckbox
                            text:       qsTr("DO NOT MODIFY UNLESS YOU ARE PDK DEVELOPER OR USING CUSTOM PAYLOAD")
                        }

                        QGCLabel {
                            id:     plWeightFieldBanner
                            text:           qsTr("Additional Payload weight in lbs:")
                        }

                        FactTextField {
                            id:     plWeightField
                            width:  20
                            fact:   payloadWeight
                        }

                        QGCLabel {
                            font.pointSize: 11
                            textFormat: Text.RichText
                            color:  qgcPal.text
                            text: "(Excluding landing gear and standard GPS lid, Default 0)"
                        }
                    }
                }
            } // Column
        } // Flow
    } // Component - spiritPageComponent
} // SetupPage

