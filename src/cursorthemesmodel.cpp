/* KDE GTK Configuration Module
 * 
 * Copyright 2016 Andrey Bondrov <andrey.bondrov@rosalab.ru>
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

#include "cursorthemesmodel.h"
#include <QDir>
#include <QDirIterator>
#include <QSet>
#include <KIconTheme>
#include <QStandardPaths>

#include <X11/Xlib.h>
#include <X11/Xcursor/Xcursor.h>

// Check for older version
#if !defined(XCURSOR_LIB_MAJOR) && defined(XCURSOR_MAJOR)
#  define XCURSOR_LIB_MAJOR XCURSOR_MAJOR
#  define XCURSOR_LIB_MINOR XCURSOR_MINOR
#endif

CursorThemesModel::CursorThemesModel(QObject* parent)
    : IconThemesModel(parent)
{
    reload();
}

QList<QDir> CursorThemesModel::installedThemesPaths()
{
    QList<QDir> availableIcons;
    QStringList dirs;

#if XCURSOR_LIB_MAJOR == 1 && XCURSOR_LIB_MINOR < 1
    // These are the default paths Xcursor will scan for cursor themes
    QString path("~/.icons:/usr/share/icons:/usr/share/pixmaps:/usr/X11R6/lib/X11/icons");

    // If XCURSOR_PATH is set, use that instead of the default path
    char *xcursorPath = std::getenv("XCURSOR_PATH");
    if (xcursorPath)
        path = xcursorPath;
#else
    // Get the search path from Xcursor
    QString path = XcursorLibraryPath();
#endif

    // Separate the paths
    dirs = path.split(':', QString::SkipEmptyParts);

    // Remove duplicates
    QMutableStringListIterator i(dirs);
    while (i.hasNext())
    {
        const QString path = i.next();
        QMutableStringListIterator j(i);
        while (j.hasNext())
            if (j.next() == path)
                j.remove();
    }

    // Expand all occurrences of ~/ to the home dir
    dirs.replaceInStrings(QRegExp(QStringLiteral("^~\\/")), QDir::home().path() + '/');
 
    foreach(const QString& dir, dirs) {
        QDir userIconsDir(dir);
        QDirIterator it(userIconsDir.path(), QDir::NoDotAndDotDot|QDir::AllDirs|QDir::NoSymLinks);
        while(it.hasNext()) {
            QString currentPath = it.next();
            QDir dir(currentPath);

            if(dir.exists() && dir.exists("cursors"))
                availableIcons << dir;
        }
    }
    return availableIcons;
}

void CursorThemesModel::fillItem(const QDir& dir, QStandardItem* item)
{
    KIconTheme theme(dir.dirName());
    if(!theme.name().isEmpty())
        item->setText(theme.name());
    else
        item->setText(dir.dirName());
    item->setToolTip(theme.description());
    item->setData(theme.inherits(), CursorThemesModel::InheritsRole);
}



void CursorThemesModel::reload()
{
    clear();
    
    QList<QDir> paths = installedThemesPaths();
    Q_FOREACH(const QDir& dir, paths) {
        QStandardItem* themeit = new QStandardItem(dir.dirName());
        themeit->setData(dir.path(), PathRole);
        themeit->setData(dir.dirName(), DirNameRole);
        fillItem(dir, themeit);
        appendRow(themeit);
    }
}
