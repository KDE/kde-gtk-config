/*
 * Copyright 2020 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <QFile>
#include <QStandardPaths>
#include <QSvgRenderer>

#include "auroraedecorationpainter.h"

const QString AuroraeDecorationPainter::s_auroraeThemesPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/aurorae/themes/");

AuroraeDecorationPainter::AuroraeDecorationPainter(const QString& themeName)
    : DecorationPainter()
    , m_themeName(themeName)
    , m_themePath(s_auroraeThemesPath + themeName + '/')
{
}

void AuroraeDecorationPainter::paintButton(QPainter &painter, const QString &buttonType, const QString &buttonState) const
{
    const QString buttonFileName = buttonTypeToFileName(buttonType);
    const QString elementIdName = buttonStateToElementId(buttonState);

    QSvgRenderer buttonRenderer {m_themePath + buttonFileName};
    buttonRenderer.render(&painter, elementIdName, DecorationPainter::ButtonGeometry);
}

QString AuroraeDecorationPainter::buttonTypeToFileName(const QString &buttonType) const
{
    if (buttonType == QStringLiteral("maximized")) {
        static const QString restoreButtonFileName = QStringLiteral("restore.svg");
        if (QFile(m_themePath + restoreButtonFileName).exists()) {
            return restoreButtonFileName;
        } else {
            return QStringLiteral("maximize.svg");
        }
    } else {
        return buttonType + QStringLiteral(".svg");
    }
}

QString AuroraeDecorationPainter::buttonStateToElementId(const QString& buttonState) const
{
    // Aurorae themes do not contain pressed and hovered inactive states,
    // so we put backdrop option to the end, so that first we use pressed and hover ones
    if (buttonState.contains(QStringLiteral("active"))) {
        return QStringLiteral("pressed-center");
    } else if (buttonState.contains(QStringLiteral("hover"))) {
        return QStringLiteral("hover-center");
    } else if (buttonState.contains(QStringLiteral("backdrop"))) {
        return QStringLiteral("inactive-center");
    } else {
        return QStringLiteral("active-center");
    }
}


