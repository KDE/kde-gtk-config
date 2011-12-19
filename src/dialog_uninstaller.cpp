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

#include "dialog_uninstaller.h"
DialogUninstaller::DialogUninstaller(QWidget* parent, AppearenceGTK *app)
    : QDialog(parent)
    , ui(new Ui::dialog_uninstaller)
    , appareance(app)
{
    ui->setupUi(this);
    
    refresthListsForUninstall();
    
    //EVENTOS
    connect(ui->but_uninstall_theme, SIGNAL(clicked()), this, SLOT(uninstallTheme()));
    connect(ui->but_uninstall_icon, SIGNAL(clicked()), this, SLOT(uninstallIcon()));

    threadEraseIcon = new ThreadErase;
    threadEraseTheme = new ThreadErase;

    connect(threadEraseIcon, SIGNAL(started()), this, SLOT(uninstallIcon()));
    connect(threadEraseTheme, SIGNAL(started()), this, SLOT(uninstallTheme()));

    connect(threadEraseIcon, SIGNAL(finished()), this, SLOT(threadUninstalledThemeIconFinished()));
    connect(threadEraseTheme, SIGNAL(finished()), this, SLOT(threadUninstalledThemeFinished()));
}

DialogUninstaller::~DialogUninstaller()
{
    delete ui;
    delete threadEraseIcon;
    delete threadEraseTheme;
}

void DialogUninstaller::refresthListsForUninstall()
{
    ui->lb_notice_uninstall_icon->clear();
    ui->lb_notice_uninstall_theme->clear();
    
    QStringList themes = appareance->getAvaliableThemesPaths();
    themes = themes.filter(QDir::homePath()); //we only one the locally installed themes
    
    //Just leave the theme name
    for(QStringList::iterator it=themes.begin(); it!=themes.end(); ++it)
        *it = QDir(*it).dirName();

    ui->cb_uninstall_theme->clear();
    ui->cb_uninstall_theme->addItems(themes);

    //now the same for icons
    QStringList icons = appareance->getAvaliableIconsPaths();
    icons = icons.filter(QDir::homePath());
    
    for(QStringList::iterator it=icons.begin(); it!=icons.end(); ++it)
        *it = QDir(*it).dirName();

    ui->cb_uninstall_icon->clear();
    ui->cb_uninstall_icon->addItems(icons);
}

void DialogUninstaller::uninstallTheme()
{
    if(ui->cb_uninstall_theme->currentIndex() < 0)
        return;

    QString tema = ui->cb_uninstall_theme->currentText();

    QStringList themes = appareance->getAvaliableThemesPaths();
    themes = themes.filter(QRegExp('/'+tema+'$'));

    Q_ASSERT(themes.size()==1);
    
    ui->cb_uninstall_theme->setEnabled(false);
    ui->but_uninstall_theme->setEnabled(false);

    ui->lb_notice_uninstall_theme->setText(i18n("Uninstalling GTK theme..."));

    threadEraseTheme->setThemeForErase(themes.first());
    threadEraseTheme->start();
}

void DialogUninstaller::uninstallIcon()
{
    if(ui->cb_uninstall_icon->currentIndex() < 0)
        return;

    QString icono = ui->cb_uninstall_icon->currentText();
    QStringList icons=appareance->getAvaliableIconsPaths();
    icons = icons.filter(QRegExp("/"+icono+"$"));

    Q_ASSERT(icons.size()==1);

    ui->cb_uninstall_icon->setEnabled(false);
    ui->but_uninstall_icon->setEnabled(false);

    ui->lb_notice_uninstall_icon->setText(i18n("Uninstalling icons..."));

    threadEraseIcon->setThemeForErase(icons.first());
    threadEraseIcon->start();
}

void DialogUninstaller::threadUninstalledThemeFinished()
{
    if(threadEraseTheme->isSuccess()) {
        ui->lb_notice_uninstall_theme->setText(i18n("GTK theme successfully uninstalled."));
        emit(themeUninstalled());
    } else {
        ui->lb_notice_uninstall_theme->setText(i18n("Could not uninstall the GTK theme."));
    }

    ui->cb_uninstall_theme->setEnabled(true);
    ui->but_uninstall_theme->setEnabled(true);

    refresthListsForUninstall();
}

void DialogUninstaller::threadUninstalledThemeIconFinished()
{
    if(threadEraseTheme->isSuccess()) {
        ui->lb_notice_uninstall_icon->setText(i18n("Icons successfully uninstalled."));
        emit(themeUninstalled());
    } else {
        ui->lb_notice_uninstall_icon->setText(i18n("Could not uninstall the icons theme."));
    }

    ui->cb_uninstall_icon->setEnabled(true);
    ui->but_uninstall_icon->setEnabled(true);

    refresthListsForUninstall();
}
