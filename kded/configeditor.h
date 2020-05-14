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
#pragma once

#include <unistd.h>
#include <QString>

class QColor;
class QFile;
class QVariant;

namespace ConfigEditor
{
    void setGtk2ConfigValue(const QString &paramName, const QVariant &paramValue);

    void setGtk3ConfigValueGSettings(const QString &paramName, const QVariant &paramValue, const QString &category = QStringLiteral("org.gnome.desktop.interface"));
    void setGtk3ConfigValueGSettingsAsEnum(const QString &paramName, int paramValue, const QString &category = QStringLiteral("org.gnome.desktop.interface"));
    void setGtk3ConfigValueSettingsIni(const QString &paramName, const QVariant &paramValue);
    void setGtk3ConfigValueXSettingsd(const QString &paramName, const QVariant &paramValue);

    void setGtk3Colors(const QMap<QString, QColor> &colorsDefinitions);

    QString gtk3ConfigValueSettingsIni(const QString& paramName);

    void removeLegacyGtk2Strings();

    void addGtkModule(const QString &moduleName);

    void addImportStatementToGtkCssUserFile();
    void modifyColorsCssFile(const QMap<QString, QColor> &colorsDefinitions);

    void replaceValueInGtkrcContents(QString &gtkrcContents, const QString &paramName, const QVariant &paramValue);
    void replaceValueInXSettingsdContents(QString &xSettingsdContents, const QString &paramName, const QVariant &paramValue);

    QString readFileContents(QFile &gtkrc);

    void reloadXSettingsd();

    pid_t pidOfXSettingsd();
};
