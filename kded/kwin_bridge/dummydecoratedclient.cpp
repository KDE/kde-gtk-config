/*
 * SPDX-FileCopyrightText: 2020 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "dummydecoratedclient.h"

namespace KDecoration3
{
DummyDecoratedWindow::DummyDecoratedWindow(DecoratedWindow *client, Decoration *decoration)
    : DecoratedWindowPrivate(client, decoration)
    , m_pallete(QStringLiteral("kdeglobals"))
    , m_maximized()
    , m_active(true)
{
}

bool DummyDecoratedWindow::isActive() const
{
    return m_active;
}

QString DummyDecoratedWindow::caption() const
{
    return {};
}

bool DummyDecoratedWindow::isOnAllDesktops() const
{
    return true;
}

bool DummyDecoratedWindow::isShaded() const
{
    return true;
}

QIcon DummyDecoratedWindow::icon() const
{
    return {};
}

bool DummyDecoratedWindow::isMaximized() const
{
    return m_maximized;
}

bool DummyDecoratedWindow::isMaximizedHorizontally() const
{
    return m_maximized;
}

bool DummyDecoratedWindow::isMaximizedVertically() const
{
    return m_maximized;
}

bool DummyDecoratedWindow::isKeepAbove() const
{
    return true;
}

bool DummyDecoratedWindow::isKeepBelow() const
{
    return true;
}

bool DummyDecoratedWindow::isCloseable() const
{
    return true;
}

bool DummyDecoratedWindow::isMaximizeable() const
{
    return true;
}

bool DummyDecoratedWindow::isMinimizeable() const
{
    return true;
}

bool DummyDecoratedWindow::providesContextHelp() const
{
    return true;
}

bool DummyDecoratedWindow::isModal() const
{
    return true;
}

bool DummyDecoratedWindow::isShadeable() const
{
    return true;
}

bool DummyDecoratedWindow::isMoveable() const
{
    return true;
}

bool DummyDecoratedWindow::isResizeable() const
{
    return true;
}

qreal DummyDecoratedWindow::width() const
{
    return {};
}

qreal DummyDecoratedWindow::height() const
{
    return {};
}

QSizeF DummyDecoratedWindow::size() const
{
    return {};
}

QPalette DummyDecoratedWindow::palette() const
{
    return m_pallete.palette();
}

QColor DummyDecoratedWindow::color(ColorGroup group, ColorRole role) const
{
    return m_pallete.color(group, role);
}

Qt::Edges DummyDecoratedWindow::adjacentScreenEdges() const
{
    return {};
}

QString DummyDecoratedWindow::windowClass() const
{
    return {};
}

void DummyDecoratedWindow::requestShowToolTip(const QString &text)
{
    Q_UNUSED(text)
}

void DummyDecoratedWindow::requestHideToolTip()
{
}

void DummyDecoratedWindow::requestClose()
{
}

void DummyDecoratedWindow::requestToggleMaximization(Qt::MouseButtons buttons)
{
    Q_UNUSED(buttons)
    m_maximized = !m_maximized;
}

void DummyDecoratedWindow::requestMinimize()
{
}

void DummyDecoratedWindow::requestContextHelp()
{
}

void DummyDecoratedWindow::requestToggleOnAllDesktops()
{
}

void DummyDecoratedWindow::requestToggleShade()
{
}

void DummyDecoratedWindow::requestToggleKeepAbove()
{
}

void DummyDecoratedWindow::requestToggleKeepBelow()
{
}

void DummyDecoratedWindow::requestShowWindowMenu(const QRect &rect)
{
    Q_UNUSED(rect)
}

bool DummyDecoratedWindow::hasApplicationMenu() const
{
    return false;
}

bool DummyDecoratedWindow::isApplicationMenuActive() const
{
    return false;
}

void DummyDecoratedWindow::showApplicationMenu(int actionId)
{
}

void DummyDecoratedWindow::requestShowApplicationMenu(const QRect &rect, int actionId)
{
}

void DummyDecoratedWindow::setMaximized(bool maximized)
{
    m_maximized = maximized;
}

void DummyDecoratedWindow::setActive(bool active)
{
    m_active = active;
}

qreal DummyDecoratedWindow::scale() const
{
    return 1;
}

qreal DummyDecoratedWindow::nextScale() const
{
    return 1;
}
}

#include "moc_dummydecoratedclient.cpp"
