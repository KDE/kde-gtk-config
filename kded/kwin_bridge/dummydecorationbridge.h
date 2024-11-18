/*
 * SPDX-FileCopyrightText: 2020 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QPluginLoader>
#include <QString>

#include <KDecoration3/Decoration>
#include <KDecoration3/Private/DecorationBridge>
#include <kdecoration3/decorationdefines.h>

class KPluginFactory;

namespace KDecoration3
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

    std::unique_ptr<KDecoration3::DecorationSettingsPrivate> settings(KDecoration3::DecorationSettings *parent) override;
    std::unique_ptr<KDecoration3::DecoratedClientPrivate> createClient(KDecoration3::DecoratedClient *client, KDecoration3::Decoration *decoration) override;

    void paintButton(QPainter &painter, const QString &buttonType, const QString &buttonState);

private:
    void disableAnimations();
    void enableAnimations();

    QString windowDecorationPluginPath(const QString &decorationTheme) const;

    void passMouseHoverEventToButton(KDecoration3::DecorationButton *button) const;
    void passMousePressEventToButton(KDecoration3::DecorationButton *button) const;

    KDecoration3::DecorationButtonType strToButtonType(const QString &type) const;

    QString m_decorationsConfigFileName;
    double globalAnimationEntryValue;

    QPluginLoader m_pluginLoader;
    KPluginFactory *m_factory;
    KDecoration3::Decoration *m_decoration;
    KDecoration3::DecoratedClientPrivate *m_client;
    KDecoration3::DummyDecorationSettings *m_settings;
};

}
