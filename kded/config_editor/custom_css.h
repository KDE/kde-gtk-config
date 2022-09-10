// SPDX-FileCopyrightText: 2019, 2022 Mikhail Zolotukhin <zomial@protonmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#pragma once

#include <QColor>
#include <QString>

namespace CustomCssEditor
{

void setColors(const QMap<QString, QColor> &colorsDefinitions);
void setCustomClientSideDecorations(const QStringList &windowDecorationsButtonsImages);
void disableCustomClientSideDecorations();

}
