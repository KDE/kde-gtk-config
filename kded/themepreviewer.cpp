/*
 * Copyright (C) 2019 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QProcess>
#include <QProcessEnvironment>
#include <QStandardPaths>

#include "themepreviewer.h"
#include "config.h"

const QString ThemePreviewer::gtk3PreviewerExecutablePath = QStandardPaths::findExecutable(QStringLiteral("gtk3_preview"), {CMAKE_INSTALL_FULL_LIBEXECDIR});

ThemePreviewer::ThemePreviewer(QObject *parent) : QObject(parent),
gtk3PreviewerProccess()
{
}

void ThemePreviewer::showGtk3App(const QString& themeName)
{
    if (gtk3PreviewerProccess.state() == QProcess::ProcessState::NotRunning) {
        QProcessEnvironment gtk3PreviewEnvironment = QProcessEnvironment::systemEnvironment();
        gtk3PreviewEnvironment.insert(QStringLiteral("GTK_THEME"), themeName);
        gtk3PreviewerProccess.setProcessEnvironment(gtk3PreviewEnvironment);

        gtk3PreviewerProccess.start(gtk3PreviewerExecutablePath);
    } else {
        gtk3PreviewerProccess.close();
    }
}
