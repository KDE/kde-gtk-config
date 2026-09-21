// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QColor>
#include <QMap>
#include <QString>

// CSS in ~/.config/gtk-N.0 that our GTK modules pick up. Files are only written when their contents change
namespace CustomCssEditor
{

// Writes colors.css and has gtk.css import it
// @note Make sure "colorreload-gtk-module" is added to the module list before calling this function
void setColors(const QMap<QString, QColor> &colorsDefinitions);

// Installs the images (deleting the originals) and the CSS that uses them
void setCustomClientSideDecorations(const QStringList &windowDecorationsButtonsImages);
void disableCustomClientSideDecorations();

}
