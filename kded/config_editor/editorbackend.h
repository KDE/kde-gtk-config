// SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QString>
#include <QVariant>

// A place GTK settings are written to: set() records values, sync() writes them out in one go if anything changed
class EditorBackend
{
public:
    virtual ~EditorBackend() = default;

    // A null value unsets the key
    virtual void set(const QString &key, const QVariant &value) = 0;
    void unset(const QString &key)
    {
        set(key, {});
    }
    virtual void sync()
    {
        // no-op by default
    }
};
