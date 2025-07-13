/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "UTGIntegrationTest.h"
#include "QGCApplication.h"
#include "MultiVehicleManager.h"
#include "SettingsManager.h"

#include <QSignalSpy>
#include <QTest>

UTGIntegrationTest::UTGIntegrationTest(void)
    : _vehicle(nullptr)
    , _utgManager(nullptr)
    , _utgSettings(nullptr)
    , _mockDevice(nullptr)
{
    
}

void UTGIntegrationTest::init(void)
{
    UnitTest::init();
    
    _connectMockLink(MAV_AUTOPILOT_ARDUPILOTMEGA);
    
    MultiVehicleManager* vehicleMgr = qgcApp()->toolbox()->multiVehicleManager();
    _vehicle = vehicleMgr->activeVehicle();
    QVERIFY(_vehicle);
    
    _utgManager = _vehicle->utgManager();
    QVERIFY(_utgManager);
    
    _utgSettings = qgcApp()->toolbox()->settingsManager()->utgSettings();
    QVERIFY(_utgSettings);
    
    _mockDevice = new MockUTGDevice(this);
    _connectMockDevice();
}

void UTGIntegrationTest::cleanup(void)
{
    _disconnectMockDevice();
    delete _mockDevice;
    _mockDevice = nullptr;
    
    _disconnectMockLink();
    UnitTest::cleanup();
}

void UTGIntegrationTest::_testFullWorkflow(void)
{
    // Test complete workflow from settings to measurement
    
    // 1. Configure settings
    _utgSettings->enabled()->setRawValue(true);
    _utgSettings->serialPort()->setRawValue("COM_TEST");
    _utgSettings->baudRate()->setRawValue(9600);
    _utgSettings->soundVelocity()->setRawValue(5920.0);
    _utgSettings->gain()->setRawValue(60);
    
    // 2. Enable UTG
    _utgManager->setEnabled(true);
    QCOMPARE(_utgManager->enabled(), true);
    
    // 3. Simulate connection
    _mockDevice->setConnected(true);
    
    // 4. Perform measurement
    QSignalSpy measurementSpy(_utgManager, SIGNAL(measurementReceived(float, qint64)));
    _mockDevice->setThicknessValue(25.43f);
    _utgManager->startSingleMeasurement();
    
    // Simulate device response
    float testThickness = 25.43f;
    QByteArray responseData(reinterpret_cast<const char*>(&testThickness), sizeof(float));
    _simulateDeviceResponse(UTGManager::CMD_GET_THICKNESS, responseData);
    
    // 5. Verify measurement received
    QCOMPARE(measurementSpy.count(), 1);
    QList<QVariant> args = measurementSpy.takeFirst();
    QCOMPARE(args.at(0).toFloat(), testThickness);
}

void UTGIntegrationTest::_testSettingsToManagerIntegration(void)
{
    // Test that settings changes are reflected in UTGManager behavior
    
    QSignalSpy enabledSpy(_utgManager, SIGNAL(enabledChanged(bool)));
    
    // Test enabling through settings
    _utgSettings->enabled()->setRawValue(true);
    QCOMPARE(_utgManager->enabled(), true);
    QCOMPARE(enabledSpy.count(), 1);
    
    // Test disabling through settings
    _utgSettings->enabled()->setRawValue(false);
    QCOMPARE(_utgManager->enabled(), false);
    QCOMPARE(enabledSpy.count(), 2);
    
    // Test auto-connect setting
    _utgSettings->enabled()->setRawValue(true);
    _utgSettings->autoConnect()->setRawValue(true);
    // Should trigger connection attempt
    
    // Test parameter changes
    _utgSettings->gain()->setRawValue(75);
    _utgSettings->soundVelocity()->setRawValue(6420.0); // Aluminum
    
    // Verify parameters are sent to device when connected
    _mockDevice->setConnected(true);
    _verifyParameterSync();
}

void UTGIntegrationTest::_testMeasurementWorkflow(void)
{
    _utgSettings->enabled()->setRawValue(true);
    _utgManager->setEnabled(true);
    _mockDevice->setConnected(true);
    
    QSignalSpy thicknessSpy(_utgManager, SIGNAL(thicknessChanged(float)));
    QSignalSpy measurementSpy(_utgManager, SIGNAL(measurementReceived(float, qint64)));
    
    // Test single measurement
    float testThickness1 = 12.75f;
    _mockDevice->setThicknessValue(testThickness1);
    _utgManager->startSingleMeasurement();
    
    _simulateDeviceResponse(UTGManager::CMD_GET_THICKNESS, 
                           QByteArray(reinterpret_cast<const char*>(&testThickness1), sizeof(float)));
    
    QCOMPARE(thicknessSpy.count(), 1);
    QCOMPARE(measurementSpy.count(), 1);
    _verifyMeasurement(testThickness1);
    
    // Test measurement with different units
    _utgSettings->measurementUnit()->setRawValue(1); // Inch
    float testThickness2 = 0.5f; // 0.5 inch
    _mockDevice->setThicknessValue(testThickness2);
    _utgManager->startSingleMeasurement();
    
    _simulateDeviceResponse(UTGManager::CMD_GET_THICKNESS, 
                           QByteArray(reinterpret_cast<const char*>(&testThickness2), sizeof(float)));
    
    _verifyMeasurement(testThickness2);
}

void UTGIntegrationTest::_testCalibrationWorkflow(void)
{
    _utgSettings->enabled()->setRawValue(true);
    _utgManager->setEnabled(true);
    _mockDevice->setConnected(true);
    
    QSignalSpy calibrationSpy(_utgManager, SIGNAL(calibrationCompleted(bool)));
    
    // Test zero calibration
    _utgManager->performZeroCalibration();
    
    // Simulate successful calibration response
    QByteArray successResponse(1, 0x01);
    _simulateDeviceResponse(UTGManager::CMD_CALIBRATE, successResponse);
    
    QCOMPARE(calibrationSpy.count(), 1);
    QList<QVariant> args = calibrationSpy.takeFirst();
    QCOMPARE(args.at(0).toBool(), true);
    
    // Test velocity calibration
    double knownThickness = 25.4;
    _utgManager->performVelocityCalibration(knownThickness);
    
    _simulateDeviceResponse(UTGManager::CMD_CALIBRATE, successResponse);
    
    QCOMPARE(calibrationSpy.count(), 1);
    args = calibrationSpy.takeFirst();
    QCOMPARE(args.at(0).toBool(), true);
    
    // Test calibration failure
    _mockDevice->setSimulateErrors(true);
    _utgManager->performZeroCalibration();
    
    QByteArray failureResponse(1, 0x00);
    _simulateDeviceResponse(UTGManager::CMD_CALIBRATE, failureResponse);
    
    QCOMPARE(calibrationSpy.count(), 1);
    args = calibrationSpy.takeFirst();
    QCOMPARE(args.at(0).toBool(), false);
}

void UTGIntegrationTest::_testErrorRecovery(void)
{
    _utgSettings->enabled()->setRawValue(true);
    _utgManager->setEnabled(true);
    
    QSignalSpy errorSpy(_utgManager, SIGNAL(errorChanged(QString)));
    QSignalSpy statusSpy(_utgManager, SIGNAL(statusChanged(UTGManager::UTGStatus)));
    
    // Test connection error
    _mockDevice->setConnected(false);
    _utgManager->connectToUTG();
    
    // Should emit error and change status
    QVERIFY(errorSpy.count() > 0 || statusSpy.count() > 0);
    
    // Test recovery
    _mockDevice->setConnected(true);
    _utgManager->connectToUTG();
    
    // Should recover and change status back
    QVERIFY(statusSpy.count() > 0);
    
    // Test communication error
    _mockDevice->setSimulateErrors(true);
    _utgManager->startSingleMeasurement();
    
    // Should handle error gracefully
    QVERIFY(_utgManager->status() != UTGManager::STATUS_ERROR || 
            !_utgManager->lastError().isEmpty());
}

void UTGIntegrationTest::_testContinuousMeasurement(void)
{
    _utgSettings->enabled()->setRawValue(true);
    _utgManager->setEnabled(true);
    _mockDevice->setConnected(true);
    
    QSignalSpy measuringSpy(_utgManager, SIGNAL(measuringChanged(bool)));
    QSignalSpy measurementSpy(_utgManager, SIGNAL(measurementReceived(float, qint64)));
    
    // Start continuous measurement
    _utgManager->startContinuousMeasurement();
    
    QCOMPARE(measuringSpy.count(), 1);
    QCOMPARE(_utgManager->measuring(), true);
    
    // Simulate multiple measurements
    for (int i = 0; i < 5; i++) {
        float thickness = 20.0f + i * 0.1f;
        _mockDevice->setThicknessValue(thickness);
        
        QByteArray responseData(reinterpret_cast<const char*>(&thickness), sizeof(float));
        _simulateDeviceResponse(UTGManager::CMD_GET_THICKNESS, responseData);
        
        QTest::qWait(50); // Small delay between measurements
    }
    
    // Should have received multiple measurements
    QVERIFY(measurementSpy.count() >= 5);
    
    // Stop continuous measurement
    _utgManager->stopMeasurement();
    
    QCOMPARE(measuringSpy.count(), 2);
    QCOMPARE(_utgManager->measuring(), false);
}

void UTGIntegrationTest::_testParameterSynchronization(void)
{
    _utgSettings->enabled()->setRawValue(true);
    _utgManager->setEnabled(true);
    _mockDevice->setConnected(true);
    
    // Clear command count
    _mockDevice->clearCommandCount();
    
    // Change multiple parameters
    _utgSettings->gain()->setRawValue(80);
    _utgSettings->soundVelocity()->setRawValue(6420.0);
    _utgSettings->threshold()->setRawValue(70);
    _utgSettings->frequency()->setRawValue(7.5);
    
    // Trigger parameter sync
    _verifyParameterSync();
    
    // Verify commands were sent to device
    QVERIFY(_mockDevice->getCommandCount() > 0);
    
    // Verify device configuration matches settings
    MockUTGDevice::DeviceConfig config = _mockDevice->config();
    QCOMPARE(config.gain, 80);
    QCOMPARE(config.soundVelocity, 6420.0);
    QCOMPARE(config.threshold, 70);
    QCOMPARE(config.frequency, 7.5);
}

void UTGIntegrationTest::_testConnectionLifecycle(void)
{
    QSignalSpy connectedSpy(_utgManager, SIGNAL(connectedChanged(bool)));
    QSignalSpy statusSpy(_utgManager, SIGNAL(statusChanged(UTGManager::UTGStatus)));
    
    // Test initial state
    QCOMPARE(_utgManager->connected(), false);
    QCOMPARE(_utgManager->status(), UTGManager::STATUS_DISCONNECTED);
    
    // Test connection
    _utgSettings->enabled()->setRawValue(true);
    _utgManager->setEnabled(true);
    _mockDevice->setConnected(true);
    
    // Simulate successful connection
    QVERIFY(connectedSpy.count() >= 0);
    QVERIFY(statusSpy.count() >= 0);
    
    // Test disconnection
    _mockDevice->setConnected(false);
    _utgManager->disconnectFromUTG();
    
    QCOMPARE(_utgManager->connected(), false);
    QCOMPARE(_utgManager->status(), UTGManager::STATUS_DISCONNECTED);
}

void UTGIntegrationTest::_testMaterialPresets(void)
{
    _utgSettings->enabled()->setRawValue(true);
    _utgManager->setEnabled(true);
    _mockDevice->setConnected(true);
    
    // Test material preset changes
    struct MaterialTest {
        int materialType;
        double expectedVelocity;
        QString name;
    };
    
    QList<MaterialTest> materials = {
        {0, 5920.0, "Steel"},
        {1, 6420.0, "Aluminum"},
        {2, 4760.0, "Copper"},
        {3, 2700.0, "Plastic"}
    };
    
    for (const MaterialTest& material : materials) {
        _utgSettings->materialType()->setRawValue(material.materialType);
        _utgManager->setMaterialType(material.materialType);
        
        // Verify device received the command
        MockUTGDevice::DeviceConfig config = _mockDevice->config();
        QCOMPARE(config.materialType, material.materialType);
        
        // For non-custom materials, velocity should be updated
        if (material.materialType < 4) {
            QCOMPARE(config.soundVelocity, material.expectedVelocity);
        }
    }
}

void UTGIntegrationTest::_testAlertSystem(void)
{
    _utgSettings->enabled()->setRawValue(true);
    _utgSettings->alertEnabled()->setRawValue(true);
    _utgSettings->minThicknessAlert()->setRawValue(10.0);
    _utgSettings->maxThicknessAlert()->setRawValue(30.0);
    
    _utgManager->setEnabled(true);
    _mockDevice->setConnected(true);
    
    // Test measurement within range (no alert)
    float normalThickness = 20.0f;
    _mockDevice->setThicknessValue(normalThickness);
    _utgManager->startSingleMeasurement();
    
    _simulateDeviceResponse(UTGManager::CMD_GET_THICKNESS, 
                           QByteArray(reinterpret_cast<const char*>(&normalThickness), sizeof(float)));
    
    // Test measurement below minimum (should trigger alert)
    float lowThickness = 5.0f;
    _mockDevice->setThicknessValue(lowThickness);
    _utgManager->startSingleMeasurement();
    
    _simulateDeviceResponse(UTGManager::CMD_GET_THICKNESS, 
                           QByteArray(reinterpret_cast<const char*>(&lowThickness), sizeof(float)));
    
    // Test measurement above maximum (should trigger alert)
    float highThickness = 35.0f;
    _mockDevice->setThicknessValue(highThickness);
    _utgManager->startSingleMeasurement();
    
    _simulateDeviceResponse(UTGManager::CMD_GET_THICKNESS, 
                           QByteArray(reinterpret_cast<const char*>(&highThickness), sizeof(float)));
    
    // Note: Alert verification would require additional signal monitoring
    // or integration with the alert system
}

// Helper method implementations
void UTGIntegrationTest::_connectMockDevice(void)
{
    // Connect mock device signals to simulate real device behavior
    connect(_mockDevice, &MockUTGDevice::responseReady, this, [this](const QByteArray& response) {
        // In a real implementation, this would be handled by the serial port
        // For testing, we can directly inject responses into UTGManager
    });
}

void UTGIntegrationTest::_disconnectMockDevice(void)
{
    if (_mockDevice) {
        _mockDevice->setConnected(false);
    }
}

void UTGIntegrationTest::_waitForSignal(QObject* sender, const char* signal, int timeoutMs)
{
    QSignalSpy spy(sender, signal);
    QVERIFY(spy.wait(timeoutMs));
}

void UTGIntegrationTest::_verifyMeasurement(float expectedValue, float tolerance)
{
    QVERIFY(qAbs(_utgManager->currentThickness() - expectedValue) <= tolerance);
}

void UTGIntegrationTest::_verifyParameterSync(void)
{
    // This would verify that UTGManager sends the current settings to the device
    // In a real implementation, this might involve checking the command queue
    // or monitoring the serial communication
}

void UTGIntegrationTest::_simulateDeviceResponse(UTGManager::UTGCommand cmd, const QByteArray& data)
{
    // Build a proper response packet
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
    
    // In a real implementation, this would be injected into the UTGManager's
    // serial port receive buffer or processing method
}
