/*
 * SPDX-FileCopyrightText: 2019 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */
#pragma once

#include <QString>
#include <functional>
#include <unistd.h>

class QColor;
class QFile;
class QVariant;

namespace ConfigEditor
{
using namespace std::placeholders;

void setGtk2ConfigValue(const QString &paramName, const QVariant &paramValue);

void setGtkConfigValueGSettings(const QString &paramName, const QVariant &paramValue, const QString &category = QStringLiteral("org.gnome.desktop.interface"));
void setGtkConfigValueGSettingsAsEnum(const QString &paramName, int paramValue, const QString &category = QStringLiteral("org.gnome.desktop.interface"));
void setGtk3ConfigValueXSettingsd(const QString &paramName, const QVariant &paramValue);

void setGtkConfigValueSettingsIni(const QString &versionString, const QString &paramName, const QVariant &paramValue);
static auto setGtk4ConfigValueSettingsIni = std::bind(setGtkConfigValueSettingsIni, QStringLiteral("gtk-4.0"), _1, _2);
static auto setGtk3ConfigValueSettingsIni = std::bind(setGtkConfigValueSettingsIni, QStringLiteral("gtk-3.0"), _1, _2);

QString gtkConfigValueSettingsIni(const QString &versionString, const QString &paramName);
static auto gtk4ConfigValueSettingsIni = std::bind(gtkConfigValueSettingsIni, QStringLiteral("gtk-4.0"), _1);
static auto gtk3ConfigValueSettingsIni = std::bind(gtkConfigValueSettingsIni, QStringLiteral("gtk-3.0"), _1);

void setCustomClientSideDecorations(const QStringList &windowDecorationsButtonsImages);
void disableCustomClientSideDecorations();
void setGtk3Colors(const QMap<QString, QColor> &colorsDefinitions);

void removeLegacyGtk2Strings();

void saveWindowDecorationsToAssets(const QStringList &windowDecorationsButtonsImages);
void addWindowDecorationsCssFile();

void addGtkModule(const QString &moduleName);

void addImportStatementsToGtkCssUserFile();
void removeWindowDecorationsCSS();
void modifyColorsCssFile(const QMap<QString, QColor> &colorsDefinitions);

void replaceValueInGtkrcContents(QString &gtkrcContents, const QString &paramName, const QVariant &paramValue);
void replaceValueInXSettingsdContents(QString &xSettingsdContents, const QString &paramName, const QVariant &paramValue);

QString readFileContents(QFile &gtkrc);

void reloadXSettingsd();

pid_t pidOfXSettingsd();
};
