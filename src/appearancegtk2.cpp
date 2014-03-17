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

#include "appearancegtk2.h"
#include <QFile>
#include <QStringList>
#include <QDir>
#include <QDirIterator>
#include <QDebug>
#include <QProcess>
#include <QStandardPaths>

bool AppearanceGTK2::loadSettings(const QString& path)
{
    QFile configFile(path);
    
    bool canRead = configFile.open(QIODevice::ReadOnly | QIODevice::Text);
    
    if(canRead) {
//         qDebug() << "The gtk2 config file exists...";
        const QMap<QString, QString> foundSettings = readSettingsTuples(&configFile);

        m_settings["theme"] = foundSettings["gtk-theme-name"];
        m_settings["icon"] = foundSettings["gtk-icon-theme-name"];
        m_settings["icon_fallback"] = foundSettings["gtk-fallback-icon-theme"];
        m_settings["font"] = foundSettings["gtk-font-name"];
        m_settings["toolbar_style"] = foundSettings.value("gtk-toolbar-style", "GTK_TOOLBAR_ICONS");
        m_settings["show_icons_buttons"] = foundSettings.value("gtk-button-images", "0");
        m_settings["show_icons_menus"] = foundSettings.value("gtk-menu-images", "0");
    }
    
    return canRead;
}

QString AppearanceGTK2::themesGtkrcFile(const QString& themeName) const
{
    QStringList themes=installedThemes();
    themes=themes.filter(QRegExp("/"+themeName+"/?$"));
    if(themes.size()==1) {
        QDirIterator it(themes.first(), QDirIterator::Subdirectories);
        while(it.hasNext()) {
            it.next();
            if(it.fileName()=="gtkrc") {
//                 qDebug() << "\tgtkrc file found at : " << it.filePath();
                return it.filePath();
            }
        }
    }
    
    return QString();
}

bool AppearanceGTK2::saveSettings(const QString& gtkrcFile) const
{
    QFile gtkrc(gtkrcFile);
    gtkrc.remove();

    if(!gtkrc.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "There was unable to write the file .gtkrc-2.0";
        return false;
    }

    QTextStream flow(&gtkrc);

    flow << "# File created by KDE Gtk Config" << "\n"
         << "# Configs for GTK2 programs \n\n";

    QString themeGtkrcFile=themesGtkrcFile(getTheme());
    
    //TODO: is this really needed?
    if(!themeGtkrcFile.isEmpty())
        flow << "include \"" << themeGtkrcFile << "\"\n"; //We include the theme's gtkrc file
    
    if(QFile::exists("/etc/gtk-2.0/gtkrc"))
        flow  << "include \"/etc/gtk-2.0/gtkrc\"\n"; //We include the /etc's config file

    int nameEnd = m_settings["font"].lastIndexOf(QRegExp(" ([0-9]+|bold|italic)"));
    QString fontFamily = m_settings["font"].left(nameEnd);

    //TODO: is this really needed?
    flow << "style \"user-font\" \n"
            << "{\n"
            << "\tfont_name=\""<< fontFamily << "\"\n"
            << "}\n";
    
    flow << "widget_class \"*\" style \"user-font\"\n";
    flow << "gtk-font-name=\"" << m_settings["font"] << "\"\n";
    flow << "gtk-theme-name=\"" << m_settings["theme"] << "\"\n";
    flow << "gtk-icon-theme-name=\""<< m_settings["icon"] << "\"\n";
    flow << "gtk-fallback-icon-theme=\"" << m_settings["icon_fallback"] << "\"\n";
    flow << "gtk-toolbar-style=" << m_settings["toolbar_style"] << "\n";
    flow << "gtk-menu-images=" << m_settings["show_icons_menus"] << "\n";
    flow << "gtk-button-images=" << m_settings["show_icons_buttons"] << "\n";
    
    //we're done with the  ~/.gtk-2.0 file
    gtkrc.close();
    
    //TODO: do we really need the linked file?
    if(QFile::remove(gtkrcFile+"-kde4"))
        qDebug() << "ready to create the symbolic link";
    
    if( !QFile::link(gtkrcFile, gtkrcFile+"-kde4") )
        qWarning() << "Couldn't create the symboling link to .gtkrc-2.0-kde4 :(";
//     else
//         qDebug() << "Symbolic link created for .gtkrc-2.0-kde4 :D";
    
    if(gtkrcFile==defaultConfigFile())
        QProcess::startDetached(QStandardPaths::findExecutable("reload_gtk_apps"));
    
    return true;
}

QString AppearanceGTK2::defaultConfigFile() const
{
    return QDir::homePath()+"/.gtkrc-2.0";
}

QStringList AppearanceGTK2::installedThemes() const
{
    QFileInfoList availableThemes;
    foreach(const QString& themesDir, QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "themes", QStandardPaths::LocateDirectory)) {
        QDir root(themesDir);
        availableThemes += root.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs);
    }
    
    //Check if there are themes installed by the user
    QDir user(QDir::homePath()+"/.themes");
    availableThemes += user.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs);

    //we just want actual themes
    QStringList paths;
    for(QFileInfoList::const_iterator it=availableThemes.constBegin(); it!=availableThemes.constEnd(); ++it) {
        bool hasGtkrc = QDir(it->filePath()).exists("gtk-2.0");

        //If it doesn't exist, we don't want it on the list
        if(hasGtkrc)
            paths += it->filePath();
    }

    return paths;
}
