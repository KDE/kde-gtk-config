/*
 * Copyright (C) 2020 Mikhail Zolotukhin <zomial@protonmail.com>
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

#pragma once

#include <KDecoration2/Private/DecoratedClientPrivate>

#include <QPalette>

#include "decorationpalette.h"

namespace KDecoration2 {

class DummyDecoratedClient : public QObject, public DecoratedClientPrivate {
    Q_OBJECT
public:
    DummyDecoratedClient(DecoratedClient *client, Decoration *decoration);

    bool isActive() const override;
    QString caption() const override;
    int desktop() const override;
    bool isOnAllDesktops() const override;
    bool isShaded() const override;
    QIcon icon() const override;
    bool isMaximized() const override;
    bool isMaximizedHorizontally() const override;
    bool isMaximizedVertically() const override;
    bool isKeepAbove() const override;
    bool isKeepBelow() const override;

    bool isCloseable() const override;
    bool isMaximizeable() const override;
    bool isMinimizeable() const override;
    bool providesContextHelp() const override;
    bool isModal() const override;
    bool isShadeable() const override;
    bool isMoveable() const override;
    bool isResizeable() const override;

    WId windowId() const override;
    WId decorationId() const override;

    int width() const override;
    int height() const override;
    QSize size() const override;
    QPalette palette() const override;
    QColor color(ColorGroup group, ColorRole role) const override;
    Qt::Edges adjacentScreenEdges() const override;

    void requestShowToolTip(const QString &text) override;

    void requestHideToolTip() override;
    void requestClose() override;
    void requestToggleMaximization(Qt::MouseButtons buttons) override;
    void requestMinimize() override;
    void requestContextHelp() override;
    void requestToggleOnAllDesktops() override;
    void requestToggleShade() override;
    void requestToggleKeepAbove() override;
    void requestToggleKeepBelow() override;
    void requestShowWindowMenu() override;

    void setMaximized(bool maximized);
    void setActive(bool active);

private:
    KWin::Decoration::DecorationPalette m_pallete;
    bool m_maximized;
    bool m_active;
};

}
