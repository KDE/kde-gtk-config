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

#include "configeditor.h"
#include "configvalueprovider.h"

class Q_DECL_EXPORT GtkConfig : public KDEDModule
{
    Q_OBJECT

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

    void setFont() const;
    void setIconTheme(int iconGroup) const;
    void setCursorTheme() const;
    void setIconsOnButtons() const;
    void setIconsInMenus() const;
    void setToolbarStyle() const;
    void setScrollbarBehavior() const;
    void setDarkThemePreference() const;

    void applyAllSettings() const;

public Q_SLOTS:
    void onGlobalSettingsChange(int settingsChangeType, int arg) const;

private:
    QScopedPointer<ConfigValueProvider> configValueProvider;
};
