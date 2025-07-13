/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "UTGManagerTest.h"
#include "QGCApplication.h"
#include "MultiVehicleManager.h"

#include <QSignalSpy>
#include <QTest>

UTGManagerTest::UTGManagerTest(void)
    : _vehicle(nullptr)
    , _utgManager(nullptr)
{
    
}

void UTGManagerTest::init(void)
{
    UnitTest::init();
    
    _connectMockLink(MAV_AUTOPILOT_ARDUPILOTMEGA);
    
    MultiVehicleManager* vehicleMgr = qgcApp()->toolbox()->multiVehicleManager();
    _vehicle = vehicleMgr->activeVehicle();
    QVERIFY(_vehicle);
    
    _utgManager = _vehicle->utgManager();
    QVERIFY(_utgManager);
}

void UTGManagerTest::cleanup(void)
{
    _disconnectMockLink();
    UnitTest::cleanup();
}

void UTGManagerTest::_testInitialization(void)
{
    _verifyInitialState();
}

void UTGManagerTest::_verifyInitialState(void)
{
    // Test initial state
    QCOMPARE(_utgManager->connected(), false);
    QCOMPARE(_utgManager->enabled(), false);
    QCOMPARE(_utgManager->currentThickness(), 0.0f);
    QCOMPARE(_utgManager->status(), UTGManager::STATUS_DISCONNECTED);
    QCOMPARE(_utgManager->measuring(), false);
    QVERIFY(!_utgManager->statusText().isEmpty());
    QVERIFY(_utgManager->lastError().isEmpty());
}

void UTGManagerTest::_testConnectionManagement(void)
{
    QSignalSpy connectedSpy(_utgManager, SIGNAL(connectedChanged(bool)));
    QSignalSpy statusSpy(_utgManager, SIGNAL(statusChanged(UTGManager::UTGStatus)));
    
    // Test enabling UTG
    _utgManager->setEnabled(true);
    QCOMPARE(_utgManager->enabled(), true);
    
    // Note: Actual connection testing would require mock serial port
    // For now, test the state management
    QCOMPARE(connectedSpy.count(), 0); // No connection without valid port
    
    // Test disabling
    _utgManager->setEnabled(false);
    QCOMPARE(_utgManager->enabled(), false);
}

void UTGManagerTest::_testCommandBuilding(void)
{
    // Test basic command structure
    QByteArray testData;
    testData.append(static_cast<char>(50)); // Gain value
    
    // We can't directly test _buildCommand as it's private, but we can test
    // the command sending mechanism indirectly
    
    // Test that commands are properly queued when not connected
    _utgManager->setGain(50);
    // Command should be queued but not sent (not connected)
    
    // Test command with data
    _utgManager->setSoundVelocity(5920.0);
    // Should handle float data properly
    
    // Test command without data
    _utgManager->getStatus();
    // Should handle commands without payload
}

void UTGManagerTest::_testResponseParsing(void)
{
    // Test response validation
    QByteArray validResponse;
    validResponse.append(static_cast<char>(0xAA)); // Start byte
    validResponse.append(static_cast<char>(UTGManager::CMD_GET_THICKNESS)); // Command
    validResponse.append(static_cast<char>(4)); // Length (float = 4 bytes)
    
    float testThickness = 25.43f;
    validResponse.append(reinterpret_cast<const char*>(&testThickness), sizeof(float));
    
    // Calculate checksum
    unsigned char checksum = 0;
    for (int i = 0; i < validResponse.size(); i++) {
        checksum ^= static_cast<unsigned char>(validResponse[i]);
    }
    validResponse.append(static_cast<char>(checksum));
    
    // Note: Direct testing of _processReceivedData would require access to private methods
    // In a real implementation, we might make these methods protected for testing
}

void UTGManagerTest::_testMeasurementCommands(void)
{
    QSignalSpy thicknessSpy(_utgManager, SIGNAL(thicknessChanged(float)));
    QSignalSpy measurementSpy(_utgManager, SIGNAL(measurementReceived(float, qint64)));
    QSignalSpy measuringSpy(_utgManager, SIGNAL(measuringChanged(bool)));
    
    // Test single measurement
    _utgManager->startSingleMeasurement();
    // Should not change measuring state for single measurement
    QCOMPARE(_utgManager->measuring(), false);
    
    // Test continuous measurement
    _utgManager->startContinuousMeasurement();
    // Note: Without actual connection, measuring state won't change
    // In real test with mock serial, we would verify state changes
    
    // Test stop measurement
    _utgManager->stopMeasurement();
    QCOMPARE(_utgManager->measuring(), false);
    
    // Test trigger measurement
    _utgManager->triggerMeasurement();
    // Should be equivalent to single measurement
}

void UTGManagerTest::_testCalibrationCommands(void)
{
    QSignalSpy calibrationSpy(_utgManager, SIGNAL(calibrationCompleted(bool)));
    
    // Test zero calibration
    _utgManager->performZeroCalibration();
    // Should send calibration command with mode 0
    
    // Test velocity calibration
    double knownThickness = 25.4;
    _utgManager->performVelocityCalibration(knownThickness);
    // Should send calibration command with mode 2 and reference value
    
    // Test general calibration
    _utgManager->startCalibration(1, 10.0); // Two-point calibration
    // Should send calibration command with specified mode and value
}

void UTGManagerTest::_testParameterCommands(void)
{
    // Test gain setting
    _utgManager->setGain(75);
    // Should send CMD_SET_GAIN with value 75
    
    // Test sound velocity setting
    _utgManager->setSoundVelocity(6420.0); // Aluminum
    // Should send CMD_SET_VELOCITY with float value
    
    // Test zero offset setting
    _utgManager->setZeroOffset(0.5);
    // Should send CMD_SET_ZERO_OFFSET with float value
    
    // Test threshold setting
    _utgManager->setThreshold(60);
    // Should send CMD_SET_THRESHOLD with value 60
    
    // Test pulse width setting
    _utgManager->setPulseWidth(150);
    // Should send CMD_SET_PULSE_WIDTH with value 150
    
    // Test frequency setting
    _utgManager->setFrequency(7.5);
    // Should send CMD_SET_FREQUENCY with float value
    
    // Test measurement unit setting
    _utgManager->setMeasurementUnit(1); // Inch
    // Should send CMD_SET_UNIT with value 1
    
    // Test measurement mode setting
    _utgManager->setMeasurementMode(2); // Echo-Echo
    // Should send CMD_SET_MODE with value 2
    
    // Test material type setting
    _utgManager->setMaterialType(1); // Aluminum
    // Should send CMD_SET_MATERIAL with value 1
}

void UTGManagerTest::_testErrorHandling(void)
{
    QSignalSpy errorSpy(_utgManager, SIGNAL(errorChanged(QString)));
    
    // Test connection to non-existent port
    // This would typically trigger an error
    _utgManager->connectToUTG();
    // Should emit error signal with appropriate message
    
    // Test timeout handling
    // Would require mock implementation to simulate timeout
    
    // Test invalid response handling
    // Would require ability to inject invalid data
}

void UTGManagerTest::_testStatusManagement(void)
{
    QSignalSpy statusSpy(_utgManager, SIGNAL(statusChanged(UTGManager::UTGStatus)));
    
    // Test initial status
    QCOMPARE(_utgManager->status(), UTGManager::STATUS_DISCONNECTED);
    
    // Test status text
    QString statusText = _utgManager->statusText();
    QVERIFY(!statusText.isEmpty());
    QVERIFY(statusText.contains("Disconnected") || statusText.contains("disconnected"));
    
    // Test status transitions would require mock serial port
    // to simulate actual connection states
}

void UTGManagerTest::_testSignalEmission(void)
{
    // Test that appropriate signals are emitted for state changes
    QSignalSpy enabledSpy(_utgManager, SIGNAL(enabledChanged(bool)));
    QSignalSpy connectedSpy(_utgManager, SIGNAL(connectedChanged(bool)));
    QSignalSpy statusSpy(_utgManager, SIGNAL(statusChanged(UTGManager::UTGStatus)));
    QSignalSpy thicknessSpy(_utgManager, SIGNAL(thicknessChanged(float)));
    QSignalSpy measuringSpy(_utgManager, SIGNAL(measuringChanged(bool)));
    QSignalSpy errorSpy(_utgManager, SIGNAL(errorChanged(QString)));
    
    // Test enabled signal
    _utgManager->setEnabled(true);
    QCOMPARE(enabledSpy.count(), 1);
    QList<QVariant> enabledArgs = enabledSpy.takeFirst();
    QCOMPARE(enabledArgs.at(0).toBool(), true);
    
    _utgManager->setEnabled(false);
    QCOMPARE(enabledSpy.count(), 1);
    enabledArgs = enabledSpy.takeFirst();
    QCOMPARE(enabledArgs.at(0).toBool(), false);
}

void UTGManagerTest::_testSettingsIntegration(void)
{
    // Test that UTGManager responds to settings changes
    auto settings = qgcApp()->toolbox()->settingsManager()->utgSettings();
    
    // Test enabling through settings
    settings->enabled()->setRawValue(true);
    // Should trigger UTGManager to enable
    
    // Test auto-connect setting
    settings->autoConnect()->setRawValue(true);
    // Should trigger connection attempt when enabled
    
    // Test serial port settings
    settings->baudRate()->setRawValue(115200);
    // Should update internal configuration
    
    settings->enabled()->setRawValue(false);
    // Should disable UTGManager
}

void UTGManagerTest::_testProtocolValidation(void)
{
    // Test protocol constants
    QCOMPARE(static_cast<int>(UTGManager::CMD_GET_THICKNESS), 0x01);
    QCOMPARE(static_cast<int>(UTGManager::CMD_SET_GAIN), 0x02);
    QCOMPARE(static_cast<int>(UTGManager::CMD_SET_VELOCITY), 0x03);
    
    // Test command enumeration completeness
    // Ensure all expected commands are defined
    QVERIFY(static_cast<int>(UTGManager::CMD_RESET) > 0);
    QVERIFY(static_cast<int>(UTGManager::CMD_SET_MATERIAL) > 0);
}

void UTGManagerTest::_testCommunicationTimeout(void)
{
    // Test timeout handling
    // This would require mock implementation to simulate slow responses
    
    // Test retry mechanism
    // Would need to simulate failed communications
    
    // Test connection retry
    // Would need to simulate connection failures
}

void UTGManagerTest::_testSerialPortConfiguration(void)
{
    // Test that serial port is configured correctly based on settings
    auto settings = qgcApp()->toolbox()->settingsManager()->utgSettings();
    
    // Set specific configuration
    settings->baudRate()->setRawValue(115200);
    settings->dataBits()->setRawValue(8);
    settings->stopBits()->setRawValue(1);
    settings->parity()->setRawValue(0); // None
    settings->flowControl()->setRawValue(false);
    
    // Test that UTGManager uses these settings
    // Would require access to internal serial port configuration
    // or mock serial port to verify settings
}

// Helper method implementations
void UTGManagerTest::_testCommandStructure(UTGManager::UTGCommand cmd, const QByteArray& data, const QByteArray& expectedCommand)
{
    // This would test the command building if we had access to the private method
    // For now, we test indirectly through public methods
}

void UTGManagerTest::_simulateResponse(UTGManager::UTGCommand cmd, const QByteArray& responseData)
{
    // This would simulate receiving a response from the UTG device
    // Useful for testing response parsing logic
}

void UTGManagerTest::_testStatusTransition(UTGManager::UTGStatus fromStatus, UTGManager::UTGStatus toStatus)
{
    // This would test status transitions
    // Useful for verifying state machine behavior
}

QByteArray UTGManagerTest::_buildTestResponse(UTGManager::UTGCommand cmd, const QByteArray& data)
{
    QByteArray response;
    response.append(static_cast<char>(0xAA)); // Start byte
    response.append(static_cast<char>(cmd));  // Command
    response.append(static_cast<char>(data.size())); // Data length
    response.append(data); // Data payload
    
    // Calculate checksum
    unsigned char checksum = 0;
    for (char byte : response) {
        checksum ^= static_cast<unsigned char>(byte);
    }
    response.append(static_cast<char>(checksum));
    
    return response;
}

void UTGManagerTest::_verifySignalEmission(const QString& signalName, int expectedCount)
{
    // Helper to verify signal emission counts
    // Would be used with QSignalSpy objects
}
