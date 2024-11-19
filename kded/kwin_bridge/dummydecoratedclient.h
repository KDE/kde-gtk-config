/*
 * SPDX-FileCopyrightText: 2020 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <KDecoration3/Private/DecoratedWindowPrivate>

#include <QPalette>

#include "decorationpalette.h"

namespace KDecoration3
{
class DummyDecoratedWindow : public QObject, public DecoratedWindowPrivate
{
    Q_OBJECT
public:
    DummyDecoratedWindow(DecoratedWindow *client, Decoration *decoration);

    bool isActive() const override;
    QString caption() const override;
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

    int width() const override;
    int height() const override;
    QSize size() const override;
    QPalette palette() const override;
    QColor color(ColorGroup group, ColorRole role) const override;
    Qt::Edges adjacentScreenEdges() const override;
    QString windowClass() const override;

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
    void requestShowWindowMenu(const QRect &rect) override;

    bool hasApplicationMenu() const override;
    bool isApplicationMenuActive() const override;
    void showApplicationMenu(int actionId) override;
    void requestShowApplicationMenu(const QRect &rect, int actionId) override;

    void setMaximized(bool maximized);
    void setActive(bool active);

private:
    KWin::Decoration::DecorationPalette m_pallete;
    bool m_maximized;
    bool m_active;
};

}
