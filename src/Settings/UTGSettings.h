/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

/// @file
/// @brief UTG (Ultrasonic Thickness Gauge) Settings

#pragma once

#include "SettingsGroup.h"

/// UTG Settings
class UTGSettings : public SettingsGroup
{
    Q_OBJECT

public:
    UTGSettings(QObject* parent = nullptr);

    DEFINE_SETTING_NAME_GROUP()

    // Serial Port Configuration
    DEFINE_SETTINGFACT(enabled)
    DEFINE_SETTINGFACT(serialPort)
    DEFINE_SETTINGFACT(baudRate)
    DEFINE_SETTINGFACT(dataBits)
    DEFINE_SETTINGFACT(stopBits)
    DEFINE_SETTINGFACT(parity)
    DEFINE_SETTINGFACT(flowControl)
    
    // UTG Operational Settings
    DEFINE_SETTINGFACT(autoConnect)
    DEFINE_SETTINGFACT(measurementUnit)
    DEFINE_SETTINGFACT(measurementMode)
    DEFINE_SETTINGFACT(soundVelocity)
    DEFINE_SETTINGFACT(zeroOffset)
    DEFINE_SETTINGFACT(gain)
    DEFINE_SETTINGFACT(threshold)
    DEFINE_SETTINGFACT(pulseWidth)
    DEFINE_SETTINGFACT(frequency)
    
    // Display and Logging Settings
    DEFINE_SETTINGFACT(displayPrecision)
    DEFINE_SETTINGFACT(logMeasurements)
    DEFINE_SETTINGFACT(logFilePath)
    DEFINE_SETTINGFACT(autoSaveMeasurements)
    
    // Calibration Settings
    DEFINE_SETTINGFACT(calibrationMode)
    DEFINE_SETTINGFACT(calibrationValue)
    DEFINE_SETTINGFACT(temperatureCompensation)
    DEFINE_SETTINGFACT(materialType)
    
    // Alert Settings
    DEFINE_SETTINGFACT(minThicknessAlert)
    DEFINE_SETTINGFACT(maxThicknessAlert)
    DEFINE_SETTINGFACT(alertEnabled)
    DEFINE_SETTINGFACT(alertSound)

public slots:
    void resetToDefaults();
    void validateSettings();

signals:
    void settingsChanged();
    void connectionSettingsChanged();
    void calibrationChanged();
};
