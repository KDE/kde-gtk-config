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

#include <QDir>
#include <QDebug>

#include <KLocalizedString>

#include "dialog_uninstaller.h"
#include "abstractappearance.h"
#include "ui_dialog_uninstaller.h"
#include "thread.h"
#include "appearencegtk.h"

DialogUninstaller::DialogUninstaller(QWidget *parent, AppearenceGTK *app)
    : QDialog(parent)
    , ui(new Ui::dialog_uninstaller)
    , appareance(app)
{
    ui->setupUi(this);
    
    refreshListsForUninstall();
    
    connect(ui->but_uninstall_theme, &QAbstractButton::clicked, this, &DialogUninstaller::uninstallTheme);
}

DialogUninstaller::~DialogUninstaller()
{
    delete ui;
}

void DialogUninstaller::refreshListsForUninstall()
{
    ui->lb_notice_uninstall_theme->clear();
    
    QStringList themes = appareance->gtk2Appearance()->installedThemes();
    themes = themes.filter(QDir::homePath()); // We only one the locally installed themes
    
    // Just leave the theme name
    for(QStringList::iterator it=themes.begin(); it!=themes.end(); ++it)
        *it = QDir(*it).dirName();

    ui->cb_uninstall_theme->clear();
    ui->cb_uninstall_theme->addItems(themes);
}

void DialogUninstaller::uninstallTheme()
{
    if(ui->cb_uninstall_theme->currentIndex() < 0)
        return;

    QString tema = ui->cb_uninstall_theme->currentText();

    QStringList themes = appareance->gtk2Appearance()->installedThemes();
    themes = themes.filter(QRegExp('/' + tema + '$'));

    Q_ASSERT(themes.size() == 1);
    
    ui->cb_uninstall_theme->setEnabled(false);
    ui->but_uninstall_theme->setEnabled(false);

    ui->lb_notice_uninstall_theme->setText(i18n("Uninstalling GTK theme..."));

    ThreadErase* threadEraseTheme = new ThreadErase;
    threadEraseTheme->setThemeForErase(themes.first());
    connect(threadEraseTheme, &KJob::finished, this, &DialogUninstaller::threadUninstalledThemeFinished);
    threadEraseTheme->start();
}

void DialogUninstaller::threadUninstalledThemeFinished(KJob* job)
{
    if (job->error()==0) {
        ui->lb_notice_uninstall_theme->setText(i18n("GTK theme successfully uninstalled."));
        emit(themeUninstalled());
    } else {
        ui->lb_notice_uninstall_theme->setText(i18n("Could not uninstall the GTK theme."));
    }

    ui->cb_uninstall_theme->setEnabled(true);
    ui->but_uninstall_theme->setEnabled(true);

    refreshListsForUninstall();
}
