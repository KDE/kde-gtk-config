/* KDE GTK Configuration Module
 * 
 * Copyright 2011 José Antonio Sanchez Reynaga <joanzare@gmail.com>
 * Copyright 2011 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QFile>
#include <QStringList>
#include <QDir>
#include <QDirIterator>
#include <QDebug>
#include <QProcess>
#include <QStandardPaths>
#include <QRegularExpression>

#include "config.h"
#include "appearancegtk2.h"

bool AppearanceGTK2::loadSettingsPrivate(const QString &path)
{
    QFile configFile(path);
    
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    const QMap<QString, QString> foundSettings = readSettingsTuples(&configFile);

    for(auto it = foundSettings.constBegin(), itEnd = foundSettings.constEnd(); it!=itEnd; ++it) {
        if (it.key() == QStringLiteral("gtk-theme-name")) {
            m_settings[QStringLiteral("theme")] = *it;
        }
    }
    return true;
}

QString AppearanceGTK2::themesGtkrcFile(const QString &themeName) const
{
    QStringList themes = installedThemes();
    themes = themes.filter(QRegExp("/" + themeName + "/?$"));
    if (themes.size() == 1) {
        QDirIterator it(themes.first(), QDirIterator::Subdirectories);
        while(it.hasNext()) {
            it.next();
            if(it.fileName() == "gtkrc") {
                return it.filePath();
            }
        }
    }
    
    return QString();
}

bool AppearanceGTK2::saveSettingsPrivate(const QString &gtkrcFile) const
{
    QFile gtkrc(gtkrcFile);

    if (gtkrc.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QString fileContents = gtkrc.readAll();

        modifyGtkrcContents(fileContents);

        gtkrc.remove();
        gtkrc.open(QIODevice::WriteOnly | QIODevice::Text);
        gtkrc.write(fileContents.toUtf8());

        if (QFileInfo(gtkrc).filePath() == defaultConfigFile()) {
            QProcess::startDetached(QStandardPaths::findExecutable("reload_gtk_apps", {CMAKE_INSTALL_FULL_LIBEXECDIR}));
        }

        return true;
    } else {
        qWarning() << "Unable to write the .gtkrc-2.0 file";
        return false;
    }
}

void AppearanceGTK2::modifyGtkrcContents(QString &fileContents) const
{
    modifyGtkrcProperty("gtk-theme-name", m_settings["theme"], fileContents);
    removeGtkrcLegacyContents(fileContents);
}

void AppearanceGTK2::modifyGtkrcProperty(const QString &propertyName, const QString &newValue, QString &fileContents) const
{
    const QRegularExpression regExp{propertyName + "=[^\n]*($|\n)"};

    static const QStringList nonStringProperties{
        "gtk-toolbar-style",
        "gtk-menu-images",
        "gtk-button-images",
        "gtk-primary-button-warps-slider",
    };

    QString newConfigString;
    if (nonStringProperties.contains(propertyName)) {
        newConfigString = propertyName + "=" + newValue + "\n";
    } else {
        newConfigString = propertyName + "=\"" + newValue + "\"\n";
    }
    
    if (fileContents.contains(regExp)) {
        fileContents.replace(regExp, newConfigString);
    } else {
        fileContents = newConfigString + "\n" + fileContents;
    }
}

void AppearanceGTK2::removeGtkrcLegacyContents(QString &fileContents) const
{
    // Remove "include" lines
    // Example:
    // include "/usr/share/themes/Adwaita-dark/gtk-2.0/gtkrc"

    static const QRegularExpression includeRegExp(QStringLiteral("include .*\n"));
    fileContents.remove(includeRegExp);

    // Remove redundant font config lines
    // Example:
    // style "user-font"
    // {
    //     font_name="Noto Sans Regular"
    // }
    // widget_class "*" style "user-font"

    static const QRegularExpression userFontStyleRegexp(QStringLiteral("style(.|\n)*{(.|\n)*}\nwidget_class.*\"user-font\""));
    fileContents.remove(userFontStyleRegexp);
}

void AppearanceGTK2::reset()
{
    m_settings.clear();
}

QString AppearanceGTK2::defaultConfigFile() const
{
    return QDir::homePath() + QStringLiteral("/.gtkrc-2.0");
}

QStringList AppearanceGTK2::installedThemes() const
{
    QFileInfoList availableThemes;
    for (const QString& themesDir : QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "themes", QStandardPaths::LocateDirectory)) {
        QDir root(themesDir);
        availableThemes += root.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);
    }
    
    // Check if there are themes installed by the user
    QDir user(QDir::homePath() + "/.themes");
    availableThemes += user.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);

    // We just want actual themes
    QStringList paths;
    for (const QFileInfo &it : availableThemes) {
        bool hasGtkrc = QDir(it.filePath()).exists("gtk-2.0");

        // If it doesn't exist, we don't want it on the list
        if (hasGtkrc) {
            paths += it.filePath();
        }
    }

    return paths;
}

bool AppearanceGTK2::loadSettings()
{
    reset();

    bool b = loadSettingsPrivate(QStringLiteral("/etc/gtk-2.0/gtkrc"));
    b |= loadSettingsPrivate(defaultConfigFile());
    return b;
}

bool AppearanceGTK2::saveSettings() const
{
    return saveSettings(defaultConfigFile());
}

bool AppearanceGTK2::loadSettings(const QString &gtkrcFile)
{
    reset();
    return loadSettingsPrivate(gtkrcFile);
}

bool AppearanceGTK2::saveSettings(const QString &gtkrcFile) const
{
    return saveSettingsPrivate(gtkrcFile);
}
