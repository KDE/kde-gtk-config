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

#ifndef DIALOGUNINSTALLER_H
#define DIALOGUNINSTALLER_H
#include <QtCore>
#include <QtGui>
#include "installer.h"
#include "ui_dialog_uninstaller.h"
#include "thread.h"
#include "appearencegtk.h"
#include "klocale.h"
namespace Ui{
    class DialogUninstaller;
}

/**
 * GUI to uninstall GTK themes and icon themes
 */
class DialogUninstaller: public QDialog
{
    Q_OBJECT
public:
    DialogUninstaller(QWidget *parent = 0, AppearenceGTK *app=0);
    ~DialogUninstaller();
    
signals:
    void themeUninstalled();
    
public slots:
    void refresthListsForUninstall();
    void uninstallTheme();
    void uninstallIcon();
    void threadUninstalledThemeFinished();
    void threadUninstalledThemeIconFinished();
 
private:
    Ui::dialog_uninstaller *ui;
    AppearenceGTK *appareance;
    
    ///Thread that uninstalls icons
    ThreadErase *threadEraseIcon;
    
    ///Thread that uninstalls themes
    ThreadErase *threadEraseTheme;
};

#endif //dialog_installer.h 
