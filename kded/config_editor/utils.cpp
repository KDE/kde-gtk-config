// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "utils.h"

#include <QDir>
#include <QStandardPaths>

namespace
{

QStringList gtkThemeSearchPaths()
{
    QStringList paths;
    paths << QDir::homePath() + QStringLiteral("/.themes/");
    paths << QDir::homePath() + QStringLiteral("/.local/share/themes/");

    for (const QString &dataLocation : QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation)) {
        paths << dataLocation + QStringLiteral("/themes/");
    }

    return paths;
}

bool gtkThemeExists(const QString &themeName)
{
    if (themeName.isEmpty()) {
        return false;
    }

    for (const QString &themeBasePath : gtkThemeSearchPaths()) {
        if (QDir(themeBasePath + themeName).exists()) {
            return true;
        }
    }

    return false;
}

}

namespace Utils
{

QString configDirPath(int gtkVersion)
{
    auto configLocation = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    return QStringLiteral("%1/gtk-%2.0").arg(configLocation).arg(gtkVersion);
};

QString readFileContents(QFile &file)
{
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return file.readAll();
    } else {
        return QString();
    }
}

QString gtk2ThemeName(const QString &themeName, bool preferDarkTheme)
{
    if (!preferDarkTheme) {
        return themeName;
    }

    // clang-format off
    static const QStringList suffixes = {
        QStringLiteral("-dark"),
        QStringLiteral("-Dark"),
        QStringLiteral(" dark"),
        QStringLiteral(" Dark"),
        QStringLiteral("dark"),
        QStringLiteral("Dark")
    };
    // clang-format on

    for (const QString &suffix : suffixes) {
        const QString candidate = themeName + suffix;
        if (gtkThemeExists(candidate)) {
            return candidate;
        }
    }

    return themeName;
}
}
