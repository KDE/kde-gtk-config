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

#ifndef ABSTACTAPPEARANCE_H
#define ABSTACTAPPEARANCE_H

#include <QStringList>
#include <QMap>

class AbstractAppearance
{
    public:
        virtual ~AbstractAppearance() {}
        virtual QString defaultConfigFile() const = 0;
        virtual bool loadSettings(const QString& path) = 0;
        virtual bool saveSettings(const QString& path) = 0;
        
        /** @returns the installed themes' paths*/
        virtual QStringList installedThemes() const = 0;
        
        void setTheme(const QString& name);
        void setIcon(const QString& name);
        void setIconFallback(const QString& name);
        void setFont(const QString& font);
        void setShowIconsInMenus(bool show);
        void setShowIconsInButtons(bool show);
        void setToolbarStyle(const QString& style);
        
        QString getTheme() const;
        QString getIcon() const;
        QString getIconFallback() const;
        QString getFont() const;
        QString getThemeGtk3() const;
        QString getToolbarStyle() const;
        bool getShowIconsInButtons() const;
        bool getShowIconsInMenus() const;
        
        QStringList installedThemesNames() const;
        bool hasProperty(const QString& key);
        
        static QMap<QString,QString> readSettingsTuples(QIODevice* device);
        
    protected:
        QMap<QString, QString> m_settings;
};

#endif
