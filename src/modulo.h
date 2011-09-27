/*
    <one line to give the library's name and an idea of what it does.>
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


#ifndef MODULO_H
#define MODULO_H

#include <kcmodule.h>
#include "ui_gui.h"
#include "aparienciagtk.h"
#include "dialog_installer.h"
#include "dialog_uninstaller.h"
#include <knewstuff3/downloaddialog.h>
namespace Ui {
    class Modulo;
}

class Modulo : public KCModule
{
  Q_OBJECT
public:
  /**
   * Constructor
   */
    explicit Modulo(QWidget* parent = 0
                    ,const QVariantList& args = QVariantList()
                   );
    
    /**
     * Destructor
     */
    ~Modulo();
    
public slots:
    void refreshLists();
    /**
     * Crear una previsualización de la fuente que se va a usar
     */
    void makePreviewFont();
    void makePreviewIconTheme();
    void appChanged(); //Metodo que se ejecuta cuando un cambio en la gui se ha registrado  
    
    void showThemeGHNS();
    void installThemeGTK3GHNS();
  
    
    virtual void save();
    virtual void defaults();
    
     /**
     * Muestra el dialogo para instalar un tema
     */
    void showDialogForInstall();
    /**
     * Muestra el dialogo para desinstalar un tema
     */
    void showDialogForUninstall();
   
  

signals:
    void selectedTheme();
    void selectedIconTheme();
    

    
private:
    Ui::GUI *ui;
    AparienciaGTK *appareance;
    
    DialogInstaller *installer;
    DialogUninstaller *uninstaller;
    
    
    

};

#endif // MODULO_H
