/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "UTGSettingsTest.h"
#include "QGCApplication.h"
#include "MultiSignalSpy.h"

#include <QSignalSpy>

UTGSettingsTest::UTGSettingsTest(void)
    : _utgSettings(nullptr)
    , _settingsManager(nullptr)
{
    
}

void UTGSettingsTest::init(void)
{
    UnitTest::init();
    
    _settingsManager = qgcApp()->toolbox()->settingsManager();
    QVERIFY(_settingsManager);
    
    _utgSettings = _settingsManager->utgSettings();
    QVERIFY(_utgSettings);
}

void UTGSettingsTest::cleanup(void)
{
    UnitTest::cleanup();
}

void UTGSettingsTest::_testDefaultValues(void)
{
    _verifyDefaultValues();
}

void UTGSettingsTest::_verifyDefaultValues(void)
{
    // Test serial port defaults
    _testFactValue(_utgSettings->enabled(), false, "enabled");
    _testFactValue(_utgSettings->serialPort(), QString(""), "serialPort");
    _testFactValue(_utgSettings->baudRate(), 9600, "baudRate");
    _testFactValue(_utgSettings->dataBits(), 8, "dataBits");
    _testFactValue(_utgSettings->stopBits(), 1, "stopBits");
    _testFactValue(_utgSettings->parity(), 0, "parity");
    _testFactValue(_utgSettings->flowControl(), false, "flowControl");
    
    // Test operational defaults
    _testFactValue(_utgSettings->autoConnect(), false, "autoConnect");
    _testFactValue(_utgSettings->measurementUnit(), 0, "measurementUnit");
    _testFactValue(_utgSettings->measurementMode(), 0, "measurementMode");
    _testFactValue(_utgSettings->soundVelocity(), 5920.0, "soundVelocity");
    _testFactValue(_utgSettings->zeroOffset(), 0.0, "zeroOffset");
    _testFactValue(_utgSettings->gain(), 50, "gain");
    _testFactValue(_utgSettings->threshold(), 50, "threshold");
    _testFactValue(_utgSettings->pulseWidth(), 100, "pulseWidth");
    _testFactValue(_utgSettings->frequency(), 5.0, "frequency");
    
    // Test display and logging defaults
    _testFactValue(_utgSettings->displayPrecision(), 2, "displayPrecision");
    _testFactValue(_utgSettings->logMeasurements(), true, "logMeasurements");
    _testFactValue(_utgSettings->logFilePath(), QString(""), "logFilePath");
    _testFactValue(_utgSettings->autoSaveMeasurements(), false, "autoSaveMeasurements");
    
    // Test calibration defaults
    _testFactValue(_utgSettings->calibrationMode(), 0, "calibrationMode");
    _testFactValue(_utgSettings->calibrationValue(), 10.0, "calibrationValue");
    _testFactValue(_utgSettings->temperatureCompensation(), false, "temperatureCompensation");
    _testFactValue(_utgSettings->materialType(), 0, "materialType");
    
    // Test alert defaults
    _testFactValue(_utgSettings->minThicknessAlert(), 0.0, "minThicknessAlert");
    _testFactValue(_utgSettings->maxThicknessAlert(), 100.0, "maxThicknessAlert");
    _testFactValue(_utgSettings->alertEnabled(), false, "alertEnabled");
    _testFactValue(_utgSettings->alertSound(), true, "alertSound");
}

void UTGSettingsTest::_testSettingsPersistence(void)
{
    // Test that settings persist across application restarts
    const double testVelocity = 6420.0; // Aluminum velocity
    const int testGain = 75;
    const bool testEnabled = true;
    
    // Set test values
    _utgSettings->soundVelocity()->setRawValue(testVelocity);
    _utgSettings->gain()->setRawValue(testGain);
    _utgSettings->enabled()->setRawValue(testEnabled);
    
    // Verify values are set
    QCOMPARE(_utgSettings->soundVelocity()->rawValue().toDouble(), testVelocity);
    QCOMPARE(_utgSettings->gain()->rawValue().toInt(), testGain);
    QCOMPARE(_utgSettings->enabled()->rawValue().toBool(), testEnabled);
    
    // Note: Full persistence testing would require application restart simulation
    // which is complex in unit tests. This tests the immediate setting/getting.
}

void UTGSettingsTest::_testSerialPortSettings(void)
{
    // Test baud rate values
    QList<int> validBaudRates = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
    for (int baudRate : validBaudRates) {
        _utgSettings->baudRate()->setRawValue(baudRate);
        QCOMPARE(_utgSettings->baudRate()->rawValue().toInt(), baudRate);
    }
    
    // Test data bits
    for (int dataBits = 5; dataBits <= 8; dataBits++) {
        _utgSettings->dataBits()->setRawValue(dataBits);
        QCOMPARE(_utgSettings->dataBits()->rawValue().toInt(), dataBits);
    }
    
    // Test stop bits
    _utgSettings->stopBits()->setRawValue(1);
    QCOMPARE(_utgSettings->stopBits()->rawValue().toInt(), 1);
    _utgSettings->stopBits()->setRawValue(2);
    QCOMPARE(_utgSettings->stopBits()->rawValue().toInt(), 2);
    
    // Test parity values (None=0, Even=2, Odd=3)
    QList<int> validParity = {0, 2, 3};
    for (int parity : validParity) {
        _utgSettings->parity()->setRawValue(parity);
        QCOMPARE(_utgSettings->parity()->rawValue().toInt(), parity);
    }
    
    // Test flow control
    _utgSettings->flowControl()->setRawValue(true);
    QCOMPARE(_utgSettings->flowControl()->rawValue().toBool(), true);
    _utgSettings->flowControl()->setRawValue(false);
    QCOMPARE(_utgSettings->flowControl()->rawValue().toBool(), false);
}

void UTGSettingsTest::_testMeasurementSettings(void)
{
    // Test measurement units (mm=0, inch=1, mil=2)
    for (int unit = 0; unit <= 2; unit++) {
        _utgSettings->measurementUnit()->setRawValue(unit);
        QCOMPARE(_utgSettings->measurementUnit()->rawValue().toInt(), unit);
    }
    
    // Test measurement modes (Pulse-Echo=0, Through-Transmission=1, Echo-Echo=2)
    for (int mode = 0; mode <= 2; mode++) {
        _utgSettings->measurementMode()->setRawValue(mode);
        QCOMPARE(_utgSettings->measurementMode()->rawValue().toInt(), mode);
    }
    
    // Test sound velocity range
    _testFactRange(_utgSettings->soundVelocity(), 1000.0, 15000.0, "soundVelocity");
    
    // Test zero offset range
    _testFactRange(_utgSettings->zeroOffset(), -10.0, 10.0, "zeroOffset");
    
    // Test gain range
    _testFactRange(_utgSettings->gain(), 0, 100, "gain");
    
    // Test threshold range
    _testFactRange(_utgSettings->threshold(), 0, 100, "threshold");
    
    // Test pulse width range
    _testFactRange(_utgSettings->pulseWidth(), 50, 1000, "pulseWidth");
    
    // Test frequency range
    _testFactRange(_utgSettings->frequency(), 1.0, 20.0, "frequency");
}

void UTGSettingsTest::_testCalibrationSettings(void)
{
    // Test calibration modes (One Point=0, Two Point=1, Velocity=2)
    for (int mode = 0; mode <= 2; mode++) {
        _utgSettings->calibrationMode()->setRawValue(mode);
        QCOMPARE(_utgSettings->calibrationMode()->rawValue().toInt(), mode);
    }
    
    // Test calibration value range
    _testFactRange(_utgSettings->calibrationValue(), 0.1, 1000.0, "calibrationValue");
    
    // Test material types (Steel=0, Aluminum=1, Copper=2, Plastic=3, Custom=4)
    for (int material = 0; material <= 4; material++) {
        _utgSettings->materialType()->setRawValue(material);
        QCOMPARE(_utgSettings->materialType()->rawValue().toInt(), material);
    }
    
    // Test temperature compensation
    _utgSettings->temperatureCompensation()->setRawValue(true);
    QCOMPARE(_utgSettings->temperatureCompensation()->rawValue().toBool(), true);
    _utgSettings->temperatureCompensation()->setRawValue(false);
    QCOMPARE(_utgSettings->temperatureCompensation()->rawValue().toBool(), false);
}

void UTGSettingsTest::_testAlertSettings(void)
{
    // Test alert thresholds
    _testFactRange(_utgSettings->minThicknessAlert(), 0.0, 1000.0, "minThicknessAlert");
    _testFactRange(_utgSettings->maxThicknessAlert(), 0.0, 1000.0, "maxThicknessAlert");
    
    // Test alert enabled
    _utgSettings->alertEnabled()->setRawValue(true);
    QCOMPARE(_utgSettings->alertEnabled()->rawValue().toBool(), true);
    _utgSettings->alertEnabled()->setRawValue(false);
    QCOMPARE(_utgSettings->alertEnabled()->rawValue().toBool(), false);
    
    // Test alert sound
    _utgSettings->alertSound()->setRawValue(true);
    QCOMPARE(_utgSettings->alertSound()->rawValue().toBool(), true);
    _utgSettings->alertSound()->setRawValue(false);
    QCOMPARE(_utgSettings->alertSound()->rawValue().toBool(), false);
}

void UTGSettingsTest::_testValidation(void)
{
    // Test validation method
    _utgSettings->validateSettings();
    
    // Set invalid values and test validation
    _utgSettings->baudRate()->setRawValue(-1);
    _utgSettings->validateSettings();
    QCOMPARE(_utgSettings->baudRate()->rawValue().toInt(), 9600); // Should reset to default
    
    _utgSettings->soundVelocity()->setRawValue(-100.0);
    _utgSettings->validateSettings();
    QCOMPARE(_utgSettings->soundVelocity()->rawValue().toDouble(), 5920.0); // Should reset to default
    
    _utgSettings->gain()->setRawValue(150);
    _utgSettings->validateSettings();
    QCOMPARE(_utgSettings->gain()->rawValue().toInt(), 50); // Should reset to valid range
}

void UTGSettingsTest::_testResetToDefaults(void)
{
    // Change some values
    _utgSettings->enabled()->setRawValue(true);
    _utgSettings->soundVelocity()->setRawValue(6420.0);
    _utgSettings->gain()->setRawValue(75);
    _utgSettings->alertEnabled()->setRawValue(true);
    
    // Reset to defaults
    _utgSettings->resetToDefaults();
    
    // Verify all values are back to defaults
    _verifyDefaultValues();
}

void UTGSettingsTest::_testSettingsSignals(void)
{
    QSignalSpy settingsChangedSpy(_utgSettings, SIGNAL(settingsChanged()));
    QSignalSpy connectionSettingsChangedSpy(_utgSettings, SIGNAL(connectionSettingsChanged()));
    QSignalSpy calibrationChangedSpy(_utgSettings, SIGNAL(calibrationChanged()));
    
    // Test settings changed signal
    _utgSettings->resetToDefaults();
    QCOMPARE(settingsChangedSpy.count(), 1);
    
    // Note: Connection and calibration signals would be tested with more complex scenarios
    // involving the actual UTGManager integration
}

void UTGSettingsTest::_testBoundaryValues(void)
{
    // Test minimum values
    _utgSettings->soundVelocity()->setRawValue(1000.0);
    QCOMPARE(_utgSettings->soundVelocity()->rawValue().toDouble(), 1000.0);
    
    // Test maximum values
    _utgSettings->soundVelocity()->setRawValue(15000.0);
    QCOMPARE(_utgSettings->soundVelocity()->rawValue().toDouble(), 15000.0);
    
    // Test precision
    _utgSettings->displayPrecision()->setRawValue(0);
    QCOMPARE(_utgSettings->displayPrecision()->rawValue().toInt(), 0);
    _utgSettings->displayPrecision()->setRawValue(4);
    QCOMPARE(_utgSettings->displayPrecision()->rawValue().toInt(), 4);
}

void UTGSettingsTest::_testMaterialPresets(void)
{
    // Test that material type changes work correctly
    // This would typically involve testing the sound velocity presets
    // but since that logic might be in UTGManager, we just test the setting
    
    _utgSettings->materialType()->setRawValue(0); // Steel
    QCOMPARE(_utgSettings->materialType()->rawValue().toInt(), 0);
    
    _utgSettings->materialType()->setRawValue(1); // Aluminum
    QCOMPARE(_utgSettings->materialType()->rawValue().toInt(), 1);
    
    _utgSettings->materialType()->setRawValue(4); // Custom
    QCOMPARE(_utgSettings->materialType()->rawValue().toInt(), 4);
}

void UTGSettingsTest::_testFactValue(Fact* fact, QVariant expectedValue, const QString& factName)
{
    QVERIFY2(fact != nullptr, qPrintable(QString("Fact %1 is null").arg(factName)));
    QCOMPARE(fact->rawValue(), expectedValue);
}

void UTGSettingsTest::_testFactRange(Fact* fact, QVariant minValue, QVariant maxValue, const QString& factName)
{
    QVERIFY2(fact != nullptr, qPrintable(QString("Fact %1 is null").arg(factName)));
    
    // Test minimum value
    fact->setRawValue(minValue);
    QCOMPARE(fact->rawValue(), minValue);
    
    // Test maximum value
    fact->setRawValue(maxValue);
    QCOMPARE(fact->rawValue(), maxValue);
    
    // Test middle value
    if (minValue.type() == QVariant::Double || maxValue.type() == QVariant::Double) {
        double midValue = (minValue.toDouble() + maxValue.toDouble()) / 2.0;
        fact->setRawValue(midValue);
        QCOMPARE(fact->rawValue().toDouble(), midValue);
    } else {
        int midValue = (minValue.toInt() + maxValue.toInt()) / 2;
        fact->setRawValue(midValue);
        QCOMPARE(fact->rawValue().toInt(), midValue);
    }
}
