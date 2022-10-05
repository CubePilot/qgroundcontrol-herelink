/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


/// @file
///     @author Don Gagne <don@thegagnes.com>

#include "QGCPalette.h"
#include "QGCApplication.h"
#include "QGCCorePlugin.h"

#include <QApplication>
#include <QPalette>

QList<QGCPalette*>   QGCPalette::_paletteObjects;

QGCPalette::Theme QGCPalette::_theme = QGCPalette::Dark;

QMap<int, QMap<int, QMap<QString, QColor>>> QGCPalette::_colorInfoMap;

QStringList QGCPalette::_colors;

QGCPalette::QGCPalette(QObject* parent) :
    QObject(parent),
    _colorGroupEnabled(true)
{
    if (_colorInfoMap.isEmpty()) {
        _buildMap();
    }

    // We have to keep track of all QGCPalette objects in the system so we can signal theme change to all of them
    _paletteObjects += this;
}

QGCPalette::~QGCPalette()
{
    bool fSuccess = _paletteObjects.removeOne(this);
    if (!fSuccess) {
        qWarning() << "Internal error";
    }
}

void QGCPalette::_buildMap()
{
    //                                      Light                 Dark
    //                                      Disabled   Enabled    Disabled   Enabled
    DECLARE_QGC_COLOR(window,               "#222222", "#222222", "#222222", "#222222")
    DECLARE_QGC_COLOR(windowShade,          "#333333", "#333333", "#333333", "#333333")
    DECLARE_QGC_COLOR(windowShadeDark,      "#282828", "#282828", "#282828", "#282828")
    DECLARE_QGC_COLOR(text,                 "#ffffff", "#ffffff", "#ffffff", "#ffffff")
    DECLARE_QGC_COLOR(warningText,          "#f85761", "#f85761", "#f85761", "#f85761")
    DECLARE_QGC_COLOR(button,               "#626270", "#626270", "#626270", "#626270")
    DECLARE_QGC_COLOR(buttonText,           "#ffffff", "#ffffff", "#ffffff", "#ffffff")
    DECLARE_QGC_COLOR(buttonHighlight,      "#fff291", "#fff291", "#fff291", "#fff291")
    DECLARE_QGC_COLOR(buttonHighlightText,  "#000000", "#000000", "#000000", "#000000")
    DECLARE_QGC_COLOR(primaryButton,        "#8cb3be", "#8cb3be", "#8cb3be", "#8cb3be")
    DECLARE_QGC_COLOR(primaryButtonText,    "#000000", "#000000", "#000000", "#000000")
    DECLARE_QGC_COLOR(textField,            "#ffffff", "#ffffff", "#ffffff", "#ffffff")
    DECLARE_QGC_COLOR(textFieldText,        "#000000", "#000000", "#000000", "#000000")
    DECLARE_QGC_COLOR(mapButton,            "#000000", "#000000", "#000000", "#000000")
    DECLARE_QGC_COLOR(mapButtonHighlight,   "#be781c", "#be781c", "#be781c", "#be781c")
    DECLARE_QGC_COLOR(mapIndicator,         "#be781c", "#be781c", "#be781c", "#be781c")
    DECLARE_QGC_COLOR(mapIndicatorChild,    "#766043", "#766043", "#766043", "#766043")
    DECLARE_QGC_COLOR(colorGreen,           "#00e04b", "#00e04b", "#00e04b", "#00e04b")
    DECLARE_QGC_COLOR(colorOrange,          "#de8500", "#de8500", "#de8500", "#de8500")
    DECLARE_QGC_COLOR(colorRed,             "#f32836", "#f32836", "#f32836", "#f32836")
    DECLARE_QGC_COLOR(colorGrey,            "#bfbfbf", "#bfbfbf", "#bfbfbf", "#bfbfbf")
    DECLARE_QGC_COLOR(colorBlue,            "#536dff", "#536dff", "#536dff", "#536dff")
    DECLARE_QGC_COLOR(alertBackground,      "#eecc44", "#eecc44", "#eecc44", "#eecc44")
    DECLARE_QGC_COLOR(alertBorder,          "#808080", "#808080", "#808080", "#808080")
    DECLARE_QGC_COLOR(alertText,            "#000000", "#000000", "#000000", "#000000")
    DECLARE_QGC_COLOR(missionItemEditor,    "#585d83", "#585d83", "#585d83", "#585d83")
    DECLARE_QGC_COLOR(hoverColor,           "#585d83", "#585d83", "#585d83", "#585d83")
    DECLARE_QGC_COLOR(statusFailedText,     "#ffffff", "#ffffff", "#ffffff", "#ffffff")
    DECLARE_QGC_COLOR(statusPassedText,     "#ffffff", "#ffffff", "#ffffff", "#ffffff")
    DECLARE_QGC_COLOR(statusPendingText,    "#ffffff", "#ffffff", "#ffffff", "#ffffff")

    // Colors are not affecting by theming
    DECLARE_QGC_COLOR(mapWidgetBorderLight, "#ffffff", "#ffffff", "#ffffff", "#ffffff")
    DECLARE_QGC_COLOR(mapWidgetBorderDark,  "#000000", "#000000", "#000000", "#000000")
    DECLARE_QGC_COLOR(brandingPurple,       "#262E41", "#262E41", "#262E41", "#262E41") //Midnight Blue
    DECLARE_QGC_COLOR(brandingBlue,         "#305671", "#305671", "#305671", "#305671") //Ascent Blue
}

void QGCPalette::setColorGroupEnabled(bool enabled)
{
    _colorGroupEnabled = enabled;
    emit paletteChanged();
}

void QGCPalette::setGlobalTheme(Theme newTheme)
{
    // Mobile build does not have themes
    if (_theme != newTheme) {
        _theme = newTheme;
        _signalPaletteChangeToAll();
    }
}

void QGCPalette::_signalPaletteChangeToAll()
{
    // Notify all objects of the new theme
    foreach (QGCPalette* palette, _paletteObjects) {
        palette->_signalPaletteChanged();
    }
}

void QGCPalette::_signalPaletteChanged()
{
    emit paletteChanged();
}
