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

#include "dummydecoratedclient.h"

namespace KDecoration2 {

DummyDecoratedClient::DummyDecoratedClient(DecoratedClient *client, Decoration *decoration)
    : DecoratedClientPrivate(client, decoration)
    , m_pallete(QStringLiteral("kdeglobals"))
    , m_maximized()
    , m_active(true)
{

}

bool DummyDecoratedClient::isActive() const
{
    return m_active;
}

QString DummyDecoratedClient::caption() const
{
    return {};
}

int DummyDecoratedClient::desktop() const
{
    return 0;
}

bool DummyDecoratedClient::isOnAllDesktops() const
{
    return true;
}

bool DummyDecoratedClient::isShaded() const
{
    return true;
}

QIcon DummyDecoratedClient::icon() const
{
    return {};
}

bool DummyDecoratedClient::isMaximized() const
{
    return m_maximized;
}

bool DummyDecoratedClient::isMaximizedHorizontally() const
{
    return m_maximized;
}

bool DummyDecoratedClient::isMaximizedVertically() const
{
    return m_maximized;
}

bool DummyDecoratedClient::isKeepAbove() const
{
    return true;
}

bool DummyDecoratedClient::isKeepBelow() const
{
    return true;
}

bool DummyDecoratedClient::isCloseable() const
{
    return true;
}

bool DummyDecoratedClient::isMaximizeable() const
{
    return true;
}

bool DummyDecoratedClient::isMinimizeable() const
{
    return true;
}

bool DummyDecoratedClient::providesContextHelp() const
{
    return true;
}

bool DummyDecoratedClient::isModal() const
{
    return true;
}

bool DummyDecoratedClient::isShadeable() const
{
    return true;
}

bool DummyDecoratedClient::isMoveable() const
{
    return true;
}

bool DummyDecoratedClient::isResizeable() const
{
    return true;
}

WId DummyDecoratedClient::windowId() const
{
    return {};
}

WId DummyDecoratedClient::decorationId() const
{
    return {};
}

int DummyDecoratedClient::width() const
{
    return {};
}

int DummyDecoratedClient::height() const
{
    return {};
}

QSize DummyDecoratedClient::size() const
{
    return {};
}

QPalette DummyDecoratedClient::palette() const
{
    return m_pallete.palette();
}

QColor DummyDecoratedClient::color(ColorGroup group, ColorRole role) const
{
    return m_pallete.color(group, role);
}

Qt::Edges DummyDecoratedClient::adjacentScreenEdges() const
{
    return {};
}

void DummyDecoratedClient::requestShowToolTip(const QString &text)
{
    Q_UNUSED(text)
}

void DummyDecoratedClient::requestHideToolTip()
{
}

void DummyDecoratedClient::requestClose()
{
}

void DummyDecoratedClient::requestToggleMaximization(Qt::MouseButtons buttons)
{
    Q_UNUSED(buttons)
    m_maximized = !m_maximized;
}

void DummyDecoratedClient::requestMinimize()
{
}

void DummyDecoratedClient::requestContextHelp()
{
}

void DummyDecoratedClient::requestToggleOnAllDesktops()
{
}

void DummyDecoratedClient::requestToggleShade()
{
}

void DummyDecoratedClient::requestToggleKeepAbove()
{
}

void DummyDecoratedClient::requestToggleKeepBelow()
{
}

void DummyDecoratedClient::requestShowWindowMenu()
{
}

void DummyDecoratedClient::setMaximized(bool maximized)
{
    m_maximized = maximized;
}

void DummyDecoratedClient::setActive(bool active)
{
    m_active = active;
}

}
