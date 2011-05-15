/*
    <GUI Installer for gtk's themes>
    Copyright (C) <2011>  <José Antonio Sánchez Reynaga>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef DIALOGINSTALLER_H
#define DIALOGINSTALLER_H
#include <QtCore>
#include <QtGui>
#include "installer.h"
#include "thread.h"
#include "ui_dialog_installer.h"
#include "klocale.h"
namespace Ui {
    class DialogInstaller;
}
/**
 * 
 * Interfaz gràfica para instalar temas gtk y paquete de iconos
 * 
 */
class DialogInstaller: public QDialog
{
    
    Q_OBJECT
public:
    DialogInstaller(QWidget *parent=0);
    ~DialogInstaller();
    
    
signals:
    /**
     * Se activa cuando ya se selecciono un tema
     */
    void selectedTheme();
        /**
     * Se activa cuando ya se selecciono un tema de iconos
     */
    void selectedIconTheme();
    
public slots:
    
    /**
      Abre un cuadro de dialogo, para seleccionar un fichero *.tar.gz de temas.
      La url lo guarda en la etiqueta del grupo correspondiente
      */
    void selectIconThemeFile();
    /**
      Abre un cuadro de dialogo, para seleccionar un fichero *.tar.gz de temas.
      La url lo guarda en la etiqueta del grupo correspondiente
      */
    void selectThemeFile();
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