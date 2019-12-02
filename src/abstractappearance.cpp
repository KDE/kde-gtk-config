/* KDE GTK Configuration Module
 * 
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

#include "abstractappearance.h"
#include <qregexp.h>
#include <QDir>
#include <QDebug>

static bool isTrue(const QString& value)
{
    return value == "1" || value == "true";
}

//SETTERS
void AbstractAppearance::setTheme(const QString& name) { m_settings["theme"] = name;}
void AbstractAppearance::setPrimaryButtonWarpsSlider(bool enabled) { m_settings["primary_button_warps_slider"] = enabled ? "1" : "0"; }

// GETTERS
QString AbstractAppearance::getTheme() const { return m_settings["theme"];}
QString AbstractAppearance::getThemeGtk3() const { return m_settings["themegtk3"]; }
bool AbstractAppearance::getPrimaryButtonWarpsSlider() const { return isTrue(m_settings["primary_button_warps_slider"]); }

QRegExp valueRx(" *([a-zA-Z\\-_]+) *= *\"?([^\"\\n]+)\"?", Qt::CaseSensitive, QRegExp::RegExp2);
QMap<QString,QString> AbstractAppearance::readSettingsTuples(QIODevice* device)
{
    QMap<QString, QString> ret;
    QTextStream flow(device);
    for(; !flow.atEnd() ;) {
        QString line = flow.readLine();
        int idxComment = line.indexOf('#');
        if(idxComment>=0)
            line = line.left(idxComment).simplified();

        if(valueRx.exactMatch(line))
            ret[valueRx.cap(1)] = valueRx.cap(2);
        else if(line.startsWith("include \"")) {
            QString filename = line.mid(9);
            filename.chop(1);
//             qDebug() << "including: " << filename;
            QFile f(filename);
            if(f.open(QFile::Text|QFile::ReadOnly)) {
                ret.unite(readSettingsTuples(&f));
            } else
                qWarning() << "couldn't include " << filename;
        }
//         else if(!line.isEmpty())
//             qWarning() << "misinterpreted line" << line;
    }
    return ret;
}

QStringList AbstractAppearance::installedThemesNames() const
{
    QStringList themes = installedThemes();
    QStringList ret;
    
    foreach(const QString& theme, themes)
        ret += QDir(theme).dirName();
    
    return ret;
}

bool AbstractAppearance::hasProperty(const QString& key) const
{
    return !m_settings.value(key).isEmpty();
}
