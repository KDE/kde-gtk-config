// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "settings_ini.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QStandardPaths>

#include "config_editor/utils.h"

namespace SettingsIniEditor
{

namespace
{

KConfigGroup gtkConfigGroup(int gtkVersion)
{
    QString configLocation = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    auto gtkConfigPath = QStringLiteral("%1/gtk-%2.0/settings.ini").arg(configLocation).arg(gtkVersion);

    KSharedConfig::Ptr gtkConfig = KSharedConfig::openConfig(gtkConfigPath, KConfig::NoGlobals);
    return gtkConfig->group(QStringLiteral("Settings"));
}
}

void setValue(const QString &paramName, const QVariant &paramValue, int gtkVersion)
{
    auto setValueForVersion = [&](int version) {
        auto group = gtkConfigGroup(version);
        group.writeEntry(paramName, paramValue);
        group.sync();
    };

    if (gtkVersion != -1) {
        setValueForVersion(gtkVersion);
        return;
    }

    for (auto version : Utils::s_gtkVersions) {
        setValueForVersion(version);
    }
}

void unsetValue(const QString &paramName, int gtkVersion)
{
    auto unsetValueForVersion = [&](int version) {
        auto group = gtkConfigGroup(version);
        group.deleteEntry(paramName);
        group.sync();
    };

    if (gtkVersion != -1) {
        unsetValueForVersion(gtkVersion);
        return;
    }

    for (auto version : Utils::s_gtkVersions) {
        unsetValueForVersion(version);
    }
}

QString value(const QString &paramName, int gtkVersion)
{
    if (gtkVersion == -1) {
        gtkVersion = *Utils::s_gtkVersions.begin();
    }

    auto group = gtkConfigGroup(gtkVersion);
    return group.readEntry(paramName);
}

}
