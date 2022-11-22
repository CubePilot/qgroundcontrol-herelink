#pragma once

#include <QObject>
#include <QString>
#include <QMetaObject>

#include "QGCMAVLink.h"
#include "Vehicle.h"

class Vehicle;

class AscentAlertsController : public QObject
{
    Q_OBJECT

public:
    AscentAlertsController(void);
    Q_PROPERTY(bool batteryFailsafe READ batteryFailsafe NOTIFY batteryFailsafeChanged);
    Q_PROPERTY(bool critBatteryFailsafe READ critBatteryFailsafe NOTIFY critBatteryFailsafeChanged);
    Q_PROPERTY(bool gpsFailsafe READ gpsFailsafe NOTIFY gpsFailsafeChanged);
    Q_PROPERTY(bool ekfFailsafe READ ekfFailsafe NOTIFY ekfFailsafeChanged);
    Q_PROPERTY(bool radioFailsafe READ radioFailsafe NOTIFY radioFailsafeChanged);

    bool batteryFailsafe(void) const {return (bool)_batteryFailsafe;}
    bool critBatteryFailsafe(void) const {return (bool)_critBatteryFailsafe;}
    bool gpsFailsafe(void) const {return (bool)_gpsFailsafe;}
    bool ekfFailsafe(void) const {return (bool)_ekfFailsafe;}
    bool radioFailsafe(void) const {return (bool)_radioFailsafe;}

signals:
    void batteryFailsafeChanged(bool batteryFailsafe);
    void critBatteryFailsafeChanged(bool critBatteryFailsafe);
    void gpsFailsafeChanged(bool gpsFailsafe);
    void ekfFailsafeChanged(bool ekfFailsafe);
    void radioFailsafeChanged(bool radioFailsafe);

private slots:
    void _setActiveVehicle                  (Vehicle* vehicle);
    void _handleTextMessage                  (int uasid, int componentid, int severity, QString text);

private:
    Vehicle*    _vehicle;
    bool        _batteryFailsafe = false;
    bool        _critBatteryFailsafe = false;
    bool        _gpsFailsafe = false;
    bool        _ekfFailsafe = false;
    bool        _radioFailsafe = false;
};