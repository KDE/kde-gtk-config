// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "custom_css.h"

#include <QDir>
#include <QFile>

#include "config_editor/utils.h"

using namespace Qt::StringLiterals;

namespace CustomCssEditor
{
namespace
{

void saveWindowDecorationsToAssets(const QStringList &windowDecorationsButtonsImages)
{
    for (auto gtkVersion : Utils::s_gtkVersions) {
        QDir assetsFolder(Utils::configDirPath(gtkVersion) + u"/assets"_s);
        if (!assetsFolder.exists()) {
            assetsFolder.mkpath(u"."_s);
        }
        for (const auto &buttonImagePath : windowDecorationsButtonsImages) {
            Utils::updateFile(assetsFolder.filePath(QFileInfo(buttonImagePath).fileName()), Utils::readFile(buttonImagePath).toUtf8());
        }
    }
    for (const auto &buttonImagePath : windowDecorationsButtonsImages) {
        QFile::remove(buttonImagePath);
    }
}

void addWindowDecorationsCssFile()
{
    const QByteArray css = Utils::readFile(u":/window_decorations.css"_s).toUtf8();
    for (auto gtkVersion : Utils::s_gtkVersions) {
        Utils::updateFile(Utils::configDirPath(gtkVersion) + u"/window_decorations.css"_s, css);
    }
}

void addImportStatementsToGtkCssUserFile()
{
    for (auto gtkVersion : Utils::s_gtkVersions) {
        const QString gtkCssPath = Utils::configDirPath(gtkVersion) + u"/gtk.css"_s;
        QByteArray gtkCssContents = Utils::readFile(gtkCssPath).toUtf8().trimmed();

        static const QList<QByteArray> importStatements{
            QByteArrayLiteral("@import 'colors.css';"),
        };
        for (const auto &statement : importStatements) {
            if (!gtkCssContents.contains(statement)) {
                gtkCssContents.append('\n' + statement);
            }
        }
        Utils::updateFile(gtkCssPath, gtkCssContents);
    }
}

void removeWindowDecorationsCSS()
{
    for (auto gtkVersion : Utils::s_gtkVersions) {
        QFile::remove(Utils::configDirPath(gtkVersion) + u"/window_decorations.css"_s);
    }
}

void modifyColorsCssFile(const QMap<QString, QColor> &colorsDefinitions)
{
    QByteArray css;
    for (auto it = colorsDefinitions.cbegin(); it != colorsDefinitions.cend(); it++) {
        css += u"@define-color %1 %2;\n"_s.arg(it.key(), it.value().name()).toUtf8();
    }
    for (auto gtkVersion : Utils::s_gtkVersions) {
        Utils::updateFile(Utils::configDirPath(gtkVersion) + u"/colors.css"_s, css);
    }
}
}

void setColors(const QMap<QString, QColor> &colorsDefinitions)
{
    addImportStatementsToGtkCssUserFile();
    modifyColorsCssFile(colorsDefinitions);
}

void setCustomClientSideDecorations(const QStringList &windowDecorationsButtonsImages)
{
    saveWindowDecorationsToAssets(windowDecorationsButtonsImages);
    addWindowDecorationsCssFile();
}

void disableCustomClientSideDecorations()
{
    removeWindowDecorationsCSS();
}

}
