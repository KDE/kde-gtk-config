/*
 * SPDX-FileCopyrightText: 2020 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QCoreApplication>
#include <QDebug>
#include <QMouseEvent>

#include <KConfigGroup>
#include <KDecoration3/DecoratedWindow>
#include <KDecoration3/DecorationSettings>
#include <KDecoration3/Private/DecoratedWindowPrivate>
#include <KDecoration3/Private/DecorationSettingsPrivate>
#include <KPluginFactory>
#include <KPluginMetaData>
#include <KSharedConfig>

#include "debug.h"
#include "decorationpainter.h"
#include "dummydecoratedclient.h"
#include "dummydecorationbridge.h"
#include "dummydecorationsettings.h"

namespace KDecoration3
{
DummyDecorationBridge::DummyDecorationBridge(const QString &decorationTheme, QObject *parent)
    : DecorationBridge(parent)
    , m_decorationsConfigFileName()
    , m_factory()
    , m_decoration()
    , m_client()
{
    // HACK:
    // Some window decorations use button fade-in and fade-out animations.
    // These animations are very slow, and they are preventing this bridge
    // to correctly draw hover states of the buttons, when we pass a "hover"
    // event to them. To avoid this harmful side effect we use a hack:
    // We disable the animations via user configuration file temporary if
    // they were enabled, draw a buttons and then enable them again.
    if (decorationTheme == QStringLiteral("Oxygen")) {
        m_decorationsConfigFileName = QStringLiteral("oxygenrc");
    } else { // for Breeze window decorations and its forks
        m_decorationsConfigFileName = QStringLiteral("breezerc");
    }

    disableAnimations();

    const QString pluginPath = windowDecorationPluginPath(decorationTheme);
    m_pluginLoader.setFileName(pluginPath);
    m_factory = qobject_cast<KPluginFactory *>(m_pluginLoader.instance());

    if (m_factory) {
        const QVariantMap args({{QStringLiteral("bridge"), QVariant::fromValue(this)}});
        m_decoration = m_factory->create<KDecoration3::Decoration>(m_factory, QVariantList({args}));

        if (m_decoration) {
            auto decorationSettings = std::make_shared<KDecoration3::DecorationSettings>(this);
            m_decoration->setSettings(decorationSettings);
            m_decoration->create();
            m_decoration->init();
            m_decoration->apply(m_decoration->nextState()->clone());

            // Update decoration settings, e.g. Breeze's "Draw a circle around close button"
            if (m_settings) {
                Q_EMIT m_settings->decorationSettings()->reconfigured();
            }
            enableAnimations();
        } else {
            qCWarning(KWINBRIDGE_LOG) << "Loading decoration" << pluginPath << "failed" << m_pluginLoader.errorString();
        }
    } else {
        qCWarning(KWINBRIDGE_LOG) << "Loading factory for decoration" << pluginPath << "failed" << m_pluginLoader.errorString();
    }
}

DummyDecorationBridge::~DummyDecorationBridge()
{
    m_pluginLoader.unload();
}

std::unique_ptr<KDecoration3::DecorationSettingsPrivate> DummyDecorationBridge::settings(KDecoration3::DecorationSettings *parent)
{
    auto newSettings = std::unique_ptr<DummyDecorationSettings>(new DummyDecorationSettings(parent));
    m_settings = newSettings.get();
    return newSettings;
}

std::unique_ptr<KDecoration3::DecoratedWindowPrivate> DummyDecorationBridge::createClient(KDecoration3::DecoratedWindow *client,
                                                                                          KDecoration3::Decoration *decoration)
{
    auto ptr = std::unique_ptr<DummyDecoratedWindow>(new DummyDecoratedWindow(client, decoration));
    m_client = ptr.get();
    return ptr;
}

void DummyDecorationBridge::paintButton(QPainter &painter, const QString &buttonType, const QString &buttonState)
{
    if (!m_factory) {
        return;
    }

    disableAnimations();
    std::unique_ptr<KDecoration3::DecorationButton> button{
        m_factory->create<KDecoration3::DecorationButton>(m_decoration,
                                                          QVariantList({
                                                              QVariant::fromValue(strToButtonType(buttonType)),
                                                              QVariant::fromValue(m_decoration),
                                                          }))};

    if (button == nullptr) {
        return;
    }

    button->setGeometry(DecorationPainter::ButtonGeometry);

    if (buttonType == QStringLiteral("maximized")) {
        // Different decorations use different ways to know if the window is maximized
        // For example Breeze uses 'checked' property, but Oxygen uses client's 'isMaximized' method
        button->setChecked(true);
        if (m_client) {
            dynamic_cast<DummyDecoratedWindow *>(m_client)->setMaximized(true);
        }
    }

    if (buttonState.contains(QStringLiteral("active"))) {
        passMousePressEventToButton(button.get());
    } else if (buttonState.contains(QStringLiteral("hover"))) {
        passMouseHoverEventToButton(button.get());
    }

    if (buttonState.contains(QStringLiteral("backdrop"))) {
        if (m_client) {
            dynamic_cast<DummyDecoratedWindow *>(m_client)->setActive(false);
        }
    } else {
        if (m_client) {
            dynamic_cast<DummyDecoratedWindow *>(m_client)->setActive(true);
        }
    }

    button->paint(&painter, DecorationPainter::ButtonGeometry);
    enableAnimations();
}

void DummyDecorationBridge::disableAnimations()
{
    KSharedConfig::Ptr decorationConfig = KSharedConfig::openConfig(m_decorationsConfigFileName, KConfig::NoGlobals);
    if (decorationConfig) {
        KConfigGroup group = decorationConfig->group(QStringLiteral("Windeco"));
        group.writeEntry(QStringLiteral("AnimationsEnabled"), false, KConfig::WriteConfigFlags());
    }

    // In case decoration is using global animation settings
    KSharedConfig::Ptr globalConfig = KSharedConfig::openConfig();
    if (globalConfig) {
        auto group = globalConfig->group(QStringLiteral("KDE"));
        globalAnimationEntryValue = group.readEntry(QStringLiteral("AnimationDurationFactor"), 1.0);
        group.writeEntry(QStringLiteral("AnimationDurationFactor"), 0, KConfig::WriteConfigFlags());
    }
}

void DummyDecorationBridge::enableAnimations()
{
    KSharedConfig::Ptr globalConfig = KSharedConfig::openConfig();
    if (globalConfig) {
        auto group = globalConfig->group(QStringLiteral("KDE"));
        group.writeEntry(QStringLiteral("AnimationDurationFactor"), globalAnimationEntryValue, KConfig::WriteConfigFlags());
    }
}

QString DummyDecorationBridge::windowDecorationPluginPath(const QString &decorationTheme) const
{
    const auto decorationPlugins = KPluginMetaData::findPlugins(QStringLiteral("org.kde.kdecoration3"));

    QString defaultPluginPath;

    for (const auto &pluginMetaData : decorationPlugins) {
        if (pluginMetaData.pluginId() == QLatin1String("org.kde.breeze")) {
            defaultPluginPath = pluginMetaData.fileName();
        }

        if (pluginMetaData.name() == decorationTheme) {
            return pluginMetaData.fileName();
        }
    }
    return defaultPluginPath;
}

void DummyDecorationBridge::passMouseHoverEventToButton(KDecoration3::DecorationButton *button) const
{
    QHoverEvent event{QEvent::HoverEnter,
                      {
                          DecorationPainter::ButtonGeometry.width() / 2.0,
                          DecorationPainter::ButtonGeometry.height() / 2.0,
                      },
                      {
                          (DecorationPainter::ButtonGeometry.width() / 2.0) - 1,
                          (DecorationPainter::ButtonGeometry.height() / 2.0) - 1,
                      },
                      Qt::NoModifier};
    QCoreApplication::instance()->sendEvent(button, &event);
}

void DummyDecorationBridge::passMousePressEventToButton(KDecoration3::DecorationButton *button) const
{
    QMouseEvent event{QEvent::MouseButtonPress,
                      {
                          DecorationPainter::ButtonGeometry.width() / 2.0,
                          DecorationPainter::ButtonGeometry.height() / 2.0,
                      },
                      Qt::LeftButton,
                      Qt::LeftButton,
                      Qt::NoModifier};
    QCoreApplication::instance()->sendEvent(button, &event);
}

KDecoration3::DecorationButtonType DummyDecorationBridge::strToButtonType(const QString &type) const
{
    if (type == QStringLiteral("minimize")) {
        return KDecoration3::DecorationButtonType::Minimize;
    } else if (type == QStringLiteral("close")) {
        return KDecoration3::DecorationButtonType::Close;
    } else {
        return KDecoration3::DecorationButtonType::Maximize;
    }
}

}

#include "moc_dummydecorationbridge.cpp"
