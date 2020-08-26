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

#include <QString>

#include <KDecoration2/Private/DecorationBridge>
#include <KDecoration2/Decoration>
#include <kdecoration2/decorationdefines.h>

#include <memory>

class KPluginFactory;
class KPluginLoader;

namespace KDecoration2
{

class DecorationSettings;
class DecoratedClientPrivate;
class DecorationSettingsPrivate;
class DummyDecorationSettings;

class DummyDecorationBridge : public DecorationBridge
{
    Q_OBJECT
public:
    DummyDecorationBridge(const QString &decorationTheme, QObject *parent = nullptr);
    ~DummyDecorationBridge() override;

    std::unique_ptr<KDecoration2::DecorationSettingsPrivate> settings(KDecoration2::DecorationSettings *parent) override;
    void update(KDecoration2::Decoration *decoration, const QRect &geometry) override;
    std::unique_ptr<KDecoration2::DecoratedClientPrivate> createClient(KDecoration2::DecoratedClient *client, KDecoration2::Decoration *decoration) override;

    void paintButton(QPainter &painter, const QString &buttonType, const QString &buttonState);

private:
    void disableAnimations();
    void enableAnimationsIfTheyWereEnabled();

    QString windowDecorationPluginPath(const QString &decorationTheme) const;

    void passMouseHoverEventToButton(KDecoration2::DecorationButton *button) const;
    void passMousePressEventToButton(KDecoration2::DecorationButton *button) const;

    KDecoration2::DecorationButtonType strToButtonType(const QString &type) const;

    QString m_decorationsConfigFileName;
    bool m_wereAnimationsEnabled;

    std::unique_ptr<KPluginLoader> m_loader;
    KPluginFactory *m_factory;
    KDecoration2::Decoration *m_decoration;
    KDecoration2::DecoratedClientPrivate *m_client;
    KDecoration2::DummyDecorationSettings *m_settings;
};

}


