// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "custom_css.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>

#include "config_editor/settings_ini.h"
#include "config_editor/utils.h"

namespace CustomCssEditor
{
namespace
{

void saveWindowDecorationsToAssets(const QStringList &windowDecorationsButtonsImages)
{
    for (auto gtkVersion : Utils::s_gtkVersions) {
        auto assetsFolderPath = Utils::configDirPath(gtkVersion) + QStringLiteral("/assets");
        auto assetsFolder = QDir(assetsFolderPath);

        if (!assetsFolder.exists()) {
            assetsFolder.mkpath(QStringLiteral("."));
        }

        for (const auto &buttonImagePath : windowDecorationsButtonsImages) {
            const QString destination = assetsFolder.path() + '/' + QFileInfo(buttonImagePath).fileName();
            QFile(destination).remove();
            QFile(buttonImagePath).copy(buttonImagePath, destination);
        }

        for (const auto &buttonImagePath : windowDecorationsButtonsImages) {
            QFile(buttonImagePath).remove();
        }
    }
}

void addWindowDecorationsCssFile()
{
    for (auto gtkVersion : Utils::s_gtkVersions) {
        QFile windowDecorationsCss{QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("/themes/Breeze/window_decorations.css"))};

        auto windowDecorationsDestination = Utils::configDirPath(gtkVersion) + QStringLiteral("/window_decorations.css");

        QFile(windowDecorationsDestination).remove();
        windowDecorationsCss.copy(windowDecorationsDestination);
    }
}

void setImportStatement(int gtkVersion, const QByteArray &statement, bool enabled)
{
    QFile gtkCss(Utils::configDirPath(gtkVersion) + QStringLiteral("/gtk.css"));

    if (gtkCss.open(QIODevice::ReadWrite)) {
        QByteArray gtkCssContents = gtkCss.readAll();

        if (!enabled) {
            gtkCssContents.replace('\n' + statement, QByteArray()).replace(statement, QByteArray());
        } else if (!gtkCssContents.contains(statement)) {
            gtkCssContents.append('\n' + statement);
        }

        gtkCss.resize(0);
        gtkCss.write(gtkCssContents);
    }
}

void removeWindowDecorationsCSS()
{
    for (auto gtkVersion : Utils::s_gtkVersions) {
        QFile windowsDecorationsCss(Utils::configDirPath(gtkVersion) + QStringLiteral("/window_decorations.css"));
        windowsDecorationsCss.remove();
    }
}

void modifyColorsCssFile(const QMap<QString, QColor> &colorsDefinitions)
{
    for (auto gtkVersion : Utils::s_gtkVersions) {
        QString colorsCssPath = Utils::configDirPath(gtkVersion) + QStringLiteral("/colors.css");
        QFile colorsCss(colorsCssPath);

        if (colorsCss.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QTextStream colorsCssStream(&colorsCss);

            for (auto it = colorsDefinitions.cbegin(); it != colorsDefinitions.cend(); it++) {
                colorsCssStream << QStringLiteral("@define-color %1 %2;\n").arg(it.key(), it.value().name());
            }
        }
    }
}
}

void addGtkModule(const QString &moduleName)
{
    const QString currentModulesString = SettingsIniEditor::value(QStringLiteral("gtk-modules"));

    if (currentModulesString.contains(moduleName)) {
        return;
    }

    if (currentModulesString.isEmpty()) { // No modules
        SettingsIniEditor::setValue(QStringLiteral("gtk-modules"), moduleName, 3);
    } else {
        SettingsIniEditor::setValue(QStringLiteral("gtk-modules"), QStringLiteral("%1:%2").arg(currentModulesString, moduleName), 3);
    }
}

void setColors(const QMap<QString, QColor> &colorsDefinitions)
{
    for (auto gtkVersion : Utils::s_gtkVersions) {
        setImportStatement(gtkVersion, QByteArrayLiteral("@import 'colors.css';"), true);
    }
    modifyColorsCssFile(colorsDefinitions);
    // addGtkModule is called in GtkConfig::setColors
}

void setCustomClientSideDecorations(const QStringList &windowDecorationsButtonsImages)
{
    saveWindowDecorationsToAssets(windowDecorationsButtonsImages);
    addWindowDecorationsCssFile();
    addGtkModule(QStringLiteral("window-decorations-gtk-module"));
}

void disableCustomClientSideDecorations()
{
    removeWindowDecorationsCSS();
}

void enableAdwaitaColors()
{
    const QString destination = Utils::configDirPath(4) + QStringLiteral("/adwaita_colors.css");
    QFile::remove(destination);
    QFile::copy(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("/themes/Breeze/adwaita_colors.css")), destination);
    setImportStatement(4, QByteArrayLiteral("@import 'adwaita_colors.css';"), true);
}

void disableAdwaitaColors()
{
    QFile::remove(Utils::configDirPath(4) + QStringLiteral("/adwaita_colors.css"));
    setImportStatement(4, QByteArrayLiteral("@import 'adwaita_colors.css';"), false);
}

}
