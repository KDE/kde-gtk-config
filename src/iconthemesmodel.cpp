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

#include "iconthemesmodel.h"
#include <QDir>
#include <QDirIterator>
#include <KDebug>
#include <KIconTheme>

IconThemesModel::IconThemesModel(bool onlyHome, QObject* parent)
    : QStandardItemModel(parent)
    , m_onlyHome(onlyHome)
{
    reload();
}

QList<QDir> IconThemesModel::installedThemesPaths()
{
    QList<QDir> availableIcons;

    //List existing folders in the root themes directory
    //TODO: Use KStandardDirs
    if(!m_onlyHome) {
        QDir root("/usr/share/icons");
        QDirIterator iter(root.path(), QDir::AllDirs|QDir::NoDotAndDotDot|QDir::NoSymLinks);
        while(iter.hasNext()) {
            QString currentPath = iter.next();
            QDir dir(currentPath);
            
            if(!dir.exists("cursors") && dir.exists("index.theme")) {
                availableIcons << dir;
            }
        }
    }

//  We verify if there are themes in the home folder
    QDir userIconsDir(QDir::home().filePath("/.icons"));
    QDirIterator it(userIconsDir.path(), QDir::NoDotAndDotDot|QDir::AllDirs);
    while(it.hasNext()) {
        QString currentPath = it.next();
        QDir dir(currentPath);

        if(!dir.exists("cursors") && dir.exists("index.theme")) {
            availableIcons << dir;
        }
    }

    return availableIcons;
}

bool greatSizeIs48(const QString& a, const QString& b)
{
    bool a48=a.contains("48"), b48=b.contains("48");
    if((a48 && b48) || (!a48 && !b48))
        return a<b;
    else
        return a48;
}

QString IconThemesModel::findFilesRecursively(const QStringList& wildcard, const QDir& directory)
{
    QFileInfoList entries = directory.entryInfoList(wildcard, QDir::Files);
    foreach(const QFileInfo& f, entries) {
        return f.absoluteFilePath();
    }
    
    QStringList subdirs = directory.entryList(QDir::AllDirs|QDir::NoDotAndDotDot);
    qSort(subdirs.begin(), subdirs.end(), greatSizeIs48);
    foreach(const QString& subdir, subdirs) {
        QString ret = findFilesRecursively(wildcard, QDir(directory.filePath(subdir)));
        if(!ret.isEmpty())
            return ret;
    }
    
    return QString();
}

void fillItem(const QDir& dir, QStandardItem* item)
{
    KIconTheme theme(dir.dirName());
    
    item->setText(theme.name());
    item->setToolTip(theme.description());
    item->setData(theme.inherits(), IconThemesModel::InheritsRole);
    QString iconName = theme.example();
    
    if(!iconName.isEmpty()) {
        QString path = IconThemesModel::findFilesRecursively(QStringList(iconName+".*"), dir.path());
        item->setIcon(QIcon(path));
    }
    
    if(item->icon().isNull()) {
        QString path = IconThemesModel::findFilesRecursively(QStringList("*.png") << "*.svg" << "*.svgz", dir.path());
        item->setIcon(QIcon(path));        
    }
}

void IconThemesModel::reload()
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
