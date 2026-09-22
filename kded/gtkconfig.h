/*
 * SPDX-FileCopyrightText: 2019 Mikhail Zolotukhin <zomial@protonmail.com>
 * SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <KConfigWatcher>
#include <KDEDModule>

#include "config_editor/gsettings.h"
#include "config_editor/gtk2.h"
#include "config_editor/settings_ini.h"
#include "config_editor/xsettings.h"
#include "configvalueprovider.h"
#include "themepreviewer.h"

class GSDXSettingsManager;

class Q_DECL_EXPORT GtkConfig : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.GtkConfig")

public:
    GtkConfig(QObject *parent, const QVariantList &args);
    ~GtkConfig();

    // Applies every setting to every backend; they only write out what changed
    void applyAllSettings();

public Q_SLOTS:
    Q_SCRIPTABLE void setGtkTheme(const QString &themeName);
    Q_SCRIPTABLE QString gtkTheme() const;
    Q_SCRIPTABLE void showGtkThemePreview(const QString &themeName) const;

private:
    QScopedPointer<ConfigValueProvider> configValueProvider;
    QScopedPointer<ThemePreviewer> themePreviewer;
    KConfigWatcher::Ptr kdeglobalsConfigWatcher;
    KConfigWatcher::Ptr kwinConfigWatcher;
    KConfigWatcher::Ptr kcminputConfigWatcher;
    KConfigWatcher::Ptr breezeConfigWatcher;
    void applyModules(const QString &theme);
    void applyWindowDecorations(const QString &theme);
    void applyColors();
    void syncBackends();

    Gtk2Backend m_gtk2;
    SettingsIniBackend m_settingsIni; // GTK 3 and 4
    SettingsIniBackend m_settingsIni3{3}; // for settings GTK 4 dropped
    XSettingsBackend m_xsettings;
    GLibSettingsBackend m_gsettings{"org.gnome.desktop.interface"};
    GLibSettingsBackend m_gsettingsSound{"org.gnome.desktop.sound"};
    GLibSettingsBackend m_gsettingsMouse{"org.gnome.desktop.peripherals.mouse"};
    GLibSettingsBackend m_gsettingsWm{"org.gnome.desktop.wm.preferences"};

    GSDXSettingsManager *m_gsdXsettingsManager = nullptr;
};
