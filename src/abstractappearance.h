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
#include <QIODevice>

class AbstractAppearance
{
    public:
        virtual ~AbstractAppearance() {}
        virtual bool loadSettings() = 0;
        virtual bool saveSettings() const = 0;
        virtual bool loadSettings(const QString& path) = 0;
        virtual bool saveSettings(const QString& path) const = 0;

        /** @returns the installed themes' paths*/
        virtual QStringList installedThemes() const = 0;
        
        void setTheme(const QString& name);
        void setPrimaryButtonWarpsSlider(bool enabled);
        
        QString getTheme() const;
        QString getThemeGtk3() const;
        bool getPrimaryButtonWarpsSlider() const;
        
        QStringList installedThemesNames() const;
        bool hasProperty(const QString& key) const;
        
        static QMap<QString,QString> readSettingsTuples(QIODevice* device);
        
    protected:
        QMap<QString, QString> m_settings;
};

#endif
