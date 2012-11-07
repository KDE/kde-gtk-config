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

#include "installer.h"
#include <QDir>
#include <QProcess>
#include <KTar>

bool Installer::installTheme(const QString &urlPackage)
{
    QString dest(QDir::homePath()+"/.themes");
    QDir::home().mkpath(dest);

    KTar package(urlPackage);
    if(!package.open(QIODevice::ReadOnly))
        return false;
    package.directory()->copyTo(dest);
    return true;
}

bool Installer::installIconTheme(const QString &urlPackage)
{
    QString dest(QDir::homePath()+"/.icons");
    QDir::home().mkpath(dest);

    KTar package(urlPackage);
    if(!package.open(QIODevice::ReadOnly))
        return false;
    package.directory()->copyTo(dest);
    return true;
}
