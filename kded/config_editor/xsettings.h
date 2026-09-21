// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QHash>

#include "editorbackend.h"

// ~/.config/xsettingsd/xsettingsd.conf; sync() also makes xsettingsd reload it
class XSettingsBackend : public EditorBackend
{
public:
    void set(const QString &key, const QVariant &value) override;
    void sync() override;

private:
    QHash<QString, QVariant> m_pending;
};
