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
#include <QDebug>
#include <QSet>
#include <KIconTheme>
#include <QStandardPaths>

IconThemesModel::IconThemesModel(bool onlyHome, QObject* parent)
    : QStandardItemModel(parent)
    , m_onlyHome(onlyHome)
{
    setSortRole(Qt::DisplayRole);
    reload();
}

QList<QDir> IconThemesModel::installedThemesPaths()
{
    QList<QDir> availableIcons;

    QSet<QString> dirs;
    dirs += QDir::home().filePath(".icons");
    if(!m_onlyHome) {
        dirs += QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "icons", QStandardPaths::LocateDirectory).toSet();
    }

    foreach(const QString& dir, dirs) {
        QDir userIconsDir(dir);
        QDirIterator it(userIconsDir.path(), QDir::NoDotAndDotDot|QDir::AllDirs|QDir::NoSymLinks);
        while(it.hasNext()) {
            QString currentPath = it.next();
            QDir dir(currentPath);

            if(dir.exists() && dir.exists("index.theme")) {
                availableIcons << dir;
            }
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

QString IconThemesModel::findIconRecursivelyByName(const QString& name, const QDir& directory)
{
    return findFilesRecursively(QStringList() << (name+".png") << (name+".svg") << (name+".svgz"), directory);
}

QString IconThemesModel::findFilesRecursively(const QStringList& wildcard, const QDir& directory)
{
    Q_ASSERT(directory.isAbsolute());
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

void IconThemesModel::reload()
{
    clear();

    QSet<QString> done;
    QList<QDir> paths = installedThemesPaths();
    Q_FOREACH(const QDir& dir, paths) {
        KIconTheme theme(dir.dirName());
        if (!theme.isValid()) { //most likely a cursor theme
//             qWarning() << "invalid theme" << dir.dirName();
            continue;
        }

        if (done.contains(dir.dirName()))
            continue;

        done << dir.dirName();

        QStandardItem* item = new QStandardItem(dir.dirName());
        item->setData(dir.path(), PathRole);
        item->setData(dir.dirName(), DirNameRole);

        item->setText(theme.name());
        item->setToolTip(theme.description());
        item->setData(theme.inherits(), IconThemesModel::InheritsRole);
        QString iconName = theme.example();
        if (iconName.isEmpty())
            iconName = QStringLiteral("folder");

        QString path = theme.iconPathByName(iconName, 16, KIconLoader::MatchBest);
        item->setIcon(QIcon(path));

        appendRow(item);
    }
    sort(0);
}
