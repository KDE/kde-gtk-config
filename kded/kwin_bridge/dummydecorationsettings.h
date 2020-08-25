/*
 * Copyright 2020 Mikhail Zolotukhin <zomial@protonmail.com>
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

#include <KDecoration2/Private/DecorationSettingsPrivate>
#include <KDecoration2/DecorationSettings>

namespace KDecoration2 {

class DummyDecorationSettings : public KDecoration2::DecorationSettingsPrivate
{
public:
    explicit DummyDecorationSettings(DecorationSettings *parent);

    virtual bool isOnAllDesktopsAvailable() const override;
    virtual bool isAlphaChannelSupported() const override;
    virtual bool isCloseOnDoubleClickOnMenu() const override;
    virtual QVector<DecorationButtonType> decorationButtonsLeft() const override;
    virtual QVector<DecorationButtonType> decorationButtonsRight() const override;
    virtual BorderSize borderSize() const override;
};

}



