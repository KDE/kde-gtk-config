/*
 * Copyright (C) 2019 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QFont>
#include <QString>
#include <QColor>

#include <KConfig>
#include <KIconTheme>
#include <KSharedConfig>
#include <KConfigGroup>

#include "configvalueprovider.h"

ConfigValueProvider::ConfigValueProvider() :
    kdeglobalsConfig(KSharedConfig::openConfig(QStringLiteral("kdeglobals"))),
    inputConfig(KSharedConfig::openConfig(QStringLiteral("kcminputrc"))),
    kwinConfig(KSharedConfig::openConfig(QStringLiteral("kwinrc")))
{

}

QString ConfigValueProvider::fontName() const
{
    static const QFont defaultFont(QStringLiteral("Noto Sans"), 10);

    KConfigGroup configGroup = kdeglobalsConfig->group(QStringLiteral("General"));
    QString fontAsString = configGroup.readEntry(QStringLiteral("font"), defaultFont.toString());
    static QFont font;
    font.fromString(fontAsString);
    const QString fontStyle = fontStyleHelper(font);
    return font.family() + QStringLiteral(", ") + fontStyle + ' ' + QString::number(font.pointSize());
}

QString ConfigValueProvider::fontStyleHelper(const QFont &font) const
{
    // BUG: 333146
    // Since Qt sometimes gives us wrong font style name,
    // we ought to use this big helper function to construct
    // the style ourselves. Some fonts will not work
    auto weight = font.weight();
    QString result;
    if (weight > QFont::Normal) {
        if (weight >= QFont::Black) {
            result = QStringLiteral("Black");
        } else if (weight >= QFont::ExtraBold) {
            result = QStringLiteral("Extra Bold");
        } else if (weight >= QFont::Bold) {
            result = QStringLiteral("Bold");
        } else if (weight >= QFont::DemiBold) {
            result = QStringLiteral("Demi Bold");
        } else if (weight >= QFont::Medium) {
            result = QStringLiteral("Medium");
        }
    } else {
        if (weight <= QFont::Thin) {
            result = QStringLiteral("Thin");
        } else if (weight <= QFont::ExtraLight) {
            result = QStringLiteral("Extra Light");
        } else if (weight <= QFont::Light) {
            result = QStringLiteral("Light");
        }
    }

    auto style = font.style();
    if (style == QFont::StyleItalic) {
        result += QLatin1Char(' ') + QStringLiteral("Italic");
    } else if (style == QFont::StyleOblique) {
        result += QLatin1Char(' ') + QStringLiteral("Oblique");
    }

    auto stretch = font.stretch();
    if (stretch == QFont::UltraCondensed) {
        result += QLatin1Char(' ') + QStringLiteral("UltraCondensed");
    } else if (stretch == QFont::ExtraCondensed) {
        result += QLatin1Char(' ') + QStringLiteral("ExtraCondensed");
    } else if (stretch == QFont::Condensed) {
        result += QLatin1Char(' ') + QStringLiteral("Condensed");
    } else if (stretch == QFont::SemiCondensed) {
        result += QLatin1Char(' ') + QStringLiteral("SemiCondensed");
    } else if (stretch == QFont::Unstretched) {
        result += QLatin1Char(' ') + QStringLiteral("Unstretched");
    } else if (stretch == QFont::SemiExpanded) {
        result += QLatin1Char(' ') + QStringLiteral("SemiExpanded");
    } else if (stretch == QFont::Expanded) {
        result += QLatin1Char(' ') + QStringLiteral("Expanded");
    } else if (stretch == QFont::ExtraExpanded) {
        result += QLatin1Char(' ') + QStringLiteral("ExtraExpanded");
    } else if (stretch == QFont::UltraExpanded) {
        result += QLatin1Char(' ') + QStringLiteral("UltraExpanded");
    }

    return result.simplified();
}


QString ConfigValueProvider::iconThemeName() const
{
    KConfigGroup configGroup = kdeglobalsConfig->group(QStringLiteral("Icons"));
    return configGroup.readEntry(QStringLiteral("Theme"), QStringLiteral("breeze"));
}

QString ConfigValueProvider::cursorThemeName() const
{
    KConfigGroup configGroup = inputConfig->group(QStringLiteral("Mouse"));
    return configGroup.readEntry(QStringLiteral("cursorTheme"), QStringLiteral("breeze_cursors"));
}

QString ConfigValueProvider::iconsOnButtons() const
{
    KConfigGroup configGroup = kdeglobalsConfig->group(QStringLiteral("KDE"));
    bool kdeConfigValue = configGroup.readEntry(QStringLiteral("ShowIconsOnPushButtons"), true);

    if (kdeConfigValue) {
        return QStringLiteral("1");
    } else {
        return QStringLiteral("0");
    }
}

QString ConfigValueProvider::iconsInMenus() const
{
    KConfigGroup configGroup = kdeglobalsConfig->group(QStringLiteral("KDE"));
    bool kdeConfigValue = configGroup.readEntry(QStringLiteral("ShowIconsInMenuItems"), true);

    if (kdeConfigValue) {
        return QStringLiteral("1");
    } else {
        return QStringLiteral("0");
    }
}

QString ConfigValueProvider::toolbarStyle(ConfigValueProvider::ToolbarStyleNotation notation) const
{
    KConfigGroup configGroup = kdeglobalsConfig->group(QStringLiteral("Toolbar style"));
    QString kdeConfigValue = configGroup.readEntry(QStringLiteral("ToolButtonStyle"), "TextBesideIcon");
    return toolbarStyleInDesiredNotation(kdeConfigValue, notation);
}

QString ConfigValueProvider::scrollbarBehavior() const
{
    KConfigGroup configGroup = kdeglobalsConfig->group(QStringLiteral("KDE"));
    bool kdeConfigValue = configGroup.readEntry(QStringLiteral("ScrollbarLeftClickNavigatesByPage"), true);
    if (kdeConfigValue) { // GTK setting is inverted
        return QStringLiteral("0");
    } else {
        return QStringLiteral("1");
    }
}

QString ConfigValueProvider::preferDarkTheme() const
{
    KConfigGroup colorsConfigGroup = kdeglobalsConfig->group(QStringLiteral("Colors:Window"));
    QColor windowBackgroundColor = colorsConfigGroup.readEntry(QStringLiteral("BackgroundNormal"), QColor(239, 240, 241));
    const int windowBackgroundGray = qGray(windowBackgroundColor.rgb());

    // We use heuristic to determine if current color scheme is dark or not
    if (windowBackgroundGray >= 192) {
        return QStringLiteral("0");
    } else {
        return QStringLiteral("1");
    }
}

QString ConfigValueProvider::windowDecorationsButtonsOrder() const
{
    KConfigGroup configGroup = kwinConfig->group(QStringLiteral("org.kde.kdecoration2"));
    QString buttonsOnLeftKdeConfigValue = configGroup.readEntry(QStringLiteral("ButtonsOnLeft"), "MS");
    QString buttonsOnRightKdeConfigValue = configGroup.readEntry(QStringLiteral("ButtonsOnRight"), "HIAX");

    QString buttonsOnLeftInGtkNotation = windowDecorationButtonsOrderInGtkNotation(buttonsOnLeftKdeConfigValue);
    QString buttonsOnRightInGtkNotation = windowDecorationButtonsOrderInGtkNotation(buttonsOnRightKdeConfigValue);

    return buttonsOnLeftInGtkNotation + QStringLiteral(":") + buttonsOnRightInGtkNotation;
}

QString ConfigValueProvider::enableAnimations() const
{
    KConfigGroup generalCfg = kdeglobalsConfig->group(QStringLiteral("KDE"));
    const qreal animationSpeedModifier = qMax(0.0, generalCfg.readEntry("AnimationDurationFactor", 1.0));

    const bool enableAnimations = !qFuzzyIsNull(animationSpeedModifier);

    if (enableAnimations) {
        return QStringLiteral("1");
    } else {
        return QStringLiteral("0");
    }
}

QString ConfigValueProvider::toolbarStyleInDesiredNotation(const QString &kdeConfigValue, ConfigValueProvider::ToolbarStyleNotation notation) const
{
    QStringList toolbarStyles {};
    if (notation == ToolbarStyleNotation::SettingsIni) {
        toolbarStyles.append({
             QStringLiteral("GTK_TOOLBAR_ICONS"),
             QStringLiteral("GTK_TOOLBAR_TEXT"),
             QStringLiteral("GTK_TOOLBAR_BOTH_HORIZ"),
             QStringLiteral("GTK_TOOLBAR_BOTH")
        });
    } else if (notation == ToolbarStyleNotation::Xsettingsd) {
        toolbarStyles.append({
             QStringLiteral("0"),
             QStringLiteral("1"),
             QStringLiteral("3"),
             QStringLiteral("2")
        });
    } else {
        toolbarStyles.append({
             QStringLiteral("icons"),
             QStringLiteral("text"),
             QStringLiteral("both-horiz"),
             QStringLiteral("both")
        });
    }

    if (kdeConfigValue == QStringLiteral("NoText")) {
        return toolbarStyles[0];
    } else if (kdeConfigValue == QStringLiteral("TextOnly")) {
        return toolbarStyles[1];
    } else if (kdeConfigValue == QStringLiteral("TextBesideIcon")) {
        return toolbarStyles[2];
    } else {
        return toolbarStyles[3];
    }
}

QString ConfigValueProvider::windowDecorationButtonsOrderInGtkNotation(const QString &kdeConfigValue) const
{
    QString gtkNotation;

    for (const QChar &buttonAbbreviation : kdeConfigValue) {
        if (buttonAbbreviation == 'X') {
            gtkNotation += QStringLiteral("close,");
        } else if (buttonAbbreviation == 'I') {
            gtkNotation += QStringLiteral("minimize,");
        } else if (buttonAbbreviation == 'A') {
            gtkNotation += QStringLiteral("maximize,");
        } else if (buttonAbbreviation == 'M') {
            gtkNotation += QStringLiteral("icon,");
        }
    }
    gtkNotation.chop(1);

    return gtkNotation;
}
