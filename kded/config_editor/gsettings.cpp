// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "gsettings.h"

#include <gio/gio.h>

namespace GSettingsEditor
{
bool checkParamExists(const char *paramName, const char *category)
{
    GSettingsSchemaSource *gSettingsSchemaSource = g_settings_schema_source_get_default();
    g_autoptr(GSettingsSchema) gSettingsSchema = g_settings_schema_source_lookup(gSettingsSchemaSource, category, true);

    return gSettingsSchema && g_settings_schema_has_key(gSettingsSchema, paramName);
}

void setValue(const char *paramName, const QVariant &paramValue, const char *category)
{
    if (!checkParamExists(paramName, category)) {
        Q_ASSERT_X(false, "gsettings", QLatin1String("%1 doesn't exist in %2").arg(paramName, category).toLatin1().constData());
        return;
    }

    g_autoptr(GSettings) gsettings = g_settings_new(category);

    if (paramValue.type() == QVariant::Type::String) {
        g_settings_set_string(gsettings, paramName, paramValue.toString().toUtf8().constData());
    } else if (paramValue.type() == QVariant::Type::Int) {
        g_settings_set_int(gsettings, paramName, paramValue.toInt());
    } else if (paramValue.type() == QVariant::Type::Bool) {
        g_settings_set_boolean(gsettings, paramName, paramValue.toBool());
    } else if (paramValue.type() == QVariant::Type::Double) {
        g_settings_set_double(gsettings, paramName, paramValue.toDouble());
    }

    g_settings_sync();
}

void setValueAsEnum(const char *paramName, int paramValue, const char *category)
{
    if (!checkParamExists(paramName, category)) {
        Q_ASSERT_X(false, "gsettings", QLatin1String("%1 doesn't exist in %2").arg(paramName, category).toLatin1().constData());
        return;
    }

    g_autoptr(GSettings) gsettings = g_settings_new(category);
    g_settings_set_enum(gsettings, paramName, paramValue);
    g_settings_sync();
}
}
