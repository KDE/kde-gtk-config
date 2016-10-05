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

#ifndef ICONTHEMESMODEL_H
#define ICONTHEMESMODEL_H

#include <QStandardItemModel>

class QDir;
class IconThemesModel : public QStandardItemModel
{
    public:
        enum ThemesRoles { PathRole=Qt::UserRole+1, InheritsRole, DirNameRole };
        
        explicit IconThemesModel(bool onlyHome=false, QObject* parent = 0);
        
        void reload();
        static QString findIconRecursivelyByName(const QString& name, const QDir& directory);
        
    private:
        static QString findFilesRecursively(const QStringList& wildcard, const QDir& directory);
        QList<QDir> installedThemesPaths();
        bool m_onlyHome;
};

#endif // ICONTHEMESMODEL_H
