/*
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

#ifndef GTKCONFIGKCMODULE_H
#define GTKCONFIGKCMODULE_H

#include <kcmodule.h>
#include "ui_gui.h"
#include "appearencegtk.h"
#include "dialog_installer.h"
#include "dialog_uninstaller.h"
#include <knewstuff3/downloaddialog.h>

namespace Ui { class Modulo; }

class GTKConfigKCModule : public KCModule
{
  Q_OBJECT
public:
    explicit GTKConfigKCModule(QWidget* parent = 0 ,const QVariantList& args = QVariantList() );
    ~GTKConfigKCModule();
    
    void refreshThemesUi(bool useConfig=false);
    
public slots:
    void refreshLists();
    void makePreviewFont();
    void makePreviewIconTheme();
    
    ///it is called whenever something in the UI has changed
    void appChanged();
    
    void showThemeGHNS();
    void installThemeGTK3GHNS();
  
    virtual void save();
    virtual void defaults();
    
    void showDialogForInstall();
    void showDialogForUninstall();

signals:
    void selectedIconTheme();
    
private:
    Ui::GUI *ui;
    AppearenceGTK *appareance;
    
    DialogInstaller *installer;
    DialogUninstaller *uninstaller;
};

#endif // MODULO_H
