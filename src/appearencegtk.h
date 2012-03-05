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

#ifndef APARIENCIAGTK_H
#define APARIENCIAGTK_H

#include <QString>
#include <QMap>
#include <qvector.h>

class AbstractAppearance;
/**
 * This class is responsible of administrating the GTK themes. It loads the 
 * configurations from the .gtkrc-2.0 file.
 */
class AppearenceGTK
{
public:
    AppearenceGTK();

    void setTheme(const QString&);
    void setThemeGtk3(const QString &theme);
    void setIcon(const QString&);
    void setIconFallback(const QString&);
    void setFont(const QString&);
    
    void setToolbarStyle(const QString&);
    void setShowIconsInMenus(const bool& show);
    void setShowIconsInButtons(const bool& show);

    QString getTheme() const;
    QString getThemeGtk3() const;
   
    QString getIcon() const;
    QString getFont() const;
    QString getIconFallback() const;
    QString getToolbarStyle() const;
    bool getShowIconsInMenus() const;
    bool getShowIconsInButtons() const;
    
    bool loadFileConfig();
    bool saveFileConfig();
    
    AbstractAppearance* gtk2Appearance() const { return m_app.first(); }
    AbstractAppearance* gtk3Appearance() const { return m_app.last(); }
private:
    QVector<AbstractAppearance*> m_app;
};

#endif // APARIENCIAGTK_H
