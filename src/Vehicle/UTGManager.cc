/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "UTGManager.h"
#include "Vehicle.h"
#include "QGCApplication.h"
#include "SettingsManager.h"

#ifdef __android__
#include "qserialportinfo.h"
#else
#include <QSerialPortInfo>
#endif
#include <QDebug>

Q_LOGGING_CATEGORY(UTGManagerLog, "UTGManagerLog")

UTGManager::UTGManager(Vehicle* vehicle, QObject* parent)
    : QObject(parent)
    , _vehicle(vehicle)
    , _settings(nullptr)
    , _serialPort(nullptr)
    , _connectionTimer(new QTimer(this))
    , _measurementTimer(new QTimer(this))
    , _connected(false)
    , _enabled(false)
    , _currentThickness(0.0f)
    , _status(STATUS_DISCONNECTED)
    , _measuring(false)
    , _cachedBaudRate(9600)
{
    // Safely get UTG settings with null checks
    QGCApplication* app = qgcApp();
    if (app && app->toolbox() && app->toolbox()->settingsManager()) {
        _settings = app->toolbox()->settingsManager()->utgSettings();
    }

    if (!_settings) {
        qCCritical(UTGManagerLog) << "Failed to get UTGSettings - UTGManager will be disabled";
        return;
    }
    
    // Setup timers
    _connectionTimer->setSingleShot(true);
    _measurementTimer->setSingleShot(false);
    
    connect(_connectionTimer, &QTimer::timeout, this, &UTGManager::_onConnectionTimer);
    connect(_measurementTimer, &QTimer::timeout, this, &UTGManager::_onMeasurementTimer);
    
    // Connect to settings changes
    connect(_settings->enabled(), &Fact::rawValueChanged, this, &UTGManager::_processSettings);
    connect(_settings->serialPort(), &Fact::rawValueChanged, this, &UTGManager::_processSettings);
    connect(_settings->baudRate(), &Fact::rawValueChanged, this, &UTGManager::_processSettings);
    connect(_settings->autoConnect(), &Fact::rawValueChanged, this, &UTGManager::_processSettings);
    
    // Initial settings processing
    _processSettings();
}

UTGManager::~UTGManager()
{
    disconnectFromUTG();
}

QString UTGManager::statusText() const
{
    switch (_status) {
    case STATUS_DISCONNECTED:
        return tr("Disconnected");
    case STATUS_CONNECTING:
        return tr("Connecting...");
    case STATUS_CONNECTED:
        return tr("Connected");
    case STATUS_ERROR:
        return tr("Error: %1").arg(_lastError);
    case STATUS_MEASURING:
        return tr("Measuring");
    case STATUS_CALIBRATING:
        return tr("Calibrating");
    default:
        return tr("Unknown");
    }
}

void UTGManager::setEnabled(bool enabled)
{
    if (!_settings) {
        qCWarning(UTGManagerLog) << "Cannot set enabled state - settings not available";
        return;
    }

    if (_enabled != enabled) {
        _enabled = enabled;
        _settings->enabled()->setRawValue(enabled);

        if (enabled && _settings->autoConnect()->rawValue().toBool()) {
            connectToUTG();
        } else if (!enabled) {
            disconnectFromUTG();
        }

        emit enabledChanged(_enabled);
    }
}

void UTGManager::connectToUTG()
{
    if (_connected || _status == STATUS_CONNECTING) {
        return;
    }
    
    qCDebug(UTGManagerLog) << "Attempting to connect to UTG";
    _setStatus(STATUS_CONNECTING);
    _setupSerial();
}

void UTGManager::disconnectFromUTG()
{
    if (!_connected && _status != STATUS_CONNECTING) {
        return;
    }
    
    qCDebug(UTGManagerLog) << "Disconnecting from UTG";
    stopMeasurement();
    _closeSerial();
    _setStatus(STATUS_DISCONNECTED);
    _connected = false;
    emit connectedChanged(_connected);
}

void UTGManager::reconnect()
{
    disconnectFromUTG();
    QTimer::singleShot(1000, this, &UTGManager::connectToUTG);
}

void UTGManager::startSingleMeasurement()
{
    if (!_connected) {
        _setError(tr("UTG not connected"));
        return;
    }
    
    _sendCommand(CMD_GET_THICKNESS);
}

void UTGManager::startContinuousMeasurement()
{
    if (!_connected) {
        _setError(tr("UTG not connected"));
        return;
    }
    
    _sendCommand(CMD_START_CONTINUOUS);
    _measuring = true;
    _measurementTimer->start(MEASUREMENT_INTERVAL_MS);
    _setStatus(STATUS_MEASURING);
    emit measuringChanged(_measuring);
}

void UTGManager::stopMeasurement()
{
    if (_measuring) {
        _sendCommand(CMD_STOP_CONTINUOUS);
        _measuring = false;
        _measurementTimer->stop();
        _setStatus(STATUS_CONNECTED);
        emit measuringChanged(_measuring);
    }
}

void UTGManager::triggerMeasurement()
{
    startSingleMeasurement();
}

void UTGManager::setGain(int gain)
{
    if (!_connected) return;
    
    QByteArray data;
    data.append(static_cast<char>(gain));
    _sendCommand(CMD_SET_GAIN, data);
}

void UTGManager::setSoundVelocity(double velocity)
{
    if (!_connected) return;
    
    QByteArray data;
    // Convert velocity to bytes (assuming 4-byte float)
    float vel = static_cast<float>(velocity);
    data.append(reinterpret_cast<const char*>(&vel), sizeof(float));
    _sendCommand(CMD_SET_VELOCITY, data);
}

void UTGManager::setZeroOffset(double offset)
{
    if (!_connected) return;
    
    QByteArray data;
    float off = static_cast<float>(offset);
    data.append(reinterpret_cast<const char*>(&off), sizeof(float));
    _sendCommand(CMD_SET_ZERO_OFFSET, data);
}

void UTGManager::setThreshold(int threshold)
{
    if (!_connected) return;
    
    QByteArray data;
    data.append(static_cast<char>(threshold));
    _sendCommand(CMD_SET_THRESHOLD, data);
}

void UTGManager::setPulseWidth(int width)
{
    if (!_connected) return;
    
    QByteArray data;
    data.append(reinterpret_cast<const char*>(&width), sizeof(int));
    _sendCommand(CMD_SET_PULSE_WIDTH, data);
}

void UTGManager::setFrequency(double frequency)
{
    if (!_connected) return;
    
    QByteArray data;
    float freq = static_cast<float>(frequency);
    data.append(reinterpret_cast<const char*>(&freq), sizeof(float));
    _sendCommand(CMD_SET_FREQUENCY, data);
}

void UTGManager::setMeasurementUnit(int unit)
{
    if (!_connected) return;
    
    QByteArray data;
    data.append(static_cast<char>(unit));
    _sendCommand(CMD_SET_UNIT, data);
}

void UTGManager::setMeasurementMode(int mode)
{
    if (!_connected) return;
    
    QByteArray data;
    data.append(static_cast<char>(mode));
    _sendCommand(CMD_SET_MODE, data);
}

void UTGManager::setMaterialType(int material)
{
    if (!_connected) return;
    
    QByteArray data;
    data.append(static_cast<char>(material));
    _sendCommand(CMD_SET_MATERIAL, data);
}

void UTGManager::startCalibration(int mode, double referenceValue)
{
    if (!_connected) return;
    
    QByteArray data;
    data.append(static_cast<char>(mode));
    if (referenceValue > 0.0) {
        float ref = static_cast<float>(referenceValue);
        data.append(reinterpret_cast<const char*>(&ref), sizeof(float));
    }
    
    _setStatus(STATUS_CALIBRATING);
    _sendCommand(CMD_CALIBRATE, data);
}

void UTGManager::performZeroCalibration()
{
    startCalibration(0); // Zero calibration mode
}

void UTGManager::performVelocityCalibration(double knownThickness)
{
    startCalibration(2, knownThickness); // Velocity calibration mode
}

void UTGManager::resetUTG()
{
    if (!_connected) return;
    
    _sendCommand(CMD_RESET);
    // After reset, we might need to reconfigure
    QTimer::singleShot(2000, this, &UTGManager::_updateSettings);
}

void UTGManager::getStatus()
{
    if (!_connected) return;
    
    _sendCommand(CMD_GET_STATUS);
}

void UTGManager::getTemperature()
{
    if (!_connected) return;

    _sendCommand(CMD_GET_TEMPERATURE);
}

void UTGManager::_onSerialDataReceived()
{
    if (!_serialPort) return;

    QByteArray data = _serialPort->readAll();
    _receiveBuffer.append(data);

    // Process complete messages
    while (_receiveBuffer.size() >= 4) { // Minimum message size
        _processReceivedData(_receiveBuffer);
        break; // Process one message at a time
    }
}

void UTGManager::_onSerialError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) return;

    QString errorString = _serialPort ? _serialPort->errorString() : tr("Unknown error");
    qCWarning(UTGManagerLog) << "Serial error:" << errorString;

    _setError(errorString);
    _setStatus(STATUS_ERROR);

    // Try to reconnect after error
    _connectionTimer->start(CONNECTION_RETRY_MS);
}

void UTGManager::_onConnectionTimer()
{
    if (_status == STATUS_CONNECTING) {
        connectToUTG();
    } else if (_status == STATUS_ERROR) {
        reconnect();
    }
}

void UTGManager::_onMeasurementTimer()
{
    if (_measuring && _connected) {
        startSingleMeasurement();
    }
}

void UTGManager::_processSettings()
{
    bool wasEnabled = _enabled;
    _enabled = _settings->enabled()->rawValue().toBool();

    if (_enabled != wasEnabled) {
        emit enabledChanged(_enabled);
    }

    if (_enabled && _settings->autoConnect()->rawValue().toBool() && !_connected) {
        connectToUTG();
    } else if (!_enabled && _connected) {
        disconnectFromUTG();
    }
}

void UTGManager::_setupSerial()
{
    _closeSerial();

    QString portName = _settings->serialPort()->rawValue().toString();
    if (portName.isEmpty()) {
        _setError(tr("No serial port configured"));
        _setStatus(STATUS_ERROR);
        return;
    }

    _serialPort = new QSerialPort(this);
    _serialPort->setPortName(portName);
    _serialPort->setBaudRate(_settings->baudRate()->rawValue().toInt());
    _serialPort->setDataBits(static_cast<QSerialPort::DataBits>(_settings->dataBits()->rawValue().toInt()));
    _serialPort->setStopBits(static_cast<QSerialPort::StopBits>(_settings->stopBits()->rawValue().toInt()));
    _serialPort->setParity(static_cast<QSerialPort::Parity>(_settings->parity()->rawValue().toInt()));
    _serialPort->setFlowControl(_settings->flowControl()->rawValue().toBool() ?
                                QSerialPort::HardwareControl : QSerialPort::NoFlowControl);

    connect(_serialPort, &QSerialPort::readyRead, this, &UTGManager::_onSerialDataReceived);
    connect(_serialPort, QOverload<QSerialPort::SerialPortError>::of(&QSerialPort::error),
            this, &UTGManager::_onSerialError);

    if (_serialPort->open(QIODevice::ReadWrite)) {
        _connected = true;
        _setStatus(STATUS_CONNECTED);
        emit connectedChanged(_connected);

        // Initialize UTG with current settings
        _updateSettings();

        qCDebug(UTGManagerLog) << "Connected to UTG on" << portName;
    } else {
        _setError(_serialPort->errorString());
        _setStatus(STATUS_ERROR);
        _connectionTimer->start(CONNECTION_RETRY_MS);
        qCWarning(UTGManagerLog) << "Failed to open UTG port:" << _serialPort->errorString();
    }
}

void UTGManager::_closeSerial()
{
    if (_serialPort) {
        _serialPort->close();
        _serialPort->deleteLater();
        _serialPort = nullptr;
    }
    _receiveBuffer.clear();
}

void UTGManager::_sendCommand(UTGCommand cmd, const QByteArray& data)
{
    if (!_serialPort || !_serialPort->isOpen()) {
        qCWarning(UTGManagerLog) << "Cannot send command: serial port not open";
        return;
    }

    QMutexLocker locker(&_commandMutex);

    QByteArray command = _buildCommand(cmd, data);
    qint64 written = _serialPort->write(command);

    if (written != command.size()) {
        qCWarning(UTGManagerLog) << "Failed to write complete command";
        _setError(tr("Communication error"));
    } else {
        qCDebug(UTGManagerLog) << "Sent command:" << QString::number(cmd, 16) << "data size:" << data.size();
    }
}

void UTGManager::_processReceivedData(const QByteArray& data)
{
    // Simple protocol: [START][CMD][LEN][DATA][CHECKSUM]
    // START = 0xAA, CMD = command byte, LEN = data length, DATA = payload, CHECKSUM = XOR of all bytes

    if (data.size() < 4) return; // Minimum: START + CMD + LEN + CHECKSUM

    if (static_cast<unsigned char>(data[0]) != 0xAA) {
        // Look for start byte
        int startIndex = data.indexOf(static_cast<char>(0xAA));
        if (startIndex > 0) {
            _receiveBuffer.remove(0, startIndex);
        } else {
            _receiveBuffer.clear();
        }
        return;
    }

    unsigned char cmd = static_cast<unsigned char>(data[1]);
    unsigned char len = static_cast<unsigned char>(data[2]);

    if (data.size() < 4 + len) return; // Not enough data yet

    QByteArray payload = data.mid(3, len);
    unsigned char receivedChecksum = static_cast<unsigned char>(data[3 + len]);

    // Verify checksum
    unsigned char calculatedChecksum = 0;
    for (int i = 0; i < 3 + len; i++) {
        calculatedChecksum ^= static_cast<unsigned char>(data[i]);
    }

    if (calculatedChecksum != receivedChecksum) {
        qCWarning(UTGManagerLog) << "Checksum mismatch";
        _receiveBuffer.remove(0, 4 + len);
        return;
    }

    // Process valid response
    _handleResponse(static_cast<UTGCommand>(cmd), payload);
    _receiveBuffer.remove(0, 4 + len);
}

void UTGManager::_handleResponse(UTGCommand cmd, const QByteArray& response)
{
    switch (cmd) {
    case CMD_GET_THICKNESS:
        if (response.size() >= sizeof(float)) {
            float thickness = *reinterpret_cast<const float*>(response.data());
            _currentThickness = thickness;
            emit thicknessChanged(thickness);
            emit measurementReceived(thickness, QDateTime::currentMSecsSinceEpoch());
            qCDebug(UTGManagerLog) << "Thickness measurement:" << thickness;
        }
        break;

    case CMD_GET_STATUS:
        if (response.size() >= 1) {
            unsigned char status = static_cast<unsigned char>(response[0]);
            qCDebug(UTGManagerLog) << "UTG status:" << status;
        }
        break;

    case CMD_GET_TEMPERATURE:
        if (response.size() >= sizeof(float)) {
            float temperature = *reinterpret_cast<const float*>(response.data());
            emit temperatureReceived(temperature);
            qCDebug(UTGManagerLog) << "Temperature:" << temperature;
        }
        break;

    case CMD_CALIBRATE:
        if (response.size() >= 1) {
            bool success = response[0] != 0;
            emit calibrationCompleted(success);
            _setStatus(STATUS_CONNECTED);
            qCDebug(UTGManagerLog) << "Calibration" << (success ? "successful" : "failed");
        }
        break;

    default:
        qCDebug(UTGManagerLog) << "Received response for command:" << QString::number(cmd, 16);
        break;
    }
}

void UTGManager::_setStatus(UTGStatus status)
{
    if (_status != status) {
        _status = status;
        emit statusChanged(_status);
    }
}

void UTGManager::_setError(const QString& error)
{
    _lastError = error;
    emit errorChanged(_lastError);
}

void UTGManager::_updateSettings()
{
    if (!_connected) return;

    // Send current settings to UTG
    setGain(_settings->gain()->rawValue().toInt());
    setSoundVelocity(_settings->soundVelocity()->rawValue().toDouble());
    setZeroOffset(_settings->zeroOffset()->rawValue().toDouble());
    setThreshold(_settings->threshold()->rawValue().toInt());
    setPulseWidth(_settings->pulseWidth()->rawValue().toInt());
    setFrequency(_settings->frequency()->rawValue().toDouble());
    setMeasurementUnit(_settings->measurementUnit()->rawValue().toInt());
    setMeasurementMode(_settings->measurementMode()->rawValue().toInt());
    setMaterialType(_settings->materialType()->rawValue().toInt());
}

QByteArray UTGManager::_buildCommand(UTGCommand cmd, const QByteArray& data)
{
    QByteArray command;
    command.append(static_cast<char>(0xAA)); // Start byte
    command.append(static_cast<char>(cmd));  // Command
    command.append(static_cast<char>(data.size())); // Data length
    command.append(data); // Data payload

    // Calculate checksum (XOR of all bytes)
    unsigned char checksum = 0;
    for (char byte : command) {
        checksum ^= static_cast<unsigned char>(byte);
    }
    command.append(static_cast<char>(checksum));

    return command;
}

bool UTGManager::_validateResponse(const QByteArray& response)
{
    return response.size() >= 4 && static_cast<unsigned char>(response[0]) == 0xAA;
}
