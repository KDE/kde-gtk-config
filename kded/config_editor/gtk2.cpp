// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "gtk2.h"

#include <QDir>
#include <QRegularExpression>

#include "utils.h"

using namespace Qt::StringLiterals;

namespace
{

QString gtkrcPath()
{
    QString path = qEnvironmentVariable("GTK2_RC_FILES", QDir::homePath() + u"/.gtkrc-2.0"_s);
    if (path.contains(":/"_L1)) { // I.e. env variable contains multiple paths
        path = QDir::homePath() + u"/.gtkrc-2.0"_s;
    }
    return path;
}

// Remove what older versions of this module used to write:
//   include "/usr/share/themes/Adwaita-dark/gtk-2.0/gtkrc"
// and
//   style "user-font"
//   {
//       font_name="Noto Sans Regular"
//   }
//   widget_class "*" style "user-font"
void removeLegacyStrings(QString &contents)
{
    static const QRegularExpression includeLine(u"include .*\n"_s);
    static const QRegularExpression userFontStyle(u"style(.|\n)*{(.|\n)*}\nwidget_class.*\"user-font\""_s);
    contents.remove(includeLine);
    contents.remove(userFontStyle);
}
}

void Gtk2Backend::set(const QString &key, const QVariant &value)
{
    m_pending.insert(key, value);
}

void Gtk2Backend::sync()
{
    if (m_pending.isEmpty()) {
        return;
    }

    const QString path = gtkrcPath();
    const QString original = Utils::readFile(path);
    QString contents = original;
    removeLegacyStrings(contents);
    for (auto it = m_pending.cbegin(); it != m_pending.cend(); it++) {
        Utils::setLine(contents, it.key(), "="_L1, it.value());
    }
    m_pending.clear();
    if (contents != original) {
        Utils::writeFile(path, contents);
    }
}
