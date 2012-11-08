/*
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

#ifndef DIALOGINSTALLER_H
#define DIALOGINSTALLER_H
#include <KDialog>

class ThreadAnalisysThemeIcon;
class ThreadAnalisysTheme;
class Thread;
namespace Ui {
    class dialog_installer;
}

/**
 * GUI to install icons and gtk themes
 */
class DialogInstaller: public KDialog
{
Q_OBJECT
public:
    DialogInstaller(QWidget *parent=0);
    ~DialogInstaller();
    
signals:
    /**
     * Whenever a theme is installed, this signal is emitted
     */
    void themeInstalled();
    
private slots:
    /**
      Instala el tema de icono a traves del objeto Installer
        Para ello lanza un hilo para que no se bloqué la GUI
      */
    void installThemeIcon();
    /**
      Instala el tema de icono a traves del objeto Installer
      Para ello lanza un hilo para que no se bloqué la GUI
      */
    void installTheme();

    void enableGUIThemeInstaller();
    void disableGUIThemeInstaller();
    void refreshGUITheme();
    void themeAnalisys();
    
    ///executed after analyzing theme
    void checkThemeAnalisys();

    void enableGUIThemeIconInstaller();
    void disableGUIThemeIconInstaller();
    void refreshGUIIconTheme();
    void themeIconAnalisys();
    void checkThemeIconAnalisys();

    
private:
    Ui::dialog_installer *ui;
    
    //PROPIEDADES PARA EL AREA DE INSTALADOR
    Thread *threadForTheme; //Hilo que instala el tema gtk
    Thread *threadForIcon; //Hilo que instala el tema de iconos
    ThreadAnalisysTheme *threadAnalisysTheme; //hilo para analizar los temas a instalar
    ThreadAnalisysThemeIcon *threadAnalisysThemeIcon; //hilo para analizar los temas a instalar
    
};

#endif //dialog_installer.h