/*
 * SPDX-FileCopyrightText: 2021 Weng Xuetian <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "config_editor/settings_ini.h"

int main()
{
    SettingsIniBackend gtk4(4);
    gtk4.unset(QStringLiteral("gtk-button-images"));
    gtk4.unset(QStringLiteral("gtk-menu-images"));
    gtk4.unset(QStringLiteral("gtk-modules"));
    gtk4.unset(QStringLiteral("gtk-toolbar-style"));
    gtk4.sync();
    return 0;
}
