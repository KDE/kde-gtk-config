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

#include "dialog_uninstaller.h"
#include "abstractappearance.h"
#include "iconthemesmodel.h"
#include "ui_dialog_uninstaller.h"
#include "thread.h"
#include "appearencegtk.h"
#include <QDir>
#include <QDebug>
#include <KLocalizedString>

DialogUninstaller::DialogUninstaller(QWidget* parent, AppearenceGTK *app)
    : QDialog(parent)
    , ui(new Ui::dialog_uninstaller)
    , appareance(app)
{
    ui->setupUi(this);
    
    refreshListsForUninstall();
    
    connect(ui->but_uninstall_theme, SIGNAL(clicked()), this, SLOT(uninstallTheme()));
    connect(ui->but_uninstall_icon, SIGNAL(clicked()), this, SLOT(uninstallIcon()));
}

DialogUninstaller::~DialogUninstaller()
{
    delete ui;
}

void DialogUninstaller::refreshListsForUninstall()
{
    ui->lb_notice_uninstall_icon->clear();
    ui->lb_notice_uninstall_theme->clear();
    
    QStringList themes = appareance->gtk2Appearance()->installedThemes();
    themes = themes.filter(QDir::homePath()); //we only one the locally installed themes
    
    //Just leave the theme name
    for(QStringList::iterator it=themes.begin(); it!=themes.end(); ++it)
        *it = QDir(*it).dirName();

    ui->cb_uninstall_theme->clear();
    ui->cb_uninstall_theme->addItems(themes);

    ui->cb_uninstall_icon->setModel(new IconThemesModel(true));
}

void DialogUninstaller::uninstallTheme()
{
    if(ui->cb_uninstall_theme->currentIndex() < 0)
        return;

    QString tema = ui->cb_uninstall_theme->currentText();

    QStringList themes = appareance->gtk2Appearance()->installedThemes();
    themes = themes.filter(QRegExp('/'+tema+'$'));

    Q_ASSERT(themes.size()==1);
    
    ui->cb_uninstall_theme->setEnabled(false);
    ui->but_uninstall_theme->setEnabled(false);

    ui->lb_notice_uninstall_theme->setText(i18n("Uninstalling GTK theme..."));

    ThreadErase* threadEraseTheme = new ThreadErase;
    threadEraseTheme->setThemeForErase(themes.first());
    connect(threadEraseTheme, SIGNAL(finished(KJob*)), this, SLOT(threadUninstalledThemeFinished(KJob*)));
    threadEraseTheme->start();
}

void DialogUninstaller::uninstallIcon()
{
    int themeIndex = ui->cb_uninstall_icon->currentIndex();
    if(themeIndex<0)
        return;
    
    QAbstractItemModel* model = ui->cb_uninstall_icon->model();
    QString theme = model->data(model->index(themeIndex, 0), IconThemesModel::PathRole).toString();

    ui->cb_uninstall_icon->setEnabled(false);
    ui->but_uninstall_icon->setEnabled(false);

    ui->lb_notice_uninstall_icon->setText(i18n("Uninstalling icons..."));

    ThreadErase* threadEraseIcon = new ThreadErase;
    threadEraseIcon->setThemeForErase(theme);
    connect(threadEraseIcon, SIGNAL(finished(KJob*)), this, SLOT(threadUninstalledThemeIconFinished(KJob*)));
    threadEraseIcon->start();
}

void DialogUninstaller::threadUninstalledThemeFinished(KJob* job)
{
    if(job->error()==0) {
        ui->lb_notice_uninstall_theme->setText(i18n("GTK theme successfully uninstalled."));
        emit(themeUninstalled());
    } else {
        ui->lb_notice_uninstall_theme->setText(i18n("Could not uninstall the GTK theme."));
    }

    ui->cb_uninstall_theme->setEnabled(true);
    ui->but_uninstall_theme->setEnabled(true);

    refreshListsForUninstall();
}

void DialogUninstaller::threadUninstalledThemeIconFinished(KJob* job)
{
    if(job->error()==0) {
        ui->lb_notice_uninstall_icon->setText(i18n("Icons successfully uninstalled."));
        emit(themeUninstalled());
    } else {
        ui->lb_notice_uninstall_icon->setText(i18n("Could not uninstall the icons theme."));
    }

    ui->cb_uninstall_icon->setEnabled(true);
    ui->but_uninstall_icon->setEnabled(true);

    refreshListsForUninstall();
}
