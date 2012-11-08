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

#include "thread.h"
#include <kdebug.h>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QDirIterator>
#include <kio/deletejob.h>
#include <kio/netaccess.h>
#include <KMimeType>
#include <KTar>

bool fileIsTar(const QString& path)
{
    QFileInfo file(path);
    if(file.isDir() || !file.exists())
        return false;
    
    KMimeType::Ptr type = KMimeType::findByPath(path);
    return type && (type->is("application/x-tar") || type->is("application/x-bzip-compressed-tar") || type->is("application/x-compressed-tar"));
}

Thread::Thread(const QString& accion)
    : action(accion)
    , success(false)
{}

void Thread::run()
{
    success = false;
    if(urlPackage.isEmpty()) {
        kDebug() << "*** ERROR: There's nothing to do";
    } else if(action == "icon") {
        kDebug() << "Installing icons theme";
        success = Installer::installIconTheme(urlPackage);
    } else if(action == "theme") {
        kDebug() << "Installing GTK theme";
        success = Installer::installTheme(urlPackage);
    }
}

void Thread::setUrlPackage(const QString& package)
{
    urlPackage = package;
}

bool Thread::isSuccess() const
{
    return success;
}

void ThreadAnalisysTheme::run()
{
    success = false;
    kDebug()<< "File to install" << packageTheme;

    KTar package(packageTheme);
    if(!package.open(QIODevice::ReadOnly)) {
        kDebug() << "ERROR extracting the package theme" << packageTheme;
        return;
    }
    kDebug() << "** EXTRACTING ICONS TO A TEMPORAL FOLDER";
    //We proceed unpacking the package in a temporal directory
    QDir temporal(QDir::tempPath()+"/CGC/theme");

    //Make sure it's already created
    temporal.mkpath(temporal.path());
    
    package.directory()->copyTo(temporal.path());
    
    // Package extracted in the temp dir. Now we want to know the name
    QString folder=temporal.entryList(QDir::AllDirs|QDir::NoDotAndDotDot).first();

    kDebug() << "FOUND THEME FOLDER = " << folder;
    kDebug() << "\n******* THEME " << temporal.path()+'/'+folder;

    //We know the path of the folder to analyze
    QDirIterator it(temporal.path()+'/'+folder);

    while(it.hasNext()) {

        QString file = it.next();
        kDebug() << "trying file" << file;

        if(it.fileName()=="gtkrc") {
            kDebug() << "FILE : " << file;
            success = true;
            break;
        }
    }

    kDebug() << "Deleting temps. Successful:" << success;
    if(KIO::NetAccess::synchronousRun(KIO::del(KUrl::fromLocalFile(temporal.path()), KIO::HideProgressInfo), 0))
        kDebug() << "There was not cleanning";
    else
        kDebug() << "Cleanning complete" << temporal.path();
}

void ThreadAnalisysTheme::setPackageTheme(const QString& theme)
{
    packageTheme = theme;
}

bool ThreadAnalisysTheme::isSuccess() const
{
    return success;
}

void ThreadAnalisysThemeIcon::run()
{
    success = false;
    kDebug()<< "*************** GTK THEME INSTALLATION";
    kDebug()<< "File to install" << packageTheme;

    KTar package(packageTheme);
    if(!package.open(QIODevice::ReadOnly)) {
        kDebug() << "ERROR extracting the package theme" << packageTheme;
        return;
    }
    kDebug() << "** EXTRACTING ICONS TO A TEMPORAL FOLDER";
    QDir temporal(QDir::tempPath()+"/CGC/icon");
    temporal.mkpath(temporal.path());

    package.directory()->copyTo(temporal.path());

    //archive extracted in the temp directory
    QString folder= temporal.entryList(QDir::AllDirs|QDir::NoDotAndDotDot).first();
    kDebug() << "FOUND THEME FOLDER = " << folder;
    kDebug() << "\n******* THEME " << temporal.path()+'/'+folder;

    QDirIterator it(temporal.path()+'/'+folder);

    while(it.hasNext()) {

        QString file = it.next();

        if(it.fileName()=="index.theme") {
            //archivo index.theme
            kDebug() << "FILE : " << file;
            success = true;
            break;
        }

    }

    kDebug() << "Deleting temps. Successful:" << success;
    if(KIO::NetAccess::synchronousRun(KIO::del(KUrl::fromLocalFile(temporal.path()), KIO::HideProgressInfo), 0)) {
        kDebug() << "There was not cleanning";
    } else
        kDebug() << "Cleanning complete." << temporal.path();
}

void ThreadAnalisysThemeIcon::setPackageTheme(const QString& theme)
{
    packageTheme = theme;
}

bool ThreadAnalisysThemeIcon::isSuccess()
{
    return success;
}

bool ThreadErase::isSuccess()
{
    return success;
}

void ThreadErase::setThemeForErase(const QString& theme)
{
    Q_ASSERT(!theme.isEmpty());
    themeForErase = theme;
}

void ThreadErase::run()
{
    QThread::sleep(3);

    success = KIO::NetAccess::synchronousRun(KIO::del(KUrl::fromLocalFile(themeForErase), KIO::HideProgressInfo), 0);
}
