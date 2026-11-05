// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QFile>
#include <QString>

namespace Utils
{
QString configDirPath(int gtkVersion);
QString readFileContents(QFile &file);
// GTK2 can't use dark variants automatically, so returns the dark theme folder if available
QString gtk2ThemeName(const QString &themeName, bool preferDarkTheme);

[[maybe_unused]] static inline const auto s_gtkVersions = {3, 4};
}
