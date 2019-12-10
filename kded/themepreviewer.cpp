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

#include <QDir>
#include <QFile>
#include <QObject>
#include <QProcess>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QString>
#include <QScopedPointer>

#include <KConfigGroup>
#include <KSharedConfig>

#include <signal.h>

#include "themepreviewer.h"
#include "configeditor.h"
#include "config.h"

const QString ThemePreviewer::previewGtk2ConfigPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QStringLiteral("/gtkrc-2.0");
const QString ThemePreviewer::currentGtk2ConfigPath = QDir::homePath() + QStringLiteral("/.gtkrc-2.0");
const QString ThemePreviewer::gtk2PreviewerExecutablePath = QStandardPaths::findExecutable(QStringLiteral("gtk_preview"), {CMAKE_INSTALL_FULL_LIBEXECDIR});

const QString ThemePreviewer::gtk3PreviewerExecutablePath = QStandardPaths::findExecutable(QStringLiteral("gtk3_preview"), {CMAKE_INSTALL_FULL_LIBEXECDIR});

ThemePreviewer::ThemePreviewer(QObject *parent) : QObject(parent),
gtk2PreviewerProccess(),
gtk3PreviewerProccess()
{
    QProcessEnvironment gtk2PreviewEnvironment = QProcessEnvironment::systemEnvironment();
    gtk2PreviewEnvironment.insert(QStringLiteral("GTK2_RC_FILES"), previewGtk2ConfigPath);

    gtk2PreviewerProccess.setProcessEnvironment(gtk2PreviewEnvironment);
    connect(&gtk2PreviewerProccess, SIGNAL(finished(int)), this, SLOT(startXsettingsd()));
}

void ThemePreviewer::showGtk2App(const QString& themeName)
{
    if (gtk2PreviewerProccess.state() == QProcess::ProcessState::NotRunning) {
        if (QFile::exists(previewGtk2ConfigPath)) {
            QFile::remove(previewGtk2ConfigPath);
        }

        QFile::copy(currentGtk2ConfigPath, previewGtk2ConfigPath);

        QFile previewConfig(previewGtk2ConfigPath);
        QString previewConfigContents = ConfigEditor::readFileContents(previewConfig);
        ConfigEditor::replaceValueInGtkrcContents(previewConfigContents, QStringLiteral("gtk-theme-name"), themeName);
        previewConfig.remove();
        previewConfig.open(QIODevice::WriteOnly | QIODevice::Text);
        previewConfig.write(previewConfigContents.toUtf8());

        stopXsettingsd();
        gtk2PreviewerProccess.start(gtk2PreviewerExecutablePath);
    } else {
        gtk2PreviewerProccess.close();
    }
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

void ThemePreviewer::startXsettingsd()
{
    if (gtk2PreviewerProccess.state() == QProcess::ProcessState::NotRunning &&
        gtk3PreviewerProccess.state() == QProcess::ProcessState::NotRunning) {
        QProcess::startDetached(QStandardPaths::findExecutable(QStringLiteral("xsettingsd")));
    }
}

void ThemePreviewer::stopXsettingsd()
{
    pid_t pidOfXSettingsd = ConfigEditor::pidOfXSettingsd();
    if (pidOfXSettingsd > 0) {
        kill(pidOfXSettingsd, SIGTERM);
    }
}
