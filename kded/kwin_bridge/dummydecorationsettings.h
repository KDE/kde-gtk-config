/*
 * SPDX-FileCopyrightText: 2020 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <KDecoration3/DecorationSettings>
#include <KDecoration3/Private/DecorationSettingsPrivate>

namespace KDecoration3
{
class DummyDecorationSettings : public KDecoration3::DecorationSettingsPrivate
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
