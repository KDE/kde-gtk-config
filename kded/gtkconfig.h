/*
 * Copyright (C) 2019 Mikhail Zolotukhin <zomial@protonmail.com>
 * Copyright (C) 2019 Nicolas Fella <nicolas.fella@gmx.de>
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

#include <KDEDModule>
#include <KConfigWatcher>

#include "configeditor.h"
#include "configvalueprovider.h"
#include "themepreviewer.h"

class Q_DECL_EXPORT GtkConfig : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.GtkConfig")

    enum class SettingsChangeType {
        Palette = 0,
        Font,
        Style,
        Settings,
        Icon,
        Cursor,
        ToolbarStyle,
        BlockShortcuts,
        NaturalSorting
    };

    enum class SettingsCategory {
        Mouse,
        Completion,
        Paths,
        Popupmenu,
        Qt,
        Shortcuts,
        Locale,
        Style
    };

public:
    GtkConfig(QObject *parent, const QVariantList& args);
    ~GtkConfig();

    void setFont() const;
    void setIconTheme(int iconGroup) const;
    void setCursorTheme() const;
    void setIconsOnButtons() const;
    void setIconsInMenus() const;
    void setToolbarStyle() const;
    void setScrollbarBehavior() const;
    void setDarkThemePreference() const;
    void setWindowDecorationsButtonsOrder() const;
    void setEnableAnimations() const;

    void applyAllSettings() const;

public Q_SLOTS:
    Q_SCRIPTABLE void setGtk2Theme(const QString &themeName) const;
    Q_SCRIPTABLE void setGtk3Theme(const QString &themeName) const;

    Q_SCRIPTABLE QString gtk2Theme() const;
    Q_SCRIPTABLE QString gtk3Theme() const;

    Q_SCRIPTABLE void showGtk2ThemePreview(const QString &themeName) const;
    Q_SCRIPTABLE void showGtk3ThemePreview(const QString &themeName) const;

    void onGlobalSettingsChange(int settingsChangeType, int arg) const;
    void onKdeglobalsSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const;
    void onKWinSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const;
    void onKCMInputSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const;

private:
    QScopedPointer<ConfigValueProvider> configValueProvider;
    QScopedPointer<ThemePreviewer> themePreviewer;
    KConfigWatcher::Ptr kdeglobalsConfigWatcher;
    KConfigWatcher::Ptr kwinConfigWatcher;
    KConfigWatcher::Ptr kcminputConfigWatcher;
};
