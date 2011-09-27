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

#ifndef DIALOGUNINSTALLER_H
#define DIALOGUNINSTALLER_H
#include <QtCore>
#include <QtGui>
#include "installer.h"
#include "ui_dialog_uninstaller.h"
#include "thread.h"
#include "aparienciagtk.h"
#include "klocale.h"
namespace Ui{
    class DialogUninstaller;
}
/**
 * 
 * Interfaz gràfica para desinstalar temas gtk y paquete de iconos
 * 
 */
class DialogUninstaller: public QDialog
{
    
    Q_OBJECT
public:
    DialogUninstaller(QWidget *parent = 0, AparienciaGTK *app=0);
    ~DialogUninstaller();
    
signals:
    void themeUninstalled();
    
    
public slots:
       //METODOS PARA EL DESINSTALADOR
    void refresthListsForUninstall();
    void uninstallTheme();
    void uninstallIcon();
    void threadUninstalledThemeFinished();
    void threadUninstalledThemeIconFinished();
 
    
private:
    Ui::dialog_uninstaller *ui;
    AparienciaGTK *appareance;
     //PROPIEDADES PARA EL DESINSTALADOR
    ThreadErase *threadEraseIcon; //Hilo que desinstala un tema de iconos
    ThreadErase *threadEraseTheme; //Hilo que desinstala un tema gtk
};

#endif //dialog_installer.h 
