/*
 * SPDX-FileCopyrightText: 2019 Mikhail Zolotukhin <zomial@protonmail.com>
 * SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <KConfigWatcher>
#include <KDEDModule>

#include "configeditor.h"
#include "configvalueprovider.h"
#include "themepreviewer.h"

class Q_DECL_EXPORT GtkConfig : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.GtkConfig")

public:
    GtkConfig(QObject *parent, const QVariantList &args);
    ~GtkConfig();

    void setFont() const;
    void setIconTheme() const;
    void setCursorTheme() const;
    void setCursorSize() const;
    void setIconsOnButtons() const;
    void setIconsInMenus() const;
    void setToolbarStyle() const;
    void setScrollbarBehavior() const;
    void setDarkThemePreference() const;
    void setWindowDecorationsAppearance() const;
    void setWindowDecorationsButtonsOrder() const;
    void setEnableAnimations() const;
    void setColors() const;

    void applyAllSettings() const;

public Q_SLOTS:
    Q_SCRIPTABLE void setGtkTheme(const QString &themeName) const;
    Q_SCRIPTABLE QString gtkTheme() const;
    Q_SCRIPTABLE void showGtkThemePreview(const QString &themeName) const;

    void onKdeglobalsSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const;
    void onKWinSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const;
    void onKCMInputSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const;
    void onBreezeSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const;

private:
    QScopedPointer<ConfigValueProvider> configValueProvider;
    QScopedPointer<ThemePreviewer> themePreviewer;
    KConfigWatcher::Ptr kdeglobalsConfigWatcher;
    KConfigWatcher::Ptr kwinConfigWatcher;
    KConfigWatcher::Ptr kcminputConfigWatcher;
    KConfigWatcher::Ptr breezeConfigWatcher;
};
