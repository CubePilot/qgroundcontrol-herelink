/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


#include "APMSpiritComponent.h"
#include "APMAutoPilotPlugin.h"
#include "APMAirframeComponent.h"
#include "ParameterManager.h"

APMSpiritComponent::APMSpiritComponent(Vehicle* vehicle, AutoPilotPlugin* autopilot, QObject* parent)
    : VehicleComponent(vehicle, autopilot, parent),
    _name(tr("Spirit"))
{
}

QString APMSpiritComponent::name(void) const
{
    return _name;
}

QString APMSpiritComponent::description(void) const
{
    return tr("The Spirit Component is used to setup battery count and payload weight.");
}

QString APMSpiritComponent::iconResource(void) const
{
    return QStringLiteral("/qmlimages/SpiritIcon.png");
}

QUrl APMSpiritComponent::setupSource(void) const
{
    return QUrl::fromUserInput(QStringLiteral("qrc:/qml/APMSpiritComponent.qml"));
}

QUrl APMSpiritComponent::summaryQmlSource(void) const
{
    return QUrl::fromUserInput(QStringLiteral("qrc:/qml/APMSpiritComponentSummary.qml"));
}
