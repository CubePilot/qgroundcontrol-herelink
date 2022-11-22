#include "AscentAlertsController.h"
#include "QGCApplication.h"
#include "QGCCorePlugin.h"
#include "UAS.h"
#include "SettingsManager.h"

AscentAlertsController::AscentAlertsController(void)
    :   _vehicle{nullptr}
{
    auto *manager = qgcApp()->toolbox()->multiVehicleManager();
    connect(manager, &MultiVehicleManager::activeVehicleChanged, this, &AscentAlertsController::_setActiveVehicle);
    _setActiveVehicle(manager->activeVehicle());
}

void AscentAlertsController::_setActiveVehicle(Vehicle* vehicle)
{
    _vehicle = vehicle;
    connect(_vehicle, &Vehicle::textMessageReceived, this, &AscentAlertsController::_handleTextMessage);
}

void AscentAlertsController::_handleTextMessage(int uasid, int componentid, int severity, QString text) {
    if(text.contains(QStringLiteral("Battery")) && text.contains(QStringLiteral(" low "))){ //Note: space before low necessary to avoid catching the work "below"
        _batteryFailsafe = true;
        emit batteryFailsafeChanged(_batteryFailsafe);
    }
    else if(text.contains(QStringLiteral("Battery")) && text.contains(QStringLiteral(" critical "))){ //Note: case sensitive; if landed, there is a Battery Critical message
        _critBatteryFailsafe = true;
        emit critBatteryFailsafeChanged(_critBatteryFailsafe);
    }
    else if(text.contains(QStringLiteral("GPS Glitch cleared"))){
        _gpsFailsafe = false;
        emit gpsFailsafeChanged(_gpsFailsafe);
    }
    else if(text.contains(QStringLiteral("GPS Glitch"))){
        _gpsFailsafe = true;
        emit gpsFailsafeChanged(_gpsFailsafe);
    }
    else if(text.contains(QStringLiteral("EKF variance"))){
        _ekfFailsafe = true;
        emit ekfFailsafeChanged(_ekfFailsafe);
    }   
    else if(text.contains(QStringLiteral("EKF Failsafe Cleared")) || text.contains(QStringLiteral("is using GPS"))){
        _ekfFailsafe = false;
        emit ekfFailsafeChanged(_ekfFailsafe);
    }
    else if(text.contains(QStringLiteral("Radio Failsafe Cleared"))){
        _radioFailsafe = false;
        emit radioFailsafeChanged(_radioFailsafe);
    }
    else if(text.contains(QStringLiteral("Radio Failsafe"))){
        _radioFailsafe = true;
        emit radioFailsafeChanged(_radioFailsafe);
    }      
}