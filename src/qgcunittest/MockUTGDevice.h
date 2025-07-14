/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QObject>
#include <QTimer>
#ifdef __android__
#include "qserialport.h"
#else
#include <QSerialPort>
#endif
#include <QQueue>
#include "UTGManager.h"

/// Mock UTG device for testing UTGManager communication
class MockUTGDevice : public QObject
{
    Q_OBJECT

public:
    MockUTGDevice(QObject* parent = nullptr);
    ~MockUTGDevice();

    /// Device state
    enum DeviceState {
        STATE_IDLE,
        STATE_MEASURING,
        STATE_CALIBRATING,
        STATE_ERROR
    };

    /// Configuration
    struct DeviceConfig {
        int gain = 50;
        double soundVelocity = 5920.0;
        double zeroOffset = 0.0;
        int threshold = 50;
        int pulseWidth = 100;
        double frequency = 5.0;
        int measurementUnit = 0; // 0=mm, 1=inch, 2=mil
        int measurementMode = 0; // 0=pulse-echo, 1=through-transmission, 2=echo-echo
        int materialType = 0;    // 0=steel, 1=aluminum, 2=copper, 3=plastic, 4=custom
    };

    // Control methods
    void setConnected(bool connected);
    void setSimulateErrors(bool simulate) { _simulateErrors = simulate; }
    void setResponseDelay(int delayMs) { _responseDelay = delayMs; }
    void setThicknessValue(float thickness) { _currentThickness = thickness; }
    void setTemperature(float temperature) { _currentTemperature = temperature; }
    
    // State queries
    bool isConnected() const { return _connected; }
    DeviceState state() const { return _state; }
    DeviceConfig config() const { return _config; }
    float currentThickness() const { return _currentThickness; }
    
    // Test helpers
    void injectCommand(const QByteArray& command);
    QByteArray getLastResponse() const { return _lastResponse; }
    int getCommandCount() const { return _commandCount; }
    void clearCommandCount() { _commandCount = 0; }

signals:
    void commandReceived(UTGManager::UTGCommand command, QByteArray data);
    void responseReady(QByteArray response);
    void errorOccurred(QString error);

private slots:
    void _sendDelayedResponse();
    void _simulateMeasurement();

private:
    void _processCommand(const QByteArray& command);
    void _sendResponse(UTGManager::UTGCommand cmd, const QByteArray& data = QByteArray());
    void _sendErrorResponse(UTGManager::UTGCommand cmd);
    QByteArray _buildResponse(UTGManager::UTGCommand cmd, const QByteArray& data);
    bool _validateCommand(const QByteArray& command);
    
    // Device state
    bool _connected;
    DeviceState _state;
    DeviceConfig _config;
    float _currentThickness;
    float _currentTemperature;
    
    // Simulation control
    bool _simulateErrors;
    int _responseDelay;
    bool _continuousMeasurement;
    
    // Test tracking
    int _commandCount;
    QByteArray _lastResponse;
    QQueue<QByteArray> _pendingResponses;
    
    // Timers
    QTimer* _responseTimer;
    QTimer* _measurementTimer;
    
    // Protocol constants
    static const unsigned char START_BYTE = 0xAA;
    static const int MIN_COMMAND_SIZE = 4; // START + CMD + LEN + CHECKSUM
};
