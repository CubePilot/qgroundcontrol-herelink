/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "UTGSettings.h"
#include "QGCApplication.h"

#include <QQmlEngine>
#include <QtQml>

DECLARE_SETTINGGROUP(UTG, "UTG")
{
    qmlRegisterUncreatableType<UTGSettings>("QGroundControl.SettingsManager", 1, 0, "UTGSettings", "Reference only");
}

// Serial Port Configuration
DECLARE_SETTINGSFACT(UTGSettings, enabled)
DECLARE_SETTINGSFACT(UTGSettings, serialPort)
DECLARE_SETTINGSFACT(UTGSettings, baudRate)
DECLARE_SETTINGSFACT(UTGSettings, dataBits)
DECLARE_SETTINGSFACT(UTGSettings, stopBits)
DECLARE_SETTINGSFACT(UTGSettings, parity)
DECLARE_SETTINGSFACT(UTGSettings, flowControl)

// UTG Operational Settings
DECLARE_SETTINGSFACT(UTGSettings, autoConnect)
DECLARE_SETTINGSFACT(UTGSettings, measurementUnit)
DECLARE_SETTINGSFACT(UTGSettings, measurementMode)
DECLARE_SETTINGSFACT(UTGSettings, soundVelocity)
DECLARE_SETTINGSFACT(UTGSettings, zeroOffset)
DECLARE_SETTINGSFACT(UTGSettings, gain)
DECLARE_SETTINGSFACT(UTGSettings, threshold)
DECLARE_SETTINGSFACT(UTGSettings, pulseWidth)
DECLARE_SETTINGSFACT(UTGSettings, frequency)

// Display and Logging Settings
DECLARE_SETTINGSFACT(UTGSettings, displayPrecision)
DECLARE_SETTINGSFACT(UTGSettings, logMeasurements)
DECLARE_SETTINGSFACT(UTGSettings, logFilePath)
DECLARE_SETTINGSFACT(UTGSettings, autoSaveMeasurements)

// Calibration Settings
DECLARE_SETTINGSFACT(UTGSettings, calibrationMode)
DECLARE_SETTINGSFACT(UTGSettings, calibrationValue)
DECLARE_SETTINGSFACT(UTGSettings, temperatureCompensation)
DECLARE_SETTINGSFACT(UTGSettings, materialType)

// Alert Settings
DECLARE_SETTINGSFACT(UTGSettings, minThicknessAlert)
DECLARE_SETTINGSFACT(UTGSettings, maxThicknessAlert)
DECLARE_SETTINGSFACT(UTGSettings, alertEnabled)
DECLARE_SETTINGSFACT(UTGSettings, alertSound)

void UTGSettings::resetToDefaults()
{
    // Reset all settings to their default values
    enabled()->setRawValue(enabled()->rawDefaultValue());
    serialPort()->setRawValue(serialPort()->rawDefaultValue());
    baudRate()->setRawValue(baudRate()->rawDefaultValue());
    dataBits()->setRawValue(dataBits()->rawDefaultValue());
    stopBits()->setRawValue(stopBits()->rawDefaultValue());
    parity()->setRawValue(parity()->rawDefaultValue());
    flowControl()->setRawValue(flowControl()->rawDefaultValue());
    
    autoConnect()->setRawValue(autoConnect()->rawDefaultValue());
    measurementUnit()->setRawValue(measurementUnit()->rawDefaultValue());
    measurementMode()->setRawValue(measurementMode()->rawDefaultValue());
    soundVelocity()->setRawValue(soundVelocity()->rawDefaultValue());
    zeroOffset()->setRawValue(zeroOffset()->rawDefaultValue());
    gain()->setRawValue(gain()->rawDefaultValue());
    threshold()->setRawValue(threshold()->rawDefaultValue());
    pulseWidth()->setRawValue(pulseWidth()->rawDefaultValue());
    frequency()->setRawValue(frequency()->rawDefaultValue());
    
    displayPrecision()->setRawValue(displayPrecision()->rawDefaultValue());
    logMeasurements()->setRawValue(logMeasurements()->rawDefaultValue());
    logFilePath()->setRawValue(logFilePath()->rawDefaultValue());
    autoSaveMeasurements()->setRawValue(autoSaveMeasurements()->rawDefaultValue());
    
    calibrationMode()->setRawValue(calibrationMode()->rawDefaultValue());
    calibrationValue()->setRawValue(calibrationValue()->rawDefaultValue());
    temperatureCompensation()->setRawValue(temperatureCompensation()->rawDefaultValue());
    materialType()->setRawValue(materialType()->rawDefaultValue());
    
    minThicknessAlert()->setRawValue(minThicknessAlert()->rawDefaultValue());
    maxThicknessAlert()->setRawValue(maxThicknessAlert()->rawDefaultValue());
    alertEnabled()->setRawValue(alertEnabled()->rawDefaultValue());
    alertSound()->setRawValue(alertSound()->rawDefaultValue());
    
    emit settingsChanged();
}

void UTGSettings::validateSettings()
{
    // Validate serial port settings
    if (baudRate()->rawValue().toInt() <= 0) {
        baudRate()->setRawValue(9600);
    }
    
    if (dataBits()->rawValue().toInt() < 5 || dataBits()->rawValue().toInt() > 8) {
        dataBits()->setRawValue(8);
    }
    
    if (stopBits()->rawValue().toInt() < 1 || stopBits()->rawValue().toInt() > 2) {
        stopBits()->setRawValue(1);
    }
    
    // Validate operational settings
    if (soundVelocity()->rawValue().toDouble() <= 0) {
        soundVelocity()->setRawValue(5920.0); // Default for steel
    }
    
    if (gain()->rawValue().toInt() < 0 || gain()->rawValue().toInt() > 100) {
        gain()->setRawValue(50);
    }
    
    if (threshold()->rawValue().toInt() < 0 || threshold()->rawValue().toInt() > 100) {
        threshold()->setRawValue(50);
    }
    
    // Validate alert settings
    if (minThicknessAlert()->rawValue().toDouble() < 0) {
        minThicknessAlert()->setRawValue(0.0);
    }
    
    if (maxThicknessAlert()->rawValue().toDouble() <= minThicknessAlert()->rawValue().toDouble()) {
        maxThicknessAlert()->setRawValue(minThicknessAlert()->rawValue().toDouble() + 10.0);
    }
    
    emit settingsChanged();
}
