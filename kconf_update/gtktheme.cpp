/*
 * SPDX-FileCopyrightText: 2020 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QDir>
#include <QRegularExpression>
#include <QString>
#include <QVariant>

#include "configeditor.h"

QString gtk2Theme();
void upgradeGtk2Theme();
void upgradeGtk3Theme();

int main()
{
    upgradeGtk2Theme();
    upgradeGtk3Theme();
    return 0;
}

void upgradeGtk2Theme()
{
    QString currentGtk2Theme = gtk2Theme();
    if (currentGtk2Theme.isEmpty() //
        || currentGtk2Theme == QStringLiteral("oxygen-gtk") //
        || currentGtk2Theme == QStringLiteral("BreezyGTK") //
        || currentGtk2Theme == QStringLiteral("Orion")) {
        ConfigEditor::setGtk2ConfigValue(QStringLiteral("gtk-theme-name"), QStringLiteral("Breeze"));
    }
}

void upgradeGtk3Theme()
{
    QString currentGtk3Theme = ConfigEditor::gtk3ConfigValueSettingsIni(QStringLiteral("gtk-theme-name"));
    if (currentGtk3Theme.isEmpty() //
        || currentGtk3Theme == QStringLiteral("oxygen-gtk") //
        || currentGtk3Theme == QStringLiteral("BreezyGTK") //
        || currentGtk3Theme == QStringLiteral("Orion")) {
        ConfigEditor::setGtkConfigValueGSettings(QStringLiteral("gtk-theme"), QStringLiteral("Breeze"));
        ConfigEditor::setGtk3ConfigValueSettingsIni(QStringLiteral("gtk-theme-name"), QStringLiteral("Breeze"));
        ConfigEditor::setGtk3ConfigValueXSettingsd(QStringLiteral("Net/ThemeName"), QStringLiteral("Breeze"));
    }
}

QString gtk2Theme()
{
    QString gtkrcPath = QDir::homePath() + QStringLiteral("/.gtkrc-2.0");
    QFile gtkrc(gtkrcPath);
    if (gtkrc.open(QIODevice::ReadWrite | QIODevice::Text)) {
        const QRegularExpression regExp(QStringLiteral("gtk-theme-name=[^\n]*($|\n)"));
        while (!gtkrc.atEnd()) {
            QString line = gtkrc.readLine();
            if (line.contains(regExp)) {
                return line.split('"')[1];
            }
        }
    }

    return QString();
}
