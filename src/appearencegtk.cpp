/* KDE GTK Configuration Module
 * 
 * Copyright 2011 José Antonio Sanchez Reynaga <joanzare@gmail.com>
 * Copyright 2011 Aleix Pol Gonzalez <aleixpol@kde.org>
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

#include "appearencegtk.h"
#include <kdebug.h>
#include <qdiriterator.h>
#include "abstractappearance.h"
#include "appearancegtk2.h"
#include "appearancegtk3.h"

AppearenceGTK::AppearenceGTK()
{
    m_app << new AppearanceGTK2;
    m_app << new AppearanceGTK3;
}

#define PROPERTY_IMPLEMENTATION(type, name, propname)\
void AppearenceGTK::set##name(const type& a) { foreach(AbstractAppearance* app, m_app) app->set##name(a); }\
type AppearenceGTK::get##name() const { return m_app.first()->get##name(); }

PROPERTY_IMPLEMENTATION(QString, Icon, "icon")
PROPERTY_IMPLEMENTATION(QString, IconFallback, "icon_fallback")
PROPERTY_IMPLEMENTATION(QString, Font, "font")
PROPERTY_IMPLEMENTATION(QString, ToolbarStyle, "toolbar_style")
PROPERTY_IMPLEMENTATION(bool, ShowIconsInButtons, "show_icons_buttons")
PROPERTY_IMPLEMENTATION(bool, ShowIconsInMenus, "show_icons_menus")

QString AppearenceGTK::getTheme() const { return gtk2Appearance()->getTheme(); }
void AppearenceGTK::setTheme(const QString& name) {  return gtk2Appearance()->setTheme(name); }
QString AppearenceGTK::getThemeGtk3() const { return gtk3Appearance()->getTheme(); }
void AppearenceGTK::setThemeGtk3(const QString& name) {  return gtk3Appearance()->setTheme(name); }

QStringList AppearenceGTK::getAvaliableIcons()
{
    QStringList paths = getAvaliableIconsPaths();
    QStringList ret;
    
    foreach(const QString& s, paths)
        ret << QDir(s).dirName();

    return ret;
}

QStringList AppearenceGTK::getAvaliableIconsPaths()
{
    QStringList availableIcons;

    //List existing folders in the root themes directory
    //TODO: Use KStandardDirs
    QDir root("/usr/share/icons");
    QDirIterator iter(root.path(), QDir::AllDirs|QDir::NoDotAndDotDot|QDir::NoSymLinks);
    while(iter.hasNext()) {
        QString currentPath = iter.next();
        QDir dir(currentPath);
        
        if(!dir.exists("cursors") && dir.exists("index.theme")) {
            availableIcons << currentPath;
        }
    }

//  We verify if there are themes in the home folder
    QDir userIconsDir(QDir::homePath()+"/.icons");
    QDirIterator it(userIconsDir.path(), QDir::NoDotAndDotDot|QDir::AllDirs);
    while(it.hasNext()) {
        QString currentPath = it.next();
        QDir dir(currentPath);

        if(!dir.exists("cursors") && dir.exists("index.theme")) {
            availableIcons << currentPath;
        }
    }

    return availableIcons;
}

////////////////////////////////////
// Methods responsible of file creation

bool AppearenceGTK::loadFileConfig()
{
    bool correct = false;
    foreach(AbstractAppearance* app, m_app) {
        bool c = app->loadSettings(app->defaultConfigFile());
        correct = correct || c;
    }
    kDebug() << "loading..." << correct;
    return correct;
}

bool AppearenceGTK::saveFileConfig()
{
    bool correct = true;
    foreach(AbstractAppearance* app, m_app) {
        bool c = app->saveSettings(app->defaultConfigFile());
        correct = correct && c;
    }
    kDebug() << "saving..." << correct;
    return correct;
}
