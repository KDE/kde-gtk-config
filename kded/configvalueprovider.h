/*
 * SPDX-FileCopyrightText: 2019 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <KSharedConfig>

class QString;
class QFont;

class ConfigValueProvider
{
public:
    ConfigValueProvider();

    QString fontName() const;
    QString iconThemeName() const;
    QString cursorThemeName() const;
    int cursorSize() const;
    bool iconsOnButtons() const;
    bool iconsInMenus() const;
    int toolbarStyle() const;
    bool scrollbarBehavior() const;
    bool preferDarkTheme() const;
    QString windowDecorationsButtonsOrder() const;
    QStringList windowDecorationsButtonsImages() const;
    bool enableAnimations() const;
    QMap<QString, QColor> colors() const;

private:
    QString fontStyleHelper(const QFont &font) const;
    QString windowDecorationButtonsOrderInGtkNotation(const QString &kdeConfigValue) const;
    QString currentWindowDecorationPluginPath() const;

    KSharedConfigPtr kdeglobalsConfig;
    KSharedConfigPtr inputConfig;
    KSharedConfigPtr kwinConfig;
};
