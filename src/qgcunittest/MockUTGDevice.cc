/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "MockUTGDevice.h"
#include <QDebug>
#include <QRandomGenerator>

MockUTGDevice::MockUTGDevice(QObject* parent)
    : QObject(parent)
    , _connected(false)
    , _state(STATE_IDLE)
    , _currentThickness(0.0f)
    , _currentTemperature(23.5f)
    , _simulateErrors(false)
    , _responseDelay(50)
    , _continuousMeasurement(false)
    , _commandCount(0)
{
    _responseTimer = new QTimer(this);
    _responseTimer->setSingleShot(true);
    connect(_responseTimer, &QTimer::timeout, this, &MockUTGDevice::_sendDelayedResponse);
    
    _measurementTimer = new QTimer(this);
    _measurementTimer->setInterval(100); // 10 Hz measurement rate
    connect(_measurementTimer, &QTimer::timeout, this, &MockUTGDevice::_simulateMeasurement);
}

MockUTGDevice::~MockUTGDevice()
{
    setConnected(false);
}

void MockUTGDevice::setConnected(bool connected)
{
    if (_connected != connected) {
        _connected = connected;
        if (!connected) {
            _measurementTimer->stop();
            _continuousMeasurement = false;
            _state = STATE_IDLE;
        }
    }
}

void MockUTGDevice::injectCommand(const QByteArray& command)
{
    if (!_connected) {
        emit errorOccurred("Device not connected");
        return;
    }
    
    _processCommand(command);
}

void MockUTGDevice::_processCommand(const QByteArray& command)
{
    if (!_validateCommand(command)) {
        emit errorOccurred("Invalid command format");
        return;
    }
    
    _commandCount++;
    
    UTGManager::UTGCommand cmd = static_cast<UTGManager::UTGCommand>(command[1]);
    unsigned char dataLen = static_cast<unsigned char>(command[2]);
    QByteArray data = command.mid(3, dataLen);
    
    emit commandReceived(cmd, data);
    
    // Simulate error responses if enabled
    if (_simulateErrors && QRandomGenerator::global()->bounded(10) < 2) { // 20% error rate
        _sendErrorResponse(cmd);
        return;
    }
    
    // Process command based on type
    switch (cmd) {
    case UTGManager::CMD_GET_THICKNESS:
        _sendResponse(cmd, QByteArray(reinterpret_cast<const char*>(&_currentThickness), sizeof(float)));
        break;
        
    case UTGManager::CMD_SET_GAIN:
        if (dataLen >= 1) {
            _config.gain = static_cast<unsigned char>(data[0]);
            _sendResponse(cmd, QByteArray(1, 0x01)); // Success
        }
        break;
        
    case UTGManager::CMD_SET_VELOCITY:
        if (dataLen >= sizeof(float)) {
            _config.soundVelocity = *reinterpret_cast<const float*>(data.data());
            _sendResponse(cmd, QByteArray(1, 0x01)); // Success
        }
        break;
        
    case UTGManager::CMD_SET_ZERO_OFFSET:
        if (dataLen >= sizeof(float)) {
            _config.zeroOffset = *reinterpret_cast<const float*>(data.data());
            _sendResponse(cmd, QByteArray(1, 0x01)); // Success
        }
        break;
        
    case UTGManager::CMD_CALIBRATE:
        _state = STATE_CALIBRATING;
        // Simulate calibration delay
        QTimer::singleShot(1000, this, [this, cmd]() {
            _state = STATE_IDLE;
            _sendResponse(cmd, QByteArray(1, 0x01)); // Success
        });
        break;
        
    case UTGManager::CMD_SET_THRESHOLD:
        if (dataLen >= 1) {
            _config.threshold = static_cast<unsigned char>(data[0]);
            _sendResponse(cmd, QByteArray(1, 0x01)); // Success
        }
        break;
        
    case UTGManager::CMD_SET_PULSE_WIDTH:
        if (dataLen >= sizeof(int)) {
            _config.pulseWidth = *reinterpret_cast<const int*>(data.data());
            _sendResponse(cmd, QByteArray(1, 0x01)); // Success
        }
        break;
        
    case UTGManager::CMD_SET_FREQUENCY:
        if (dataLen >= sizeof(float)) {
            _config.frequency = *reinterpret_cast<const float*>(data.data());
            _sendResponse(cmd, QByteArray(1, 0x01)); // Success
        }
        break;
        
    case UTGManager::CMD_GET_STATUS:
        {
            unsigned char status = static_cast<unsigned char>(_state);
            _sendResponse(cmd, QByteArray(1, static_cast<char>(status)));
        }
        break;
        
    case UTGManager::CMD_RESET:
        _config = DeviceConfig(); // Reset to defaults
        _state = STATE_IDLE;
        _continuousMeasurement = false;
        _measurementTimer->stop();
        _sendResponse(cmd, QByteArray(1, 0x01)); // Success
        break;
        
    case UTGManager::CMD_SET_UNIT:
        if (dataLen >= 1) {
            _config.measurementUnit = static_cast<unsigned char>(data[0]);
            _sendResponse(cmd, QByteArray(1, 0x01)); // Success
        }
        break;
        
    case UTGManager::CMD_SET_MODE:
        if (dataLen >= 1) {
            _config.measurementMode = static_cast<unsigned char>(data[0]);
            _sendResponse(cmd, QByteArray(1, 0x01)); // Success
        }
        break;
        
    case UTGManager::CMD_START_CONTINUOUS:
        _continuousMeasurement = true;
        _state = STATE_MEASURING;
        _measurementTimer->start();
        _sendResponse(cmd, QByteArray(1, 0x01)); // Success
        break;
        
    case UTGManager::CMD_STOP_CONTINUOUS:
        _continuousMeasurement = false;
        _state = STATE_IDLE;
        _measurementTimer->stop();
        _sendResponse(cmd, QByteArray(1, 0x01)); // Success
        break;
        
    case UTGManager::CMD_GET_TEMPERATURE:
        _sendResponse(cmd, QByteArray(reinterpret_cast<const char*>(&_currentTemperature), sizeof(float)));
        break;
        
    case UTGManager::CMD_SET_MATERIAL:
        if (dataLen >= 1) {
            _config.materialType = static_cast<unsigned char>(data[0]);
            // Update sound velocity based on material
            switch (_config.materialType) {
            case 0: _config.soundVelocity = 5920.0; break; // Steel
            case 1: _config.soundVelocity = 6420.0; break; // Aluminum
            case 2: _config.soundVelocity = 4760.0; break; // Copper
            case 3: _config.soundVelocity = 2700.0; break; // Plastic
            default: break; // Custom - don't change velocity
            }
            _sendResponse(cmd, QByteArray(1, 0x01)); // Success
        }
        break;
        
    default:
        _sendErrorResponse(cmd);
        break;
    }
}

void MockUTGDevice::_sendResponse(UTGManager::UTGCommand cmd, const QByteArray& data)
{
    QByteArray response = _buildResponse(cmd, data);
    _lastResponse = response;
    
    if (_responseDelay > 0) {
        _pendingResponses.enqueue(response);
        _responseTimer->start(_responseDelay);
    } else {
        emit responseReady(response);
    }
}

void MockUTGDevice::_sendErrorResponse(UTGManager::UTGCommand cmd)
{
    QByteArray errorData(1, 0x00); // Error code
    _sendResponse(cmd, errorData);
}

void MockUTGDevice::_sendDelayedResponse()
{
    if (!_pendingResponses.isEmpty()) {
        QByteArray response = _pendingResponses.dequeue();
        emit responseReady(response);
    }
}

void MockUTGDevice::_simulateMeasurement()
{
    if (_continuousMeasurement && _state == STATE_MEASURING) {
        // Simulate realistic thickness variations
        float baseThickness = _currentThickness;
        if (baseThickness == 0.0f) {
            baseThickness = 25.0f; // Default test thickness
        }
        
        // Add small random variation (±0.1mm)
        float variation = (QRandomGenerator::global()->bounded(200) - 100) / 1000.0f;
        float newThickness = baseThickness + variation;
        
        // Send thickness measurement
        QByteArray thicknessData(reinterpret_cast<const char*>(&newThickness), sizeof(float));
        _sendResponse(UTGManager::CMD_GET_THICKNESS, thicknessData);
    }
}

QByteArray MockUTGDevice::_buildResponse(UTGManager::UTGCommand cmd, const QByteArray& data)
{
    QByteArray response;
    response.append(static_cast<char>(START_BYTE));
    response.append(static_cast<char>(cmd));
    response.append(static_cast<char>(data.size()));
    response.append(data);
    
    // Calculate checksum
    unsigned char checksum = 0;
    for (char byte : response) {
        checksum ^= static_cast<unsigned char>(byte);
    }
    response.append(static_cast<char>(checksum));
    
    return response;
}

bool MockUTGDevice::_validateCommand(const QByteArray& command)
{
    if (command.size() < MIN_COMMAND_SIZE) {
        return false;
    }
    
    if (static_cast<unsigned char>(command[0]) != START_BYTE) {
        return false;
    }
    
    unsigned char dataLen = static_cast<unsigned char>(command[2]);
    if (command.size() != MIN_COMMAND_SIZE + dataLen) {
        return false;
    }
    
    // Verify checksum
    unsigned char calculatedChecksum = 0;
    for (int i = 0; i < command.size() - 1; i++) {
        calculatedChecksum ^= static_cast<unsigned char>(command[i]);
    }
    
    unsigned char receivedChecksum = static_cast<unsigned char>(command.last());
    return calculatedChecksum == receivedChecksum;
}
