// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "gsettings.h"

#include <gio/gio.h>

using namespace Qt::StringLiterals;

GLibSettingsBackend::GLibSettingsBackend(const QByteArray &schema)
    : m_schemaId(schema)
    , m_schema(g_settings_schema_source_lookup(g_settings_schema_source_get_default(), schema.constData(), true))
{
    if (m_schema) {
        m_settings = g_settings_new_full(m_schema, nullptr, nullptr);
    } else {
        Q_ASSERT_X(false, "gsettings", qPrintable(u"schema %1 is not installed"_s.arg(QString::fromUtf8(m_schemaId))));
    }
}

GLibSettingsBackend::~GLibSettingsBackend()
{
    g_clear_object(&m_settings);
    g_clear_pointer(&m_schema, g_settings_schema_unref);
}

void GLibSettingsBackend::set(const QString &key, const QVariant &value)
{
    const QByteArray name = key.toUtf8();

    if (!m_settings || !g_settings_schema_has_key(m_schema, name.constData())) {
        Q_ASSERT_X(false, "gsettings", qPrintable(u"%1 doesn't exist in %2"_s.arg(key, QString::fromUtf8(m_schemaId))));
        return;
    }

    if (value.isNull()) {
        g_settings_reset(m_settings, name.constData());
        return;
    }
    if (value.userType() == qMetaTypeId<GSettingsEnum>()) {
        const int newValue = value.value<GSettingsEnum>().value;
        if (g_settings_get_enum(m_settings, name.constData()) != newValue) {
            g_settings_set_enum(m_settings, name.constData(), newValue);
        }
        return;
    }

    GVariant *newValue = nullptr;
    switch (value.typeId()) {
    case QMetaType::QString:
        newValue = g_variant_new_string(value.toString().toUtf8().constData());
        break;
    case QMetaType::UInt:
        newValue = g_variant_new_uint32(value.toUInt());
        break;
    case QMetaType::Int:
        newValue = g_variant_new_int32(value.toInt());
        break;
    case QMetaType::Bool:
        newValue = g_variant_new_boolean(value.toBool());
        break;
    case QMetaType::Double:
        newValue = g_variant_new_double(value.toDouble());
        break;
    default:
        Q_UNREACHABLE();
    }
    g_variant_ref_sink(newValue);
    g_autoptr(GVariant) currentValue = g_settings_get_value(m_settings, name.constData());
    if (!g_variant_equal(currentValue, newValue)) {
        g_settings_set_value(m_settings, name.constData(), newValue);
    }
    g_variant_unref(newValue);
}

void GLibSettingsBackend::sync()
{
    g_settings_sync();
}
