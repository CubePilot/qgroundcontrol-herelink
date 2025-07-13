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
#include "UTGManager.h"
#include "UTGSettings.h"
#include "MockUTGDevice.h"
#include "Vehicle.h"

/// Integration test for UTG system (UTGManager + UTGSettings + MockUTGDevice)
class UTGIntegrationTest : public UnitTest
{
    Q_OBJECT
    
public:
    UTGIntegrationTest(void);

    void init(void) override;
    void cleanup(void) override;

private slots:
    void _testFullWorkflow(void);
    void _testSettingsToManagerIntegration(void);
    void _testMeasurementWorkflow(void);
    void _testCalibrationWorkflow(void);
    void _testErrorRecovery(void);
    void _testContinuousMeasurement(void);
    void _testParameterSynchronization(void);
    void _testConnectionLifecycle(void);
    void _testMaterialPresets(void);
    void _testAlertSystem(void);

private:
    Vehicle* _vehicle;
    UTGManager* _utgManager;
    UTGSettings* _utgSettings;
    MockUTGDevice* _mockDevice;
    
    // Helper methods
    void _connectMockDevice(void);
    void _disconnectMockDevice(void);
    void _waitForSignal(QObject* sender, const char* signal, int timeoutMs = 5000);
    void _verifyMeasurement(float expectedValue, float tolerance = 0.1f);
    void _verifyParameterSync(void);
    void _simulateDeviceResponse(UTGManager::UTGCommand cmd, const QByteArray& data);
};
