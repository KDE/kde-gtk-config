/*
 * SPDX-FileCopyrightText: 2020 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "decorationpainter.h"

#include <QRect>
#include <QString>

#include <KConfigGroup>
#include <KSharedConfig>

#include "auroraedecorationpainter.h"
#include "standarddecorationpainter.h"

const QRect DecorationPainter::ButtonGeometry{0, 0, 50, 50};

std::unique_ptr<DecorationPainter> DecorationPainter::get()
{
    auto kwinConfig = KSharedConfig::openConfig(QStringLiteral("kwinrc"));
    KConfigGroup decorationGroup = kwinConfig->group(QStringLiteral("org.kde.kdecoration2"));
    const QString themeName = decorationGroup.readEntry(QStringLiteral("theme"), QStringLiteral("Breeze"));
    static const QString breezePlugin = QStringLiteral("org.kde.breeze");
    const QString pluginName = decorationGroup.readEntry(QStringLiteral("library"), breezePlugin);

    static const QString auroraeThemePrefix = QStringLiteral("__aurorae__svg__");
    if (themeName.startsWith(auroraeThemePrefix)) {
        QString prefixlessThemeName = themeName.mid(auroraeThemePrefix.size());
        auto decoration = std::unique_ptr<AuroraeDecorationPainter>{new AuroraeDecorationPainter(prefixlessThemeName)};
        // Use found decoration only if the geometry of the buttons is compatible (squared geometry)
        const QRect geometry = decoration->internalButtonGeometry(QStringLiteral("close"));
        if (geometry.width() > 0 && geometry.width() == geometry.height()) {
            return decoration;
        }
    } else if (pluginName == QStringLiteral("org.kde.oxygen")) {
        // Use only the Oxygen and Breeze decorations,
        // as the buttons of other decorations may be incompatible
        // due to their button geometry.
        return std::unique_ptr<StandardDecorationPainter>{new StandardDecorationPainter(pluginName)};
    }

    // Fallback to Breeze decoration plugin
    return std::unique_ptr<StandardDecorationPainter>{new StandardDecorationPainter(breezePlugin)};
}
