// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <KConfigGroup>

#include "editorbackend.h"

// ~/.config/gtk-N.0/settings.ini
class SettingsIniBackend : public EditorBackend
{
public:
    // -1 writes to every supported GTK version
    explicit SettingsIniBackend(int gtkVersion = -1);

    void set(const QString &key, const QVariant &value) override;
    void sync() override;

    // Current value of a key in the given version's settings.ini
    template<typename T = QString>
    static T value(const QString &key, int gtkVersion, const T &defaultValue = {})
    {
        return group(gtkVersion).readEntry(key, defaultValue);
    }

private:
    static KConfigGroup group(int gtkVersion);

    QList<KConfigGroup> m_groups;
};
