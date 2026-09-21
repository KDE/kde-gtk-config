// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include "editorbackend.h"

typedef struct _GSettings GSettings;
typedef struct _GSettingsSchema GSettingsSchema;

// Wrap a value in this to write it with g_settings_set_enum()
struct GSettingsEnum {
    int value;
    bool operator==(const GSettingsEnum &) const = default;
};
Q_DECLARE_METATYPE(GSettingsEnum)

// Keys of one GSettings schema, e.g. "org.gnome.desktop.interface"
class GLibSettingsBackend : public EditorBackend
{
public:
    explicit GLibSettingsBackend(const QByteArray &schema);
    ~GLibSettingsBackend() override;

    void set(const QString &key, const QVariant &value) override;
    void sync() override;

private:
    QByteArray m_schemaId;
    GSettingsSchema *m_schema; // null if not installed
    GSettings *m_settings = nullptr;
};
