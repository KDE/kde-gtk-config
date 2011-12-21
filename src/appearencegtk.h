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

#ifndef APARIENCIAGTK_H
#define APARIENCIAGTK_H

#include <QtCore>
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
    void setIconFallBack(const QString&);
    void setFont(const QString&);
    
    //New features
    void setToolbarStyle(const QString&);
    void setShowIconsInMenus(bool show);
    void setShowIconsInButtons(bool show);

    QString getTheme() const; //Obtiene el nombre del tema actualmente utilizado
    QString getThemeGtk3() const;
   
    QString getIcon() const; //Obtiene el nombre del tema de iconos utilizado actualmente
    QString getFont() const; //Obtiene el tipo de letra que se esta utilizando
    QString getIconFallBack() const; // Obtiene el tema de iconos fallback
    QString getToolbarStyle() const; //Obtiene el estilo de la barra de herramientas
    bool getShowIconsInMenus() const;
    bool getShowIconsInButtons() const;
    
    static QStringList getAvaliableIconsPaths();//Obtiene las rutas absolutas de los temas de iconos disponibles en el sistema
    static QStringList getAvaliableIcons(); //Obtiene temas de icono disponibles en el sistema
    QStringList getAvaliableThemes() const;
    QStringList getAvaliableGtk3Themes();
    QStringList getAvaliableThemesPaths() const;

    bool loadFileConfig();
    bool saveFileConfig();
    bool saveGTK2Config(const QString& rootDir) const;
    bool saveGTK3Config(const QString& rootDir) const;
    
    QString gtkrcPath();
    QString gtk3settingsPath();
private:
    QString themesGtkrcFile(const QString& themeName) const;
    bool loadGTK2Config();
    bool loadGTK3Config();
    
    QMap <QString, QString> settings;
};

#endif // APARIENCIAGTK_H
