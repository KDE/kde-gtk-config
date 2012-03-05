/* KDE GTK Configuration Module
 * 
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

#include "abstractappearance.h"
#include <qregexp.h>
#include <QDir>

//SETTERS
void AbstractAppearance::setTheme(const QString& name) { m_settings["theme"] = name;}
void AbstractAppearance::setIcon(const QString& ic) { m_settings["icon"] = ic;}
void AbstractAppearance::setIconFallback(const QString& fall) { m_settings["icon_fallback"] = fall; }
void AbstractAppearance::setFont(const QString& fo) { m_settings["font"] = fo;}
void AbstractAppearance::setShowIconsInButtons(bool show) { m_settings["show_icons_buttons"] = show ? "1" : "0"; }
void AbstractAppearance::setShowIconsInMenus(bool show) { m_settings["show_icons_menus"] = show ? "1" : "0"; }
void AbstractAppearance::setToolbarStyle(const QString& toolbar_style) { m_settings["toolbar_style"] = toolbar_style; }

// GETTERS
QString AbstractAppearance::getTheme() const { return m_settings["theme"];}
QString AbstractAppearance::getIcon() const { return m_settings["icon"];}
QString AbstractAppearance::getIconFallback() const { return m_settings["icon_fallback"]; }
QString AbstractAppearance::getFont() const { return m_settings["font"]; }
QString AbstractAppearance::getThemeGtk3() const { return m_settings["themegtk3"]; }
QString AbstractAppearance::getToolbarStyle() const { return m_settings["toolbar_style"]; }
bool AbstractAppearance::getShowIconsInButtons() const { return m_settings["show_icons_buttons"]=="1"; }
bool AbstractAppearance::getShowIconsInMenus() const { return m_settings["show_icons_menus"]=="1"; }

QRegExp valueRx("([a-zA-Z\\-]+) *= *\"?([^\"\\n]+)\"?\\n", Qt::CaseSensitive, QRegExp::RegExp2);
QMap<QString,QString> AbstractAppearance::readSettingsTuples(const QString& allText)
{
    QMap<QString,QString> ret;
    for(int offset=0; offset>=0; offset=valueRx.indexIn(allText, offset+valueRx.cap(0).size())) {
        ret[valueRx.cap(1)] = valueRx.cap(2);
    }
    return ret;
}

QString AbstractAppearance::readMapDefaultValue(const QMap<QString,QString>& map, const QString& key, const QString& defaultValue)
{
    return map.contains(key) ? map[key] : defaultValue;
}

QStringList AbstractAppearance::installedThemesNames() const
{
    QStringList themes = installedThemes();
    QStringList ret;
    
    foreach(const QString& theme, themes)
        ret += QDir(theme).dirName();
    
    return ret;
}
