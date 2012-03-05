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

#include "appearancegtk3.h"
#include <KDebug>
#include <QFile>
#include <QDir>

QStringList AppearanceGTK3::installedThemes() const
{
    //TODO: port to kstandarddirs
    QDir root("/usr/share/themes");
    QFileInfoList availableThemes = root.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs);

    //Also show the user-installed themes
    QDir user(QDir::homePath()+"/.themes");
    availableThemes += user.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs);

    //we just want actual themes
    QStringList themes;
    for(QFileInfoList::iterator it=availableThemes.begin(); it!=availableThemes.end(); ++it) {
        bool hasGtkrc = QDir(it->filePath()).exists("gtk-3.0");

        //If it doesn't exist, we don't want it on the list
        if(hasGtkrc)
            themes += it->filePath();
    }

    return themes;
}

bool AppearanceGTK3::saveSettings(const QString& file)
{
    //Opening GTK3 config file $ENV{XDG_CONFIG_HOME}/gtk-3.0/m_settings.ini
    QDir::home().mkpath(file.left(file.lastIndexOf('/'))); //we make sure the path exists
    QFile file_gtk3(file);
    
    if(!file_gtk3.open(QIODevice::WriteOnly | QIODevice::Text)) {
        kDebug() << "Couldn't open GTK3 config file for writing at:" << file_gtk3.fileName();
        return false;
    }
    
    QTextStream flow3(&file_gtk3);
    flow3 << "[Settings]\n";
    flow3 << "gtk-font-name=" << m_settings["font"] << "\n";
    flow3 << "gtk-theme-name=" << m_settings["theme"] << "\n";
    flow3 << "gtk-icon-theme-name= "<< m_settings["icon"] << "\n";
    flow3 << "gtk-fallback-icon-theme=" << m_settings["icon_fallback"] << "\n";
    flow3 << "gtk-toolbar-style=" << m_settings["toolbar_style"] << "\n";
    flow3 << "gtk-menu-images=" << m_settings["show_icons_buttons"] << "\n";
    flow3 << "gtk-button-images=" << m_settings["show_icons_menus"] << "\n";
    
    return true;
}

bool AppearanceGTK3::loadSettings(const QString& path)
{
    QFile fileGtk3(path);
    bool canRead=fileGtk3.open(QIODevice::ReadOnly | QIODevice::Text);
    
    if(canRead) {
        QTextStream flow(&fileGtk3);

        QMap<QString, QString> foundSettings = readSettingsTuples(flow.readAll());
        
        m_settings["theme"] = foundSettings["gtk-theme-name"];
        m_settings["icon"] = foundSettings["gtk-icon-theme-name"];
        m_settings["icon_fallback"] = foundSettings["gtk-fallback-icon-theme"];
        m_settings["font"] = foundSettings["gtk-font-name"];
        
        m_settings["toolbar_style"] = readMapDefaultValue(foundSettings, "gtk-toolbar-style", "GTK_TOOLBAR_ICONS");
        m_settings["show_icons_buttons"] = readMapDefaultValue(foundSettings, "gtk-button-images", "0");
        m_settings["show_icons_menus"] = readMapDefaultValue(foundSettings, "gtk-menu-images", "0");
    } else
        kDebug() << "Cannot open the GTK3 config file" << path;
    
    return canRead;
}

QString AppearanceGTK3::defaultConfigFile() const
{
    QString root = qgetenv("XDG_CONFIG_HOME");
    if(root.isEmpty())
        root = QFileInfo(QDir::home(), ".config").absoluteFilePath();
    
    return root+"/gtk-3.0/settings.ini";
}