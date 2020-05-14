/*
 * Copyright (C) 2020 Mikhail Zolotukhin <zomial@protonmail.com>
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
#include <QString>
#include <QVariant>
#include <QRegularExpression>

#include "configeditor.h"

QString gtk2Theme();
void upgradeGtk2Theme();
void upgradeGtk3Theme();

int main() {
    upgradeGtk2Theme();
    upgradeGtk3Theme();
    return 0;
}

void upgradeGtk2Theme() {
    QString currentGtk2Theme = gtk2Theme();
    if (currentGtk2Theme.isEmpty()
     || currentGtk2Theme == QStringLiteral("oxygen-gtk")
     || currentGtk2Theme == QStringLiteral("BreezyGTK")
     || currentGtk2Theme == QStringLiteral("Orion")
    ) {
        ConfigEditor::setGtk2ConfigValue(QStringLiteral("gtk-theme-name"), QStringLiteral("Breeze"));
    }
}

void upgradeGtk3Theme() {
    QString currentGtk3Theme = ConfigEditor::gtk3ConfigValueSettingsIni(QStringLiteral("gtk-theme-name"));
    if (currentGtk3Theme.isEmpty()
     || currentGtk3Theme == QStringLiteral("oxygen-gtk")
     || currentGtk3Theme == QStringLiteral("BreezyGTK")
     || currentGtk3Theme == QStringLiteral("Orion")
    ) {
        ConfigEditor::setGtk3ConfigValueGSettings(QStringLiteral("gtk-theme"), QStringLiteral("Breeze"));
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
