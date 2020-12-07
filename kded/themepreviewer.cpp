/*
 * SPDX-FileCopyrightText: 2019 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QProcessEnvironment>
#include <QStandardPaths>

#include "themepreviewer.h"
#include "config.h"

const QString ThemePreviewer::gtk3PreviewerExecutablePath = QStandardPaths::findExecutable(QStringLiteral("gtk3_preview"), {CMAKE_INSTALL_FULL_LIBEXECDIR});

ThemePreviewer::ThemePreviewer(QObject *parent) : QObject(parent), gtk3PreviewerProcess()
{
}

void ThemePreviewer::showGtk3App(const QString& themeName)
{
    if (gtk3PreviewerProcess.state() == QProcess::ProcessState::NotRunning) {
        QProcessEnvironment gtk3PreviewEnvironment = QProcessEnvironment::systemEnvironment();
        gtk3PreviewEnvironment.insert(QStringLiteral("GTK_THEME"), themeName);
        gtk3PreviewerProcess.setProcessEnvironment(gtk3PreviewEnvironment);

        gtk3PreviewerProcess.start(gtk3PreviewerExecutablePath, QStringList());
    } else {
        gtk3PreviewerProcess.close();
    }
}
