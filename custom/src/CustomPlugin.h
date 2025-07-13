/****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 *   @brief Custom QGCCorePlugin Declaration
 *   @author Gus Grubba <gus@auterion.com>
 */

#pragma once

#include "QGCCorePlugin.h"
#include "QGCOptions.h"
#include "QGCLoggingCategory.h"
#include "VideoReceiver.h"
#include "SettingsManager.h"

#include <QTranslator>

class CustomPlugin;
class CustomSettings;

Q_DECLARE_LOGGING_CATEGORY(CustomLog)

//-----------------------------------------------------------------------------
//-- Our own, custom options
class CustomOptions : public QGCOptions
{
public:
    CustomOptions(CustomPlugin*, QObject* parent = nullptr);
    QUrl        flyViewOverlay                  () const final { return QUrl::fromUserInput("qrc:/custom/CustomThicknessReadingFlyView.qml"); }
    QColor      toolbarBackgroundLight          () const final;
    QColor      toolbarBackgroundDark           () const final;
};

//-----------------------------------------------------------------------------
class CustomPlugin : public QGCCorePlugin
{
    Q_OBJECT
public:
    CustomPlugin(QGCApplication* app, QGCToolbox *toolbox);
    ~CustomPlugin();

    // Overrides from QGCCorePlugin
    QGCOptions*             options                         () final;
    QQmlApplicationEngine*  createRootWindow                (QObject* parent) final;
    void                    paletteOverride                 (QString colorName, QGCPalette::PaletteColorInfo_t& colorInfo) final;

    const static QColor     _windowShadeEnabledLightColor;
    const static QColor     _windowShadeEnabledDarkColor;

private:
    Q_PROPERTY(bool   getThicknessGaugeEnabled  READ getThicknessGaugeEnabled   WRITE setThicknessGaugeEnabled  NOTIFY thicknessGaugeUpdated)
    Q_PROPERTY(float  getThicknessReading       READ getThicknessReading        WRITE setThicknessReading   NOTIFY readingUpdated)
    Q_PROPERTY(int    connectContext            READ connectContext)
    Q_PROPERTY(QColor getBorderColor            READ getBorderColor             WRITE setBorderColor        NOTIFY readingUpdated)

    //QFile _test;

    bool isThicknessReadingEnabled = false;
    float thicknessReading = 0.0;
    QColor borderColor = QColor(0, 0, 0, 0);

    CustomOptions* _pOptions = nullptr;

    Vehicle* activeVehicle = nullptr;

    int connectContext();
    float getThicknessReading();
    void setThicknessReading(float reading);
    bool getThicknessGaugeEnabled();
    void setThicknessGaugeEnabled(bool);
    QColor getBorderColor();
    void setBorderColor(QColor color);
    void onParameterReadyVehicleAvailable();

    // void _initializeThicknessCsv();
    // void _writeThicknessCsvLine(float thicknessReading, float altitude = 0.0f);

private slots:
    void onThicknessReadingChange(float);

signals:
    void readingUpdated();
    void thicknessGaugeUpdated();
};
