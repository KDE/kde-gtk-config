/* KDE GTK Configuration Module
 * 
 * Copyright 2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
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

#include "fontstest.h"
#include <fontshelpers.h>
#include <QFontDatabase>
#include <QStringList>
#include <qtest.h>

QTEST_MAIN( FontsTest )

void FontsTest::testBruteForce()
{
    QFontDatabase fdb;
    QStringList allfamilies = fdb.families();
    foreach(const QString& family, allfamilies) {
        QStringList styles = fdb.styles(family);
        foreach(const QString& style, styles) {
            QFont f(family);
            f.setStyleName(style);

            QString str = fontToString(f);
            QFont f2 = stringToFont(str);
            if(str != fontToString(f2)) {
                qDebug() << "faulty font:" << str << f << f2;
            }
            QCOMPARE(str, fontToString(f2));
            QCOMPARE(f, f2);
        }
    }
}
