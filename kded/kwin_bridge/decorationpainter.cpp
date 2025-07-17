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
    const QString pluginName = decorationGroup.readEntry(QStringLiteral("library"), QStringLiteral("org.kde.breeze"));

    // FIXME this does not work for Aurorae QML decorations
    static const QString auroraeThemePrefix = QStringLiteral("__aurorae__svg__");
    if (themeName.startsWith(auroraeThemePrefix)) {
        QString prefixlessThemeName = themeName.mid(auroraeThemePrefix.size());
        return std::unique_ptr<AuroraeDecorationPainter>{new AuroraeDecorationPainter(prefixlessThemeName)};
    } else {
        return std::unique_ptr<StandardDecorationPainter>{new StandardDecorationPainter(pluginName)};
    }
}
