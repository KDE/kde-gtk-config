/*
 * SPDX-FileCopyrightText: 2020 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QDir>
#include <QRegularExpression>
#include <QString>
#include <QVariant>

#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>

#include <gio/gio.h>

#include "config_editor/gsettings.h"
#include "config_editor/gtk2.h"
#include "config_editor/settings_ini.h"
#include "config_editor/xsettings.h"

QString gtk2Theme();
void upgradeGtk2Theme(const QString &defaultTheme);
void upgradeGtk3Theme(const QString &defaultTheme);

int main()
{
    QString defaultTheme = QStringLiteral("Breeze");

    KSharedConfig::Ptr globalConfig = KSharedConfig::openConfig();
    if (globalConfig) {
        auto group = globalConfig->group(QStringLiteral("KDE"));
        defaultTheme = group.readEntry(QStringLiteral("gtkTheme"), QStringLiteral("Breeze"));
    }

    upgradeGtk2Theme(defaultTheme);
    upgradeGtk3Theme(defaultTheme);
    g_settings_sync();
    return 0;
}

void upgradeGtk2Theme(const QString &defaultTheme)
{
    QString currentGtk2Theme = gtk2Theme();
    if (currentGtk2Theme.isEmpty() //
        || currentGtk2Theme == QStringLiteral("oxygen-gtk") //
        || currentGtk2Theme == QStringLiteral("BreezyGTK") //
        || currentGtk2Theme == QStringLiteral("Orion")) {
        Gtk2ConfigEditor::setValue(QStringLiteral("gtk-theme-name"), defaultTheme);
    }
}

void upgradeGtk3Theme(const QString &defaultTheme)
{
    QString currentGtk3Theme = SettingsIniEditor::value(QStringLiteral("gtk-theme-name"), 3);
    if (currentGtk3Theme.isEmpty() //
        || currentGtk3Theme == QStringLiteral("oxygen-gtk") //
        || currentGtk3Theme == QStringLiteral("BreezyGTK") //
        || currentGtk3Theme == QStringLiteral("Orion")) {
        GSettingsEditor::setValue("gtk-theme", defaultTheme);
        SettingsIniEditor::setValue(QStringLiteral("gtk-theme-name"), defaultTheme, 3);
        XSettingsEditor::setValue(QStringLiteral("Net/ThemeName"), defaultTheme);
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
