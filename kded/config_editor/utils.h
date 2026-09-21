// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QLatin1StringView>
#include <QString>
#include <QVariant>

#include <array>

namespace Utils
{

inline constexpr std::array s_gtkVersions{3, 4};

// ~/.config/gtk-N.0
QString configDirPath(int gtkVersion);

QString readFile(const QString &path);
void writeFile(const QString &path, const QString &contents);
// Writes the file unless it already has these contents; returns whether it was written
bool updateFile(const QString &path, const QByteArray &contents);

// Replaces or adds a "<key><sep><value>" line, a null value removes it; returns whether the contents changed
bool setLine(QString &contents, const QString &key, QLatin1StringView sep, const QVariant &value);
}
