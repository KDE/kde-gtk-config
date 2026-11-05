/*
 * SPDX-FileCopyrightText: 2026 David Edmundson <kde@davidedmundson.co.uk>
 * SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <KConfigWatcher>
#include <KDEDModule>

#include "configvalueprovider.h"

class Q_DECL_EXPORT GtkConfigXSettings : public KDEDModule
{
    Q_OBJECT

public:
    GtkConfigXSettings(QObject *parent, const QVariantList &args);

    void setGtk2Theme(const QString &themeName, const bool preferDarkTheme) const;
    void setFont() const;
    void setIconTheme() const;
    void setSoundTheme() const;
    void setEventSoundsEnabled() const;
    void setCursorTheme() const;
    void setCursorSize() const;
    void setIconsOnButtons() const;
    void setIconsInMenus() const;
    void setToolbarStyle() const;
    void setScrollbarBehavior() const;
    void setDoubleClickInterval() const;
    void setCursorBlinkRate() const;
    void setWindowDecorationsButtonsOrder() const;
    void setEnableAnimations() const;
    void setGlobalScale() const;
    void setTextScale() const;

    void applyAllSettings() const;

public Q_SLOTS:
    void onGtk2ThemeChanged(const QString &themeName) const;
    void onKdeglobalsSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const;
    void onKWinSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const;
    void onKCMInputSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const;

private:
    QScopedPointer<ConfigValueProvider> configValueProvider;
    KConfigWatcher::Ptr kdeglobalsConfigWatcher;
    KConfigWatcher::Ptr kwinConfigWatcher;
    KConfigWatcher::Ptr kcminputConfigWatcher;
};
