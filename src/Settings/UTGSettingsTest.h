/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include "UnitTest.h"
#include "UTGSettings.h"
#include "SettingsManager.h"

/// Unit test for UTGSettings
class UTGSettingsTest : public UnitTest
{
    Q_OBJECT
    
public:
    UTGSettingsTest(void);

    void init(void) override;
    void cleanup(void) override;

private slots:
    void _testDefaultValues(void);
    void _testSettingsPersistence(void);
    void _testSerialPortSettings(void);
    void _testMeasurementSettings(void);
    void _testCalibrationSettings(void);
    void _testAlertSettings(void);
    void _testValidation(void);
    void _testResetToDefaults(void);
    void _testSettingsSignals(void);
    void _testBoundaryValues(void);
    void _testMaterialPresets(void);

private:
    UTGSettings* _utgSettings;
    SettingsManager* _settingsManager;
    
    void _verifyDefaultValues(void);
    void _testFactValue(Fact* fact, QVariant expectedValue, const QString& factName);
    void _testFactRange(Fact* fact, QVariant minValue, QVariant maxValue, const QString& factName);
};
