/// @file
/// @brief IGC settings

#pragma once

#include "SettingsGroup.h"

/// Simple branding. Allows to define icon to use on main toolbar.
class IGCSettings : public SettingsGroup
{
    Q_OBJECT
public:
    IGCSettings(QObject* parent = nullptr);
    DEFINE_SETTING_NAME_GROUP()
    DEFINE_SETTINGFACT(isEthernetCommEnabled)
    DEFINE_SETTINGFACT(cameraIpAddress)
    DEFINE_SETTINGFACT(cameraInfoUri)
    DEFINE_SETTINGFACT(showMenuNavWidget)

};
