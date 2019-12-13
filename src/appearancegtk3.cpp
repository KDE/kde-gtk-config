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
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

#include <KSharedConfig>
#include <KConfigGroup>

#include <gio/gio.h>

#include "appearancegtk3.h"

QStringList AppearanceGTK3::installedThemes() const
{
    QFileInfoList availableThemes;
    for (const QString& themesDir : QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("themes"), QStandardPaths::LocateDirectory)) {
        QDir root(themesDir);
        availableThemes += root.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);
    }

    // Also show the user-installed themes
    QDir user(QDir::homePath() + QStringLiteral("/.themes"));
    availableThemes += user.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);

    // We just want actual themes
    QStringList themes;

    // Check that the theme contains a gtk-3.* subdirectory
    QStringList gtk3SubdirPattern(QStringLiteral("gtk-3.*"));
    for (const QFileInfo &it : availableThemes) {
        QDir themeDir(it.filePath());
        if(!themeDir.entryList(gtk3SubdirPattern, QDir::Dirs).isEmpty())
            themes += it.filePath();
    }

    return themes;
}

bool AppearanceGTK3::saveSettings(const KSharedConfig::Ptr& file) const
{
    KConfigGroup group(file, QStringLiteral("Settings"));

    group.writeEntry(QStringLiteral("gtk-theme-name"), m_settings["theme"]);

    const bool sync = group.sync();
    Q_ASSERT(sync);
    return true;
}

bool AppearanceGTK3::loadSettings(const KSharedConfig::Ptr& file)
{
    KConfigGroup group(file, QStringLiteral("Settings"));

    if (!file || !group.isValid()) {
        qWarning() << QStringLiteral("Cannot open the GTK3 config file") << file;
        return false;
    }

    m_settings.clear();

    m_settings[QStringLiteral("theme")] = group.readEntry(QStringLiteral("gtk-theme-name"));
    for(auto it = m_settings.begin(); it != m_settings.end(); ) {
        if (it.value().isEmpty()) {
            it = m_settings.erase(it);
        } else {
            ++it;
        }
    }
    return true;
}

QString AppearanceGTK3::configFileName() const
{
    return QStringLiteral("gtk-3.0/settings.ini");
}

QString AppearanceGTK3::defaultConfigFile() const
{
    QString root = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    if(root.isEmpty()) {
        root = QFileInfo(QDir::home(), QStringLiteral(".config")).absoluteFilePath();
    }

    return root + '/' + configFileName();
}

bool AppearanceGTK3::saveSettings(const QString& file) const
{
    auto cfg = KSharedConfig::openConfig(file, KConfig::NoGlobals);
    return saveSettings(cfg);
}

bool AppearanceGTK3::loadSettings(const QString& path)
{
    auto cfg = KSharedConfig::openConfig(path, KConfig::NoGlobals);
    return loadSettings(cfg);
}

bool AppearanceGTK3::loadSettings()
{
    auto cfg = KSharedConfig::openConfig(configFileName(), KConfig::NoGlobals);
    return loadSettings(cfg);
}

bool AppearanceGTK3::saveSettings() const
{
    // FIXME kdebz#387417
    // We should maybe use GSettings everywhere in future, but at this moment we
    // need this to have this configuration available in sandboxed applications which
    // is only possible through dconf
    g_autoptr(GSettings) gsettings = g_settings_new("org.gnome.desktop.interface");
    g_settings_set_string(gsettings, "gtk-theme", m_settings["theme"].toUtf8().constData());

    auto cfg = KSharedConfig::openConfig(configFileName(), KConfig::NoGlobals);
    return saveSettings(cfg);
}
