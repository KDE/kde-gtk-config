// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "gsettings.h"

#include <gio/gio.h>

namespace GSettingsEditor
{
bool checkParamExists(const QString &paramName, const QString &category)
{
    GSettingsSchemaSource *gSettingsSchemaSource = g_settings_schema_source_get_default();
    g_autoptr(GSettingsSchema) gSettingsSchema = g_settings_schema_source_lookup(gSettingsSchemaSource, category.toUtf8().constData(), true);

    return gSettingsSchema && g_settings_schema_has_key(gSettingsSchema, paramName.toUtf8().constData());
}

void setValue(const QString &paramName, const QVariant &paramValue, const QString &category)
{
    if (!checkParamExists(paramName, category)) {
        Q_ASSERT_X(false, "gsettings", QLatin1String("%1 doesn't exist in %2").arg(paramName, category).toLatin1().constData());
        return;
    }

    g_autoptr(GSettings) gsettings = g_settings_new(category.toUtf8().constData());

    if (paramValue.type() == QVariant::Type::String) {
        g_settings_set_string(gsettings, paramName.toUtf8().constData(), paramValue.toString().toUtf8().constData());
    } else if (paramValue.type() == QVariant::Type::Int) {
        g_settings_set_int(gsettings, paramName.toUtf8().constData(), paramValue.toInt());
    } else if (paramValue.type() == QVariant::Type::Bool) {
        g_settings_set_boolean(gsettings, paramName.toUtf8().constData(), paramValue.toBool());
    } else if (paramValue.type() == QVariant::Type::Double) {
        g_settings_set_double(gsettings, paramName.toUtf8().constData(), paramValue.toDouble());
    }

    g_settings_sync();
}

void setValueAsEnum(const QString &paramName, int paramValue, const QString &category)
{
    if (!checkParamExists(paramName, category)) {
        Q_ASSERT_X(false, "gsettings", QLatin1String("%1 doesn't exist in %2").arg(paramName, category).toLatin1().constData());
        return;
    }

    g_autoptr(GSettings) gsettings = g_settings_new(category.toUtf8().constData());
    g_settings_set_enum(gsettings, paramName.toUtf8().constData(), paramValue);
    g_settings_sync();
}
}
