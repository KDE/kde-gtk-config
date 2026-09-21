// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "utils.h"

#include <QFile>
#include <QStandardPaths>

#include <algorithm>

using namespace Qt::StringLiterals;

namespace Utils
{

QString configDirPath(int gtkVersion)
{
    return u"%1/gtk-%2.0"_s.arg(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation), QString::number(gtkVersion));
}

QString readFile(const QString &path)
{
    QFile file(path);
    return file.open(QIODevice::ReadOnly | QIODevice::Text) ? QString::fromUtf8(file.readAll()) : QString();
}

void writeFile(const QString &path, const QString &contents)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        file.write(contents.toUtf8());
    }
}

bool updateFile(const QString &path, const QByteArray &contents)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly) && file.readAll() == contents) {
        return false;
    }
    file.close();
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    file.write(contents);
    return true;
}

bool setLine(QString &contents, const QString &key, QLatin1StringView sep, const QVariant &value)
{
    const QString prefix = key + sep;
    QString line;
    if (value.typeId() == QMetaType::QString) {
        line = prefix + u'"' + value.toString() + u'"';
    } else if (value.typeId() == QMetaType::Bool) {
        // neither gtkrc nor xsettingsd support 'true' and 'false' as values
        line = prefix + QString::number(value.toInt());
    } else if (!value.isNull()) {
        line = prefix + value.toString();
    }

    const auto hasKey = [&prefix](const QString &line) {
        return line.startsWith(prefix);
    };
    QStringList lines = contents.split(u'\n');
    auto it = std::find_if(lines.begin(), lines.end(), hasKey);
    if (it == lines.end()) {
        if (line.isNull()) {
            return false;
        }
        lines.prepend(line);
    } else {
        if (line.isNull()) {
            it = lines.erase(it);
        } else {
            *it++ = line;
        }
        lines.erase(std::remove_if(it, lines.end(), hasKey), lines.end()); // duplicates
    }

    const QString result = lines.join(u'\n');
    if (result == contents) {
        return false;
    }
    contents = result;
    return true;
}
}
