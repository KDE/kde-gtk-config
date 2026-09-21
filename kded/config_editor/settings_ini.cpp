// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "settings_ini.h"

#include <KSharedConfig>

#include "utils.h"

using namespace Qt::StringLiterals;

KConfigGroup SettingsIniBackend::group(int gtkVersion)
{
    return KSharedConfig::openConfig(Utils::configDirPath(gtkVersion) + u"/settings.ini"_s, KConfig::NoGlobals)->group(u"Settings"_s);
}

SettingsIniBackend::SettingsIniBackend(int gtkVersion)
{
    if (gtkVersion == -1) {
        for (int version : Utils::s_gtkVersions) {
            m_groups << group(version);
        }
    } else {
        m_groups << group(gtkVersion);
    }
}

void SettingsIniBackend::set(const QString &key, const QVariant &value)
{
    for (KConfigGroup &group : m_groups) {
        if (value.isNull()) {
            group.deleteEntry(key);
        } else {
            group.writeEntry(key, value);
        }
    }
}

void SettingsIniBackend::sync()
{
    for (KConfigGroup &group : m_groups) {
        group.sync();
    }
}
