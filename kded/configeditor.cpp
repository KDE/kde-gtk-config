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

#include <QColor>
#include <QDir>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QProcess>
#include <QVariant>
#include <QMap>
#include <QList>
#include <QTextStream>

#include <KSharedConfig>
#include <KConfigGroup>
#include <KColorScheme>
#include <KColorUtils>

#include <string>
#include <csignal>
#include <cstdio>

#include <gio/gio.h>

#include "configeditor.h"

void ConfigEditor::setGtk3ConfigValueGSettings(const QString &paramName, const QVariant &paramValue, const QString &category)
{
    g_autoptr(GSettings) gsettings = g_settings_new(category.toUtf8().constData());

    if (paramValue.type() == QVariant::Type::String) {
        g_settings_set_string(gsettings, paramName.toUtf8().constData(), paramValue.toString().toUtf8().constData());
    } else if (paramValue.type() == QVariant::Type::Int) {
        g_settings_set_int(gsettings, paramName.toUtf8().constData(), paramValue.toInt());
    } else if (paramValue.type() == QVariant::Type::Bool) {
        g_settings_set_boolean(gsettings, paramName.toUtf8().constData(), paramValue.toBool());
    }
}

void ConfigEditor::setGtk3ConfigValueGSettingsAsEnum(const QString& paramName, int paramValue, const QString& category)
{
    g_autoptr(GSettings) gsettings = g_settings_new(category.toUtf8().constData());
    g_settings_set_enum(gsettings, paramName.toUtf8().constData(), paramValue);
}

void ConfigEditor::setGtk3ConfigValueSettingsIni(const QString &paramName, const QVariant &paramValue)
{
    QString configLocation = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    QString gtk3ConfigPath = configLocation + QStringLiteral("/gtk-3.0/settings.ini");

    KSharedConfig::Ptr gtk3Config = KSharedConfig::openConfig(gtk3ConfigPath, KConfig::NoGlobals);
    KConfigGroup group = gtk3Config->group(QStringLiteral("Settings"));

    group.writeEntry(paramName, paramValue);
    group.sync();
}

void ConfigEditor::setGtk3ConfigValueXSettingsd(const QString &paramName, const QVariant &paramValue)
{
    QString configLocation = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);

    QDir xsettingsdPath = configLocation + QStringLiteral("/xsettingsd");
    if (!xsettingsdPath.exists()) {
        xsettingsdPath.mkpath(QStringLiteral("."));
    }

    QString xSettingsdConfigPath = xsettingsdPath.path() + QStringLiteral("/xsettingsd.conf");

    QFile xSettingsdConfig(xSettingsdConfigPath);
    QString xSettingsdConfigContents = readFileContents(xSettingsdConfig);
    replaceValueInXSettingsdContents(xSettingsdConfigContents, paramName, paramValue);
    xSettingsdConfig.remove();
    xSettingsdConfig.open(QIODevice::WriteOnly | QIODevice::Text);
    xSettingsdConfig.write(xSettingsdConfigContents.toUtf8());
    reloadXSettingsd();
}

void ConfigEditor::setGtk2ConfigValue(const QString &paramName, const QVariant &paramValue)
{
    QString gtkrcPath = qEnvironmentVariable("GTK2_RC_FILES", QDir::homePath() + QStringLiteral("/.gtkrc-2.0"));
    if (gtkrcPath.contains(QStringLiteral(":/"))) { // I.e. env variable contains multiple paths
        gtkrcPath = QDir::homePath() + QStringLiteral("/.gtkrc-2.0");
    }
    QFile gtkrc(gtkrcPath);
    QString gtkrcContents = readFileContents(gtkrc);
    replaceValueInGtkrcContents(gtkrcContents, paramName, paramValue);
    gtkrc.remove();
    gtkrc.open(QIODevice::WriteOnly | QIODevice::Text);
    gtkrc.write(gtkrcContents.toUtf8());
}

void ConfigEditor::setCustomClientSideDecorations(const QStringList &windowDecorationsButtonsImages)
{
    saveWindowDecorationsToAssets(windowDecorationsButtonsImages);
    addWindowDecorationsCssFile();
    addImportStatementsToGtkCssUserFile();
    addGtkModule(QStringLiteral("window-decorations-gtk-module"));
}

void ConfigEditor::disableCustomClientSideDecorations()
{
    removeDecorationsImportStatementFromGtkCssUserFile();
}

void ConfigEditor::setGtk3Colors(const QMap<QString, QColor> &colorsDefinitions)
{
    addImportStatementsToGtkCssUserFile();
    modifyColorsCssFile(colorsDefinitions);
    addGtkModule(QStringLiteral("colorreload-gtk-module"));
}

QString ConfigEditor::gtk3ConfigValueSettingsIni(const QString& paramName)
{
    QString configLocation = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    QString gtk3ConfigPath = configLocation + QStringLiteral("/gtk-3.0/settings.ini");

    KSharedConfig::Ptr gtk3Config = KSharedConfig::openConfig(gtk3ConfigPath, KConfig::NoGlobals);
    KConfigGroup group = gtk3Config->group(QStringLiteral("Settings"));

    return group.readEntry(paramName);
}


void ConfigEditor::removeLegacyGtk2Strings()
{
    QString gtkrcPath = QDir::homePath() + QStringLiteral("/.gtkrc-2.0");
    QFile gtkrc(gtkrcPath);
    QString gtkrcContents = readFileContents(gtkrc);

    // Remove "include" lines
    // Example:
    // include "/usr/share/themes/Adwaita-dark/gtk-2.0/gtkrc"
    static const QRegularExpression includeRegExp(QStringLiteral("include .*\n"));
    gtkrcContents.remove(includeRegExp);

    // Remove redundant font config lines
    // Example:
    // style "user-font"
    // {
    //     font_name="Noto Sans Regular"
    // }
    // widget_class "*" style "user-font"
    static const QRegularExpression userFontStyleRegexp(QStringLiteral("style(.|\n)*{(.|\n)*}\nwidget_class.*\"user-font\""));
    gtkrcContents.remove(userFontStyleRegexp);

    gtkrc.remove();
    gtkrc.open(QIODevice::WriteOnly | QIODevice::Text);
    gtkrc.write(gtkrcContents.toUtf8());
}

void ConfigEditor::saveWindowDecorationsToAssets(const QStringList &windowDecorationsButtonsImages)
{
    QDir assetsFolder {QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QStringLiteral("/gtk-3.0/assets")};

    if (!assetsFolder.exists()) {
        assetsFolder.mkpath(QStringLiteral("."));
    }

    for (const auto &buttonImagePath : windowDecorationsButtonsImages) {
        const QString destination = assetsFolder.path() + '/' + QFileInfo(buttonImagePath).fileName();
        QFile(destination).remove();
        QFile(buttonImagePath).rename(buttonImagePath, destination);
    }
}

void ConfigEditor::addWindowDecorationsCssFile()
{
    QFile windowDecorationsCss {QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("/themes/Breeze/window_decorations.css"))};
    QString windowDecorationsDestination {QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QStringLiteral("/gtk-3.0/window_decorations.css")};

    QFile(windowDecorationsDestination).remove();
    windowDecorationsCss.copy(windowDecorationsDestination);
}

void ConfigEditor::addGtkModule(const QString& moduleName)
{
    const QString currentModulesString = gtk3ConfigValueSettingsIni(QStringLiteral("gtk-modules"));

    if (currentModulesString.contains(moduleName)) {
        return;
    }

    if (currentModulesString.isEmpty()) { // No modules
        setGtk3ConfigValueSettingsIni(QStringLiteral("gtk-modules"), moduleName);
    } else {
        setGtk3ConfigValueSettingsIni(QStringLiteral("gtk-modules"), QStringLiteral("%1:%2").arg(currentModulesString, moduleName));
    }
}

void ConfigEditor::addImportStatementsToGtkCssUserFile()
{
    QString gtkCssPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QStringLiteral("/gtk-3.0/gtk.css");
    QFile gtkCss(gtkCssPath);

    if (gtkCss.open(QIODevice::ReadWrite)) {
        QByteArray gtkCssContents = gtkCss.readAll().trimmed();

        static const QVector<QByteArray> importStatements {
            QByteArrayLiteral("\n@import 'colors.css';"),
            QByteArrayLiteral("\n@import 'window_decorations.css';"),
        };

        for (const auto &statement : importStatements) {
            if (!gtkCssContents.contains(statement.trimmed())) {
                gtkCssContents.append(statement);
            }
        }

        gtkCss.remove();
        gtkCss.open(QIODevice::WriteOnly | QIODevice::Text);
        gtkCss.write(gtkCssContents);
    }
}

void ConfigEditor::removeDecorationsImportStatementFromGtkCssUserFile()
{
    QString gtkCssPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QStringLiteral("/gtk-3.0/gtk.css");
    QFile gtkCss(gtkCssPath);

    if (gtkCss.open(QIODevice::ReadWrite)) {
        QByteArray gtkCssContents = gtkCss.readAll().trimmed();

        static const QByteArray importStatement = QByteArrayLiteral("\n@import 'window_decorations.css';");

        gtkCssContents.replace(importStatement.trimmed(), QByteArray());

        gtkCss.remove();
        gtkCss.open(QIODevice::WriteOnly | QIODevice::Text);
        gtkCss.write(gtkCssContents);
    }
}

void ConfigEditor::modifyColorsCssFile(const QMap<QString, QColor> &colorsDefinitions)
{
    QString colorsCssPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QStringLiteral("/gtk-3.0/colors.css");
    QFile colorsCss(colorsCssPath);

    if (colorsCss.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream colorsCssStream(&colorsCss);

        for (auto it = colorsDefinitions.cbegin(); it != colorsDefinitions.cend(); it++) {
            colorsCssStream << QStringLiteral("@define-color %1 %2;\n").arg(it.key(), it.value().name());
        }
    }
}

QString ConfigEditor::readFileContents(QFile &file)
{
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        return file.readAll();
    } else {
        return QString();
    }
}

void ConfigEditor::replaceValueInGtkrcContents(QString &gtkrcContents, const QString &paramName, const QVariant &paramValue)
{
    const QRegularExpression regExp(paramName + QStringLiteral("=[^\n]*($|\n)"));

    QString newConfigString;
    if (paramValue.type() == QVariant::Type::String) {
        newConfigString = QStringLiteral("%1=\"%2\"\n").arg(paramName, paramValue.toString());
    } else if (paramValue.type() == QVariant::Type::Bool) {
        // GTK2 does not support 'true' and 'false' as values
        newConfigString = QStringLiteral("%1=%2\n").arg(paramName, QString::number(paramValue.toInt()));
    } else {
        newConfigString = QStringLiteral("%1=%2\n").arg(paramName, paramValue.toString());
    }

    if (gtkrcContents.contains(regExp)) {
        gtkrcContents.replace(regExp, newConfigString);
    } else {
        gtkrcContents = newConfigString + gtkrcContents;
    }
}

void ConfigEditor::replaceValueInXSettingsdContents(QString &xSettingsdContents, const QString &paramName, const QVariant &paramValue)
{
    const QRegularExpression regExp(paramName + QStringLiteral(" [^\n]*($|\n)"));

    QString newConfigString;
    if (paramValue.type() == QVariant::Type::String) {
        newConfigString = QStringLiteral("%1 \"%2\"\n").arg(paramName, paramValue.toString());
    } else if (paramValue.type() == QVariant::Type::Bool) {
        // XSettigsd does not support 'true' and 'false' as values
        newConfigString = QStringLiteral("%1 %2\n").arg(paramName, QString::number(paramValue.toInt()));
    } else {
        newConfigString = QStringLiteral("%1 %2\n").arg(paramName, paramValue.toString());
    }

    if (xSettingsdContents.contains(regExp)) {
        xSettingsdContents.replace(regExp, newConfigString);
    } else {
        xSettingsdContents = newConfigString + xSettingsdContents;
    }
}

pid_t ConfigEditor::pidOfXSettingsd()
{
    QProcess pidof;
    pidof.start(QStringLiteral("pidof"), QStringList() << QStringLiteral("-s") << QStringLiteral("xsettingsd"));
    pidof.waitForFinished();
    QString xsettingsdPid = QString(pidof.readAllStandardOutput()).remove('\n');
    return xsettingsdPid.toInt();
}

void ConfigEditor::reloadXSettingsd()
{
    pid_t xSettingsdPid = pidOfXSettingsd();
    if (xSettingsdPid == 0) {
        QProcess::startDetached(QStandardPaths::findExecutable(QStringLiteral("xsettingsd")), QStringList());
    } else {
        kill(xSettingsdPid, SIGHUP);
    }
}
