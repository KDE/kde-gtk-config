/*
 * SPDX-FileCopyrightText: 2019 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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

    void setCustomClientSideDecorations(const QStringList &windowDecorationsButtonsImages);
    void disableCustomClientSideDecorations();
    void setGtk3Colors(const QMap<QString, QColor> &colorsDefinitions);

    QString gtk3ConfigValueSettingsIni(const QString& paramName);

    void removeLegacyGtk2Strings();


    void saveWindowDecorationsToAssets(const QStringList &windowDecorationsButtonsImages);
    void addWindowDecorationsCssFile();

    void addGtkModule(const QString &moduleName);

    void addImportStatementsToGtkCssUserFile();
    void removeDecorationsImportStatementFromGtkCssUserFile();
    void modifyColorsCssFile(const QMap<QString, QColor> &colorsDefinitions);

    void replaceValueInGtkrcContents(QString &gtkrcContents, const QString &paramName, const QVariant &paramValue);
    void replaceValueInXSettingsdContents(QString &xSettingsdContents, const QString &paramName, const QVariant &paramValue);

    QString readFileContents(QFile &gtkrc);

    void reloadXSettingsd();

    pid_t pidOfXSettingsd();
};
