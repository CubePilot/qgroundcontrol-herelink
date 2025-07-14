/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

/// @file
/// @brief UTG (Ultrasonic Thickness Gauge) Manager

#pragma once

#include <QObject>
#include <QTimer>
#ifdef __android__
#include "qserialport.h"
#else
#include <QSerialPort>
#endif
#include <QQueue>
#include <QMutex>
#include "QGCLoggingCategory.h"
#include "UTGSettings.h"

Q_DECLARE_LOGGING_CATEGORY(UTGManagerLog)

class Vehicle;

/// UTG Manager handles communication and control of the Ultrasonic Thickness Gauge
class UTGManager : public QObject
{
    Q_OBJECT

public:
    UTGManager(Vehicle* vehicle, QObject* parent = nullptr);
    ~UTGManager();

    enum UTGCommand {
        CMD_GET_THICKNESS = 0x01,
        CMD_SET_GAIN = 0x02,
        CMD_SET_VELOCITY = 0x03,
        CMD_SET_ZERO_OFFSET = 0x04,
        CMD_CALIBRATE = 0x05,
        CMD_SET_THRESHOLD = 0x06,
        CMD_SET_PULSE_WIDTH = 0x07,
        CMD_SET_FREQUENCY = 0x08,
        CMD_GET_STATUS = 0x09,
        CMD_RESET = 0x0A,
        CMD_SET_UNIT = 0x0B,
        CMD_SET_MODE = 0x0C,
        CMD_START_CONTINUOUS = 0x0D,
        CMD_STOP_CONTINUOUS = 0x0E,
        CMD_GET_TEMPERATURE = 0x0F,
        CMD_SET_MATERIAL = 0x10
    };

    enum UTGStatus {
        STATUS_DISCONNECTED,
        STATUS_CONNECTING,
        STATUS_CONNECTED,
        STATUS_ERROR,
        STATUS_MEASURING,
        STATUS_CALIBRATING
    };

    enum MeasurementMode {
        MODE_SINGLE,
        MODE_CONTINUOUS,
        MODE_TRIGGERED
    };

    // Properties
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(float currentThickness READ currentThickness NOTIFY thicknessChanged)
    Q_PROPERTY(UTGStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)
    Q_PROPERTY(bool measuring READ measuring NOTIFY measuringChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY errorChanged)

    // Getters
    bool connected() const { return _connected; }
    bool enabled() const { return _enabled; }
    float currentThickness() const { return _currentThickness; }
    UTGStatus status() const { return _status; }
    QString statusText() const;
    bool measuring() const { return _measuring; }
    QString lastError() const { return _lastError; }

    // Setters
    void setEnabled(bool enabled);

public slots:
    // Connection management
    void connectToUTG();
    void disconnectFromUTG();
    void reconnect();

    // Measurement commands
    void startSingleMeasurement();
    void startContinuousMeasurement();
    void stopMeasurement();
    void triggerMeasurement();

    // Configuration commands
    void setGain(int gain);
    void setSoundVelocity(double velocity);
    void setZeroOffset(double offset);
    void setThreshold(int threshold);
    void setPulseWidth(int width);
    void setFrequency(double frequency);
    void setMeasurementUnit(int unit);
    void setMeasurementMode(int mode);
    void setMaterialType(int material);

    // Calibration commands
    void startCalibration(int mode, double referenceValue = 0.0);
    void performZeroCalibration();
    void performVelocityCalibration(double knownThickness);

    // Utility commands
    void resetUTG();
    void getStatus();
    void getTemperature();

signals:
    void connectedChanged(bool connected);
    void enabledChanged(bool enabled);
    void thicknessChanged(float thickness);
    void statusChanged(UTGStatus status);
    void measuringChanged(bool measuring);
    void errorChanged(QString error);
    void measurementReceived(float thickness, qint64 timestamp);
    void calibrationCompleted(bool success);
    void temperatureReceived(float temperature);

private slots:
    void _onSerialDataReceived();
    void _onSerialError(QSerialPort::SerialPortError error);
    void _onConnectionTimer();
    void _onMeasurementTimer();
    void _processSettings();

private:
    // Internal methods
    void _setupSerial();
    void _closeSerial();
    void _sendCommand(UTGCommand cmd, const QByteArray& data = QByteArray());
    void _processReceivedData(const QByteArray& data);
    void _handleResponse(UTGCommand cmd, const QByteArray& response);
    void _setStatus(UTGStatus status);
    void _setError(const QString& error);
    void _updateSettings();
    QByteArray _buildCommand(UTGCommand cmd, const QByteArray& data);
    bool _validateResponse(const QByteArray& response);

    Vehicle* _vehicle;
    UTGSettings* _settings;
    QSerialPort* _serialPort;
    QTimer* _connectionTimer;
    QTimer* _measurementTimer;
    
    // State variables
    bool _connected;
    bool _enabled;
    float _currentThickness;
    UTGStatus _status;
    bool _measuring;
    QString _lastError;
    
    // Communication
    QQueue<QPair<UTGCommand, QByteArray>> _commandQueue;
    QByteArray _receiveBuffer;
    QMutex _commandMutex;
    
    // Settings cache
    QString _cachedSerialPort;
    int _cachedBaudRate;
    
    static const int RESPONSE_TIMEOUT_MS = 1000;
    static const int CONNECTION_RETRY_MS = 5000;
    static const int MEASUREMENT_INTERVAL_MS = 100;
};
