// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "gsettings.h"

#include <gio/gio.h>

namespace GSettingsEditor
{
void setValue(const QString &paramName, const QVariant &paramValue, const QString &category)
{
    g_autoptr(GSettings) gsettings = g_settings_new(category.toUtf8().constData());

    if (paramValue.type() == QVariant::Type::String) {
        g_settings_set_string(gsettings, paramName.toUtf8().constData(), paramValue.toString().toUtf8().constData());
    } else if (paramValue.type() == QVariant::Type::Int) {
        g_settings_set_int(gsettings, paramName.toUtf8().constData(), paramValue.toInt());
    } else if (paramValue.type() == QVariant::Type::Bool) {
        g_settings_set_boolean(gsettings, paramName.toUtf8().constData(), paramValue.toBool());
    }

    g_settings_sync();
}

void setValueAsEnum(const QString &paramName, int paramValue, const QString &category)
{
    g_autoptr(GSettings) gsettings = g_settings_new(category.toUtf8().constData());
    g_settings_set_enum(gsettings, paramName.toUtf8().constData(), paramValue);
    g_settings_sync();
}
}
