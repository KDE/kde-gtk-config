// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "xsettings.h"

#include <QDir>
#include <QProcess>
#include <QStandardPaths>

#include <csignal>
#include <unistd.h>

#include "utils.h"

using namespace Qt::StringLiterals;

namespace
{

pid_t pidOfXSettingsd()
{
    QProcess pgrep;
    pgrep.start(u"pgrep"_s,
                {
                    u"-u"_s,
                    QString::number(getuid()),
                    u"-n"_s, // select most recently started
                    u"xsettingsd"_s,
                });
    pgrep.waitForFinished();
    return QString::fromUtf8(pgrep.readAllStandardOutput()).trimmed().toInt();
}

void reloadXSettingsd()
{
    const pid_t pid = pidOfXSettingsd();
    if (pid == 0) {
        QProcess::startDetached(QStandardPaths::findExecutable(u"xsettingsd"_s), {});
    } else {
        kill(pid, SIGHUP);
    }
}

}

void XSettingsBackend::set(const QString &key, const QVariant &value)
{
    m_pending.insert(key, value);
}

void XSettingsBackend::sync()
{
    if (m_pending.isEmpty()) {
        return;
    }

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + u"/xsettingsd"_s);
    if (!dir.exists()) {
        dir.mkpath(u"."_s);
    }
    const QString path = dir.filePath(u"xsettingsd.conf"_s);

    const QString original = Utils::readFile(path);
    QString contents = original;
    for (auto it = m_pending.cbegin(); it != m_pending.cend(); it++) {
        Utils::setLine(contents, it.key(), " "_L1, it.value());
    }
    m_pending.clear();
    if (contents == original) {
        return;
    }
    Utils::writeFile(path, contents);
    reloadXSettingsd();
}
