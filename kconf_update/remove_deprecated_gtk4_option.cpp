/*
 * SPDX-FileCopyrightText: 2021 Weng Xuetian <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "configeditor.h"

int main()
{
    ConfigEditor::unsetGtk4ConfigValueSettingsIni(QStringLiteral("gtk-button-images"));
    ConfigEditor::unsetGtk4ConfigValueSettingsIni(QStringLiteral("gtk-menu-images"));
    ConfigEditor::unsetGtk4ConfigValueSettingsIni(QStringLiteral("gtk-toolbar-style"));
    return 0;
}
