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

#include "dialog_installer.h"
#include "ui_dialog_installer.h"
#include <KMessageBox>
#include "installer.h"
#include "thread.h"
#include <QFile>
#include <QMimeDatabase>
#include <klocalizedstring.h>

static bool fileIsTar(const QString& path)
{
    QFileInfo file(path);
    if(file.isDir() || !file.exists())
        return false;
    
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForUrl(QUrl::fromLocalFile(path));
    return type.isValid() && (type.inherits("application/x-tar") || type.inherits("application/x-bzip-compressed-tar") || type.inherits("application/x-compressed-tar"));
}

DialogInstaller::DialogInstaller(QWidget *parent)
    : QDialog(parent), ui(new Ui::dialog_installer)
{
    ui->setupUi(this);
    
    //TODO: make sure it's a good idea to have the threads always instanciated
    threadForTheme = new Thread("theme");
    threadForIcon = new Thread("icon");
    threadAnalisysTheme = new ThreadAnalisysTheme;
    threadAnalisysThemeIcon = new ThreadAnalisysThemeIcon;
    
    //installation ui
    connect(ui->theme_file, &KUrlRequester::textChanged, this, &DialogInstaller::themeAnalisys);
    connect(ui->icon_file, &KUrlRequester::textChanged, this, &DialogInstaller::themeIconAnalisys);

    connect(ui->but_icon_install, SIGNAL(clicked()), this, SLOT(installThemeIcon()));
    connect(ui->but_theme_install, SIGNAL(clicked()), this, SLOT(installTheme()));

    connect(threadAnalisysTheme, &KJob::finished, this, &DialogInstaller::checkThemeAnalisys);
    connect(threadAnalisysThemeIcon, &KJob::finished, this, &DialogInstaller::checkThemeIconAnalisys);

    connect(threadForTheme, &Thread::started, this, &DialogInstaller::disableGUIThemeInstaller);
    connect(threadForIcon, &Thread::started, this, &DialogInstaller::disableGUIThemeIconInstaller);
    connect(threadForTheme, &KJob::finished, this, &DialogInstaller::enableGUIThemeInstaller);
    connect(threadForIcon, &KJob::finished, this, &DialogInstaller::enableGUIThemeIconInstaller);

    //ui refresh
    connect(threadForTheme, &KJob::finished, this, &DialogInstaller::refreshGUITheme);
    connect(threadForIcon, &KJob::finished, this, &DialogInstaller::refreshGUIIconTheme);

}

DialogInstaller::~DialogInstaller()
{
    delete ui;
    delete threadAnalisysTheme;
    delete threadAnalisysThemeIcon;
    delete threadForIcon;
    delete threadForTheme;
}

void DialogInstaller::installTheme()
{
    QString file = ui->theme_file->text();

//     qDebug()<< "File to install" << file;
    if(!fileIsTar(file)) {
        KMessageBox::error(this, i18n("Could not install the %1 theme.", file), i18n("Cannot install theme"));
        return;
    }

    threadForTheme->setUrlPackage(file);
    threadForTheme->start();
}

void DialogInstaller::installThemeIcon()
{
    QString file = ui->icon_file->text();

//     qDebug()<< "File to install" << file;
    if(!fileIsTar(file)) {
        KMessageBox::error(this, i18n("Could not install the %1 theme.", file), i18n("Cannot install theme"));
        return;
    }

    threadForIcon->setUrlPackage(file);
    threadForIcon->start();
}

void DialogInstaller::themeAnalisys()
{
    ui->lb_theme_notice->setText(i18n("Parsing theme..."));

    ui->theme_file->setEnabled(false);
    ui->but_icon_install->setEnabled(false);

    QString archivo = ui->theme_file->text();
    threadAnalisysTheme->setPackageTheme(archivo);
    threadAnalisysTheme->start();
}

void DialogInstaller::checkThemeAnalisys()
{
    ui->theme_file->setEnabled(true);

    if(threadAnalisysTheme->isSuccess()) {
        ui->lb_theme_notice->setText(i18n("This GTK theme can be installed"));
        ui->but_theme_install->setEnabled(true);
    } else {
        ui->lb_theme_notice->setText(i18n("This GTK theme cannot be installed"));
        ui->but_theme_install->setEnabled(false);
    }

}

void DialogInstaller::themeIconAnalisys()
{
    ui->lb_icon_notice->setText(i18n("Parsing theme..."));

    ui->icon_file->setEnabled(false);
    ui->but_icon_install->setEnabled(false);

    QString archivo = ui->icon_file->text();
    threadAnalisysThemeIcon->setPackageTheme(archivo);
    threadAnalisysThemeIcon->start();
}

void DialogInstaller::checkThemeIconAnalisys()
{
    ui->icon_file->setEnabled(true);

    if(threadAnalisysThemeIcon->isSuccess()) {
        ui->lb_icon_notice->setText(i18n("This icons theme can be installed"));
        ui->but_icon_install->setEnabled(true);
    } else {
        ui->lb_icon_notice->setText(i18n("This icons theme cannot be installed"));
        ui->but_icon_install->setEnabled(false);
    }
}

void DialogInstaller::enableGUIThemeInstaller()
{
    ui->but_theme_install->setEnabled(true);
    ui->theme_file->setEnabled(true);
}

void DialogInstaller::enableGUIThemeIconInstaller()
{
    ui->but_icon_install->setEnabled(true);
    ui->icon_file->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Close)->setEnabled(true);
}

void DialogInstaller::disableGUIThemeInstaller()
{
    ui->lb_theme_notice->setText(i18n("Installing GTK theme..."));
    ui->but_theme_install->setEnabled(false);
    ui->theme_file->setEnabled(false);
}
void DialogInstaller::disableGUIThemeIconInstaller()
{
    ui->lb_icon_notice->setText(i18n("Installing icons..."));
    ui->but_icon_install->setEnabled(false);
    ui->icon_file->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Close)->setEnabled(false);
}

void DialogInstaller::refreshGUIIconTheme()
{
    if(threadForIcon->isSuccess()) {
        ui->lb_icon_notice->setText(i18n("Icons Successfully Installed"));
        ui->icon_file->clear();
        emit themeInstalled();
    } else {
        ui->lb_icon_notice->setText(i18n("The icons theme cannot be installed"));
        ui->icon_file->clear();
    }
}

// Esto se ejecuta cuando un tema es tratado a instalar
void DialogInstaller::refreshGUITheme()
{
    if(threadForTheme->isSuccess()) {
        ui->lb_theme_notice->setText(i18n("GTK Theme Successfully Installed"));
        emit themeInstalled();
    } else {
        ui->lb_theme_notice->setText(i18n("The GTK theme cannot be installed"));
    }
    ui->theme_file->clear();
}
