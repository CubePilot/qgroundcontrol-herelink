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
#include "Vehicle.h"
#include "MultiSignalSpy.h"

#ifdef __android__
#include "qserialport.h"
#else
#include <QSerialPort>
#endif

/// Unit test for UTGManager
class UTGManagerTest : public UnitTest
{
    Q_OBJECT
    
public:
    UTGManagerTest(void);

    void init(void) override;
    void cleanup(void) override;

private slots:
    void _testInitialization(void);
    void _testConnectionManagement(void);
    void _testCommandBuilding(void);
    void _testResponseParsing(void);
    void _testMeasurementCommands(void);
    void _testCalibrationCommands(void);
    void _testParameterCommands(void);
    void _testErrorHandling(void);
    void _testStatusManagement(void);
    void _testSignalEmission(void);
    void _testSettingsIntegration(void);
    void _testProtocolValidation(void);
    void _testCommunicationTimeout(void);
    void _testSerialPortConfiguration(void);

private:
    Vehicle* _vehicle;
    UTGManager* _utgManager;
    
    // Helper methods
    void _verifyInitialState(void);
    void _testCommandStructure(UTGManager::UTGCommand cmd, const QByteArray& data, const QByteArray& expectedCommand);
    void _simulateResponse(UTGManager::UTGCommand cmd, const QByteArray& responseData);
    void _testStatusTransition(UTGManager::UTGStatus fromStatus, UTGManager::UTGStatus toStatus);
    QByteArray _buildTestResponse(UTGManager::UTGCommand cmd, const QByteArray& data);
    void _verifySignalEmission(const QString& signalName, int expectedCount);
};
