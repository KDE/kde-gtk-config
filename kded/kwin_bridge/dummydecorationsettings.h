/*
 * SPDX-FileCopyrightText: 2020 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <KDecoration2/DecorationSettings>
#include <KDecoration2/Private/DecorationSettingsPrivate>

namespace KDecoration2
{
class DummyDecorationSettings : public KDecoration2::DecorationSettingsPrivate
{
public:
    explicit DummyDecorationSettings(DecorationSettings *parent);

    bool isOnAllDesktopsAvailable() const override;
    bool isAlphaChannelSupported() const override;
    bool isCloseOnDoubleClickOnMenu() const override;
    QList<DecorationButtonType> decorationButtonsLeft() const override;
    QList<DecorationButtonType> decorationButtonsRight() const override;
    BorderSize borderSize() const override;
};

}
