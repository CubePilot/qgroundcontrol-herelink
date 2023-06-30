#include "IGCSettings.h"

#include <QQmlEngine>
#include <QtQml>

DECLARE_SETTINGGROUP(IGC, "IGC")
{
    qmlRegisterUncreatableType<IGCSettings>("QGroundControl.SettingsManager", 1, 0, "IGCSettings", "Reference only"); \
}

DECLARE_SETTINGSFACT(IGCSettings, isEthernetCommEnabled)
DECLARE_SETTINGSFACT(IGCSettings, cameraIpAddress)
DECLARE_SETTINGSFACT(IGCSettings, cameraInfoUri)
DECLARE_SETTINGSFACT(IGCSettings, showMenuNavWidget)
