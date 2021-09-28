/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


#include "APMAutoPilotPlugin.h"
#include "UAS.h"
#include "APMParameterMetaData.h"
#include "APMFirmwarePlugin.h"
#include "ArduCopterFirmwarePlugin.h"
#include "ArduRoverFirmwarePlugin.h"
#include "VehicleComponent.h"
#include "APMAirframeComponent.h"
#include "APMFlightModesComponent.h"
#include "APMRadioComponent.h"
#include "APMSafetyComponent.h"
#include "APMTuningComponent.h"
#include "APMSensorsComponent.h"
#include "APMPowerComponent.h"
#include "APMSpiritComponent.h"
#include "APMMotorComponent.h"
#include "APMCameraComponent.h"
#include "APMLightsComponent.h"
#include "APMSubFrameComponent.h"
#include "APMFollowComponent.h"
#include "ESP8266Component.h"
#include "APMHeliComponent.h"
#include "QGCApplication.h"
#include "ParameterManager.h"

#if !defined(NO_SERIAL_LINK) && !defined(__android__)
#include <QSerialPortInfo>
#endif

/// This is the AutoPilotPlugin implementatin for the MAV_AUTOPILOT_ARDUPILOT type.
APMAutoPilotPlugin::APMAutoPilotPlugin(Vehicle* vehicle, QObject* parent)
    : AutoPilotPlugin           (vehicle, parent)
    , _incorrectParameterVersion(false)
    , _airframeComponent        (nullptr)
    , _cameraComponent          (nullptr)
    , _lightsComponent          (nullptr)
    , _subFrameComponent        (nullptr)
    , _flightModesComponent     (nullptr)
    , _powerComponent           (nullptr)
    , _motorComponent           (nullptr)
    , _radioComponent           (nullptr)
    , _safetyComponent          (nullptr)
    , _sensorsComponent         (nullptr)
    , _tuningComponent          (nullptr)
    , _esp8266Component         (nullptr)
    , _heliComponent            (nullptr)
#if 0
    // Follow me not ready for Stable
    , _followComponent          (nullptr)
#endif
{
#if !defined(NO_SERIAL_LINK) && !defined(__android__)
    connect(vehicle->parameterManager(), &ParameterManager::parametersReadyChanged, this, &APMAutoPilotPlugin::_checkForBadCubeBlack);
#endif
}

APMAutoPilotPlugin::~APMAutoPilotPlugin()
{

}

const QVariantList& APMAutoPilotPlugin::vehicleComponents(void)
{
    if (_components.count() == 0 && !_incorrectParameterVersion) {
        if (_vehicle->parameterManager()->parametersReady()) {

            _spiritComponent = new APMSpiritComponent(_vehicle, this);
            _spiritComponent->setupTriggerSignals();
            _components.append(QVariant::fromValue((VehicleComponent*)_spiritComponent));

            _safetyComponent = new APMSafetyComponent(_vehicle, this);
            _safetyComponent->setupTriggerSignals();
            _components.append(QVariant::fromValue((VehicleComponent*)_safetyComponent));

            _sensorsComponent = new APMSensorsComponent(_vehicle, this);
            _sensorsComponent->setupTriggerSignals();
            _components.append(QVariant::fromValue((VehicleComponent*)_sensorsComponent));

        } else {
            qWarning() << "Call to vehicleCompenents prior to parametersReady";
        }
    }

    return _components;
}

QString APMAutoPilotPlugin::prerequisiteSetup(VehicleComponent* component) const
{
    bool requiresAirframeCheck = false;

    if (qobject_cast<const APMSpiritComponent*>(component)) {
        requiresAirframeCheck = true;
    } else if (qobject_cast<const APMSafetyComponent*>(component)) {
        requiresAirframeCheck = true;
    } else if (qobject_cast<const APMSensorsComponent*>(component)) {
        requiresAirframeCheck = true;
    }

    if (requiresAirframeCheck) {
        if (_airframeComponent && !_airframeComponent->setupComplete()) {
            return _airframeComponent->name();
        }
    }

    return QString();
}

#if !defined(NO_SERIAL_LINK) && !defined(__android__)
/// The following code is executed when the Vehicle is parameter ready. It checks for the service bulletin against Cube Blacks.
void APMAutoPilotPlugin::_checkForBadCubeBlack(void)
{
    bool cubeBlackFound = false;
    for (const QVariant& varLink: _vehicle->links()) {
        SerialLink* serialLink = varLink.value<SerialLink*>();
        if (serialLink && QSerialPortInfo(*serialLink->_hackAccessToPort()).description().contains(QStringLiteral("CubeBlack"))) {
            cubeBlackFound = true;
        }

    }
    if (!cubeBlackFound) {
        return;
    }

    ParameterManager* paramMgr = _vehicle->parameterManager();

    QString paramAcc3("INS_ACC3_ID");
    QString paramGyr3("INS_GYR3_ID");
    QString paramEnableMask("INS_ENABLE_MASK");

    if (paramMgr->parameterExists(-1, paramAcc3) && paramMgr->getParameter(-1, paramAcc3)->rawValue().toInt() == 0 &&
            paramMgr->parameterExists(-1, paramGyr3) && paramMgr->getParameter(-1, paramGyr3)->rawValue().toInt() == 0 &&
            paramMgr->parameterExists(-1, paramEnableMask) && paramMgr->getParameter(-1, paramEnableMask)->rawValue().toInt() >= 7) {
        qgcApp()->showMessage(tr("WARNING: The flight board you are using has a critical service bulletin against it which advises against flying. For details see: https://discuss.cubepilot.org/t/sb-0000002-critical-service-bulletin-for-cubes-purchased-between-january-2019-to-present-do-not-fly/406"));

    }
}
#endif
