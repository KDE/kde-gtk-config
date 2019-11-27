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

#include <KConfig>
#include <KIconTheme>
#include <KSharedConfig>
#include <KConfigGroup>

#include "configvalueprovider.h"

ConfigValueProvider::ConfigValueProvider() :
    kdeglobalsConfig(KSharedConfig::openConfig(QStringLiteral("kdeglobals"))),
    inputConfig(KSharedConfig::openConfig(QStringLiteral("kcminputrc")))
{

}

QString ConfigValueProvider::fontName() const
{
    static const QFont defaultFont(QStringLiteral("Noto Sans"), 10);

    kdeglobalsConfig->reparseConfiguration();
    KConfigGroup configGroup = kdeglobalsConfig->group(QStringLiteral("General"));
    QString fontAsString = configGroup.readEntry(QStringLiteral("font"), defaultFont.toString());
    static QFont font;
    font.fromString(fontAsString);
    return font.family() + ' ' + font.styleName() + ' ' + QString::number(font.pointSize());
}

QString ConfigValueProvider::iconThemeName() const
{
    KIconTheme *newIconTheme = KIconLoader::global()->theme();
    if (newIconTheme) {
        return newIconTheme->internalName();
    } else {
        return QStringLiteral("breeze");
    }
}

QString ConfigValueProvider::cursorThemeName() const
{
    inputConfig->reparseConfiguration();
    KConfigGroup configGroup = inputConfig->group(QStringLiteral("Mouse"));
    return configGroup.readEntry(QStringLiteral("cursorTheme"), QStringLiteral("breeze_cursors"));
}

QString ConfigValueProvider::iconsOnButtons() const
{
    kdeglobalsConfig->reparseConfiguration();
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
    kdeglobalsConfig->reparseConfiguration();
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
    kdeglobalsConfig->reparseConfiguration();
    KConfigGroup configGroup = kdeglobalsConfig->group(QStringLiteral("Toolbar style"));
    QString kdeConfigValue = configGroup.readEntry(QStringLiteral("ToolButtonStyle"), "TextBesideIcon");
    return toolbarStyleInDesiredNotation(kdeConfigValue, notation);
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
