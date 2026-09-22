/*
 * SPDX-FileCopyrightText: 2019 Mikhail Zolotukhin <zomial@protonmail.com>
 * SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "gtkconfig.h"

#include <KColorScheme>
#include <KPluginFactory>
#include <KWindowSystem>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDir>
#include <QFont>
#include <QGuiApplication>
#include <QStandardPaths>
#include <QTimer>

#include <vector>

#include "config_editor/custom_css.h"
#include "gsd-xsettings-manager/gsd-xsettings-manager.h"

using namespace Qt::StringLiterals;

K_PLUGIN_CLASS_WITH_JSON(GtkConfig, "gtkconfig.json")

GtkConfig::GtkConfig(QObject *parent, const QVariantList &)
    : KDEDModule(parent)
    , configValueProvider(new ConfigValueProvider())
    , themePreviewer(new ThemePreviewer(this))
    , kdeglobalsConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig()))
    , kwinConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("kwinrc"))))
    , kcminputConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("kcminputrc"))))
    , breezeConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("breezerc"))))
{
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerService(QStringLiteral("org.kde.GtkConfig"));
    dbus.registerObject(QStringLiteral("/GtkConfig"), this, QDBusConnection::ExportScriptableSlots);

    if (qgetenv("GTK_USE_PORTAL") != "1" && KWindowSystem::isPlatformWayland()) {
        m_gsdXsettingsManager = new GSDXSettingsManager(this);
    }

    // Any change re-applies everything; backends only write out values that did change
    for (const auto &watcher : {kdeglobalsConfigWatcher, kwinConfigWatcher, kcminputConfigWatcher, breezeConfigWatcher}) {
        connect(watcher.data(), &KConfigWatcher::configChanged, this, &GtkConfig::applyAllSettings);
    }

    applyAllSettings();
}

GtkConfig::~GtkConfig()
{
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.unregisterService(QStringLiteral("org.kde.GtkConfig"));
    dbus.unregisterObject(QStringLiteral("/GtkConfig"));
}

namespace
{

QStringList gtkThemeSearchPaths()
{
    QStringList paths;
    paths << QDir::homePath() + QStringLiteral("/.themes/");
    paths << QDir::homePath() + QStringLiteral("/.local/share/themes/");

    for (const QString &dataLocation : QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation)) {
        paths << dataLocation + QStringLiteral("/themes/");
    }

    return paths;
}

bool gtkThemeExists(const QString &themeName)
{
    if (themeName.isEmpty()) {
        return false;
    }

    for (const QString &themeBasePath : gtkThemeSearchPaths()) {
        if (QDir(themeBasePath + themeName).exists()) {
            return true;
        }
    }

    return false;
}

QString findGtk2DarkThemeVariant(const QString &themeName)
{
    static const QStringList suffixes =
        {QStringLiteral("-dark"), QStringLiteral("-Dark"), QStringLiteral(" dark"), QStringLiteral(" Dark"), QStringLiteral("dark"), QStringLiteral("Dark")};

    for (const QString &suffix : suffixes) {
        const QString candidate = themeName + suffix;
        if (gtkThemeExists(candidate)) {
            return candidate;
        }
    }

    return {};
}

// GTK 2 does not support using dark variant automatically, so we have to find a dark theme folder variant if available
QString gtk2ThemeName(const QString &themeName, bool preferDarkTheme)
{
    if (preferDarkTheme) {
        const QString darkVariant = findGtk2DarkThemeVariant(themeName);
        if (!darkVariant.isEmpty()) {
            return darkVariant;
        }
    }
    return themeName;
}
}

void GtkConfig::setGtkTheme(const QString &themeName)
{
    m_settingsIni.set(u"gtk-theme-name"_s, themeName);
    m_settingsIni.sync(); // applyAllSettings() reads it back
    applyAllSettings();
}

QString GtkConfig::gtkTheme() const
{
    const QString theme = SettingsIniBackend::value(u"gtk-theme-name"_s, 3);
    return theme.isEmpty() ? u"Breeze"_s : theme;
}

void GtkConfig::showGtkThemePreview(const QString &themeName) const
{
    const bool darkTheme = configValueProvider->preferDarkTheme();
    const auto themeVariant = darkTheme ? QStringLiteral(":dark") : QStringLiteral(":light");
    themePreviewer->showGtk3App(themeName + themeVariant);
}

void GtkConfig::applyAllSettings()
{
    const ConfigValueProvider &p = *configValueProvider;

    const QString theme = gtkTheme();
    const bool preferDark = p.preferDarkTheme();
    // https://gitlab.gnome.org/GNOME/gsettings-desktop-schemas/-/blob/master/headers/gdesktop-enums.h
    const GSettingsEnum colorScheme{preferDark ? 1 /*G_DESKTOP_COLOR_SCHEME_PREFER_DARK*/ : 2 /*G_DESKTOP_COLOR_SCHEME_PREFER_LIGHT*/};

    const int cursorSize = p.cursorSize();
    const qreal xwaylandScale = KWindowSystem::isPlatformWayland() ? p.x11GlobalScaleFactor() : 1.0;
    const bool cursorBlink = p.cursorBlinkRate() > 0;
    // Range for cursor-blink-time in GSettings. Qt's GTK 3 platform theme didn't honor blink enabled properly before,
    // set a sane default value when it's off to keep it from blinking like crazy.
    const int cursorBlinkTime = cursorBlink ? qBound(100, p.cursorBlinkRate(), 2500) : 1000;

    const bool enableAnimations = p.enableAnimations();

    const double x11Scale = p.x11GlobalScaleFactor();
    const int x11TextDpi = 96 * 1024 * x11Scale;

    struct Target {
        EditorBackend &backend;
        QString key;
    };
    struct Setting {
        std::vector<Target> targets;
        QVariant value;
    };
    // clang-format off
    const Setting settings[] = {
        {{
            {m_settingsIni, u"gtk-theme-name"_s},
            {m_gsettings, u"gtk-theme"_s},
        }, theme},
        {{
            {m_gtk2, u"gtk-theme-name"_s},
            {m_xsettings, u"Net/ThemeName"_s},
        }, gtk2ThemeName(theme, preferDark)},
        {{
            {m_settingsIni, u"gtk-application-prefer-dark-theme"_s},
        }, preferDark},
        {{
            {m_gsettings, u"color-scheme"_s},
        }, QVariant::fromValue(colorScheme)},

        {{
            {m_gtk2, u"gtk-font-name"_s},
            {m_settingsIni, u"gtk-font-name"_s},
            {m_xsettings, u"Gtk/FontName"_s},
        }, p.fontName(false)},
        {{
            {m_gsettings, u"font-name"_s},
            {m_gsettings, u"document-font-name"_s},
        }, p.fontName(true)},
        {{
            {m_gsettings, u"monospace-font-name"_s},
        }, p.fixedName(true)},

        {{
            {m_gtk2, u"gtk-icon-theme-name"_s},
            {m_settingsIni, u"gtk-icon-theme-name"_s},
            {m_xsettings, u"Net/IconThemeName"_s},
            {m_gsettings, u"icon-theme"_s},
        }, p.iconThemeName()},

        {{
            {m_gtk2, u"gtk-cursor-theme-name"_s},
            {m_settingsIni, u"gtk-cursor-theme-name"_s},
            {m_xsettings, u"Gtk/CursorThemeName"_s},
            {m_gsettings, u"cursor-theme"_s},
        }, p.cursorThemeName()},
        {{
            {m_gtk2, u"gtk-cursor-theme-size"_s},
            {m_settingsIni, u"gtk-cursor-theme-size"_s},
            {m_gsettings, u"cursor-size"_s},
        }, cursorSize},
        {{
            {m_xsettings, u"Gtk/CursorThemeSize"_s},
        }, int(cursorSize * xwaylandScale)},
        {{
            {m_gtk2, u"gtk-cursor-blink"_s},
            {m_settingsIni, u"gtk-cursor-blink"_s},
            {m_xsettings, u"Net/CursorBlink"_s},
            {m_gsettings, u"cursor-blink"_s},
        }, cursorBlink},
        {{
            {m_gtk2, u"gtk-cursor-blink-time"_s},
            {m_settingsIni, u"gtk-cursor-blink-time"_s},
            {m_xsettings, u"Net/CursorBlinkTime"_s},
            {m_gsettings, u"cursor-blink-time"_s},
        }, cursorBlinkTime},

        {{
            {m_gtk2, u"gtk-sound-theme-name"_s},
            {m_settingsIni, u"gtk-sound-theme-name"_s},
            {m_xsettings, u"Net/SoundThemeName"_s},
            {m_gsettingsSound, u"theme-name"_s},
        }, p.soundThemeName()},
        {{
            {m_gtk2, u"gtk-enable-event-sounds"_s},
            {m_settingsIni, u"gtk-enable-event-sounds"_s},
            {m_xsettings, u"Net/EnableEventSounds"_s},
            {m_gsettingsSound, u"event-sounds"_s},
        }, p.eventSoundsEnabled()},

        // GTK 4 dropped these three
        {{
            {m_gtk2, u"gtk-button-images"_s},
            {m_settingsIni3, u"gtk-button-images"_s},
            {m_xsettings, u"Gtk/ButtonImages"_s},
        }, p.iconsOnButtons()},
        {{
            {m_gtk2, u"gtk-menu-images"_s},
            {m_settingsIni3, u"gtk-menu-images"_s},
            {m_xsettings, u"Gtk/MenuImages"_s},
        }, p.iconsInMenus()},
        {{
            {m_gtk2, u"gtk-toolbar-style"_s},
            {m_settingsIni3, u"gtk-toolbar-style"_s},
            {m_xsettings, u"Gtk/ToolbarStyle"_s},
        }, p.toolbarStyle()},
        {{
            {m_gsettings, u"toolbar-style"_s},
        }, QVariant::fromValue(GSettingsEnum{p.toolbarStyle()})},

        {{
            {m_gtk2, u"gtk-primary-button-warps-slider"_s},
            {m_settingsIni, u"gtk-primary-button-warps-slider"_s},
            {m_xsettings, u"Gtk/PrimaryButtonWarpsSlider"_s},
        }, p.scrollbarBehavior()},
        {{
            {m_gtk2, u"gtk-double-click-time"_s},
            {m_settingsIni, u"gtk-double-click-time"_s},
            {m_xsettings, u"Net/DoubleClickTime"_s},
            {m_gsettingsMouse, u"double-click"_s},
        }, p.doubleClickInterval()},

        {{
            {m_gtk2, u"gtk-enable-animations"_s},
            {m_xsettings, u"Gtk/EnableAnimations"_s},
            {m_gsettings, u"enable-animations"_s},
        }, enableAnimations},

        {{
            {m_settingsIni, u"gtk-decoration-layout"_s},
            {m_xsettings, u"Gtk/DecorationLayout"_s},
            {m_gsettingsWm, u"button-layout"_s},
        }, p.windowDecorationsButtonsOrder()},

        {{
            {m_xsettings, u"Gdk/WindowScalingFactor"_s},
            {m_gsettings, u"scaling-factor"_s}, // For IntelliJ IDEA
        }, unsigned(x11Scale)},
        {{
            {m_xsettings, u"Xft/DPI"_s},
        }, QVariant()},
        {{
            {m_xsettings, u"Gdk/UnscaledDPI"_s},
        }, x11TextDpi / int(x11Scale)},
        {{
            {m_settingsIni, u"gtk-xft-dpi"_s},
        }, x11TextDpi},
        {{
            {m_gsettings, u"text-scaling-factor"_s},
        }, 1.0},
    };
    // clang-format on
    for (const auto &[targets, value] : settings) {
        for (const auto &[backend, key] : targets) {
            backend.set(key, value);
        }
    }

    // GSD serves this settings.ini key over D-Bus
    const bool animationsChanged = SettingsIniBackend::value<bool>(u"gtk-enable-animations"_s, 3) != enableAnimations;
    m_settingsIni.set(u"gtk-enable-animations"_s, enableAnimations);
    if (animationsChanged && m_gsdXsettingsManager) {
        m_gsdXsettingsManager->enableAnimationsChanged();
    }

    applyModules(theme);
    applyWindowDecorations(theme);
    applyColors();

    syncBackends();
}

void GtkConfig::applyModules(const QString &theme)
{
    // Our GTK 3 modules, added to whatever the user has (and never removed). GTK 4 has no modules
    const QString currentModules = SettingsIniBackend::value(u"gtk-modules"_s, 3);
    QStringList modules = currentModules.split(u':', Qt::SkipEmptyParts);
    QStringList ourModules{u"colorreload-gtk-module"_s};
    if (theme == "Breeze"_L1) { // Only Breeze GTK supports custom decoration buttons
        ourModules << u"window-decorations-gtk-module"_s;
    }
    for (const QString &module : std::as_const(ourModules)) {
        if (!modules.contains(module)) {
            modules << module;
        }
    }
    m_settingsIni3.set(u"gtk-modules"_s, modules.join(u':'));
    if (modules.join(u':') != currentModules && m_gsdXsettingsManager) {
        m_gsdXsettingsManager->modulesChanged(); // GSD serves this settings.ini key over D-Bus
    }
}

void GtkConfig::applyWindowDecorations(const QString &theme)
{
    // Window decorations are part of the theme, in case of Breeze we inject custom ones from KWin
    if (theme == "Breeze"_L1) {
        CustomCssEditor::setCustomClientSideDecorations(configValueProvider->windowDecorationsButtonsImages());
    } else {
        CustomCssEditor::disableCustomClientSideDecorations();
    }
}

void GtkConfig::applyColors()
{
    // modulesChanged signal will take some time to reach a GTK app, so explicitly wait a moment
    QTimer::singleShot(200, this, [colors = configValueProvider->colors()] {
        CustomCssEditor::setColors(colors);
    });
}

void GtkConfig::syncBackends()
{
    m_gtk2.sync();
    m_settingsIni.sync();
    m_settingsIni3.sync();
    m_xsettings.sync();
    m_gsettings.sync();
    m_gsettingsSound.sync();
    m_gsettingsMouse.sync();
    m_gsettingsWm.sync();
}

#include "gtkconfig.moc"

#include "moc_gtkconfig.cpp"
