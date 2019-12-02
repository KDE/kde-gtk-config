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


#ifndef GTKCONFIGKCMODULE_H
#define GTKCONFIGKCMODULE_H

#include <kcmodule.h>
#include "appearencegtk.h"
#include "dialog_installer.h"
#include "dialog_uninstaller.h"
#include <kns3/downloaddialog.h>

class KProcess;
namespace Ui { class Modulo; class GUI; }

class GTKConfigKCModule : public KCModule
{
  Q_OBJECT
public:
    explicit GTKConfigKCModule(QWidget* parent = 0 ,const QVariantList& args = QVariantList() );
    ~GTKConfigKCModule();
    
    void refreshThemesUi(bool useConfig=false);
    
    void save() override;
    void defaults() override;
    void load() override;
    
public slots:
    void refreshLists();
    
    ///it is called whenever something in the UI has changed
    void appChanged();
    void savePreviewConfig();
    
    void showThemeGHNS();
    void installThemeGTK3GHNS();
    
    void showDialogForInstall();
    void showDialogForUninstall();
    
    void runGtk2IfNecessary(bool);
    void runGtk3IfNecessary(bool);
    void untogglePreview();
    
private:
    void syncUI();
    bool m_loading = false;
    
    Ui::GUI *ui;
    AppearenceGTK *appareance;
    
    DialogInstaller *installer;
    DialogUninstaller *uninstaller;
    KProcess* m_p2;
    KProcess* m_p3;
    QString m_tempGtk2Preview;
    QString m_tempGtk3Preview;
    bool m_saveEnabled;
};

#endif // MODULO_H
