/*
 * SPDX-FileCopyrightText: 2019 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <KSharedConfig>

class QString;
class QFont;
class KColorScheme;

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
    static QString fontStyleHelper(const QFont &font) ;
    static QString windowDecorationButtonsOrderInGtkNotation(const QString &kdeConfigValue) ;

    static QString fontWeightAsString(int weight);
    static QString fontStyleAsString(int style);
    static QString fontStretchAsString(int fontStretch);

    static QMap<QString, QColor> normalColors(QHash<QString, QHash<QString, KColorScheme>> csc);
    static QMap<QString, QColor> backdropColors(QHash<QString, QHash<QString, KColorScheme>> csc);
    static QMap<QString, QColor> insensitiveColors(QHash<QString, QHash<QString, KColorScheme>> csc);
    static QMap<QString, QColor> insensitiveBackdropColors(QHash<QString, QHash<QString, KColorScheme>> csc);
    QMap<QString, QColor> ignorantColors(QHash<QString, QHash<QString, KColorScheme>> csc) const;


    KSharedConfigPtr kdeglobalsConfig;
    KSharedConfigPtr inputConfig;
    KSharedConfigPtr kwinConfig;
};
