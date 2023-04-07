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
#include <QFont>
#include <QGuiApplication>

#include <algorithm>
#include <cmath>

#include "config_editor/custom_css.h"
#include "config_editor/gsettings.h"
#include "config_editor/gtk2.h"
#include "config_editor/settings_ini.h"
#include "config_editor/xsettings.h"
#include "gsd-xsettings-manager/gsd-xsettings-manager.h"

K_PLUGIN_CLASS_WITH_JSON(GtkConfig, "gtkconfig.json")

GtkConfig::GtkConfig(QObject *parent, const QVariantList &)
    : KDEDModule(parent)
    , configValueProvider(new ConfigValueProvider())
    , themePreviewer(new ThemePreviewer(this))
    , kdeglobalsConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig()))
    , kwinConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("kwinrc"))))
    , kcmfontsConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("kcmfonts"))))
    , kcminputConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("kcminputrc"))))
    , breezeConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("breezerc"))))
{
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerService(QStringLiteral("org.kde.GtkConfig"));
    dbus.registerObject(QStringLiteral("/GtkConfig"), this, QDBusConnection::ExportScriptableSlots);

    if (qgetenv("GTK_USE_PORTAL") != "1" && KWindowSystem::isPlatformWayland()) {
        m_gsdXsettingsManager = new GSDXSettingsManager(this);
    }

    connect(kdeglobalsConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfig::onKdeglobalsSettingsChange);
    connect(kwinConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfig::onKWinSettingsChange);
    connect(kcmfontsConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfig::onKCMFontsSettingsChange);
    connect(kcminputConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfig::onKCMInputSettingsChange);
    connect(breezeConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfig::onBreezeSettingsChange);

    Gtk2ConfigEditor::removeLegacyStrings();
    applyAllSettings();
}

GtkConfig::~GtkConfig()
{
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.unregisterService(QStringLiteral("org.kde.GtkConfig"));
    dbus.unregisterObject(QStringLiteral("/GtkConfig"));
}

void GtkConfig::setGtkTheme(const QString &themeName) const
{
    Gtk2ConfigEditor::setValue(QStringLiteral("gtk-theme-name"), themeName);
    GSettingsEditor::setValue(QStringLiteral("gtk-theme"), themeName);
    SettingsIniEditor::setValue(QStringLiteral("gtk-theme-name"), themeName);
    XSettingsEditor::setValue(QStringLiteral("Net/ThemeName"), themeName);

    // Window decorations are part of the theme, in case of Breeze we inject custom ones from KWin
    setWindowDecorationsAppearance();
}

QString GtkConfig::gtkTheme() const
{
    return SettingsIniEditor::value(QStringLiteral("gtk-theme-name"));
}

void GtkConfig::showGtkThemePreview(const QString &themeName) const
{
    themePreviewer->showGtk3App(themeName);
}

void GtkConfig::setFont() const
{
    const QString configFontName = configValueProvider->fontName();
    Gtk2ConfigEditor::setValue(QStringLiteral("gtk-font-name"), configFontName);
    GSettingsEditor::setValue(QStringLiteral("font-name"), configFontName);
    SettingsIniEditor::setValue(QStringLiteral("gtk-font-name"), configFontName);
    XSettingsEditor::setValue(QStringLiteral("Gtk/FontName"), configFontName);
}

void GtkConfig::setIconTheme() const
{
    const QString iconThemeName = configValueProvider->iconThemeName();
    Gtk2ConfigEditor::setValue(QStringLiteral("gtk-icon-theme-name"), iconThemeName);
    GSettingsEditor::setValue(QStringLiteral("icon-theme"), iconThemeName);
    SettingsIniEditor::setValue(QStringLiteral("gtk-icon-theme-name"), iconThemeName);
    XSettingsEditor::setValue(QStringLiteral("Net/IconThemeName"), iconThemeName);
}

void GtkConfig::setCursorTheme() const
{
    const QString cursorThemeName = configValueProvider->cursorThemeName();
    Gtk2ConfigEditor::setValue(QStringLiteral("gtk-cursor-theme-name"), cursorThemeName);
    GSettingsEditor::setValue(QStringLiteral("cursor-theme"), cursorThemeName);
    SettingsIniEditor::setValue(QStringLiteral("gtk-cursor-theme-name"), cursorThemeName);
    XSettingsEditor::setValue(QStringLiteral("Gtk/CursorThemeName"), cursorThemeName);
}

void GtkConfig::setCursorSize() const
{
    const int cursorSize = configValueProvider->cursorSize();
    Gtk2ConfigEditor::setValue(QStringLiteral("gtk-cursor-theme-size"), cursorSize);
    GSettingsEditor::setValue(QStringLiteral("cursor-size"), cursorSize);
    SettingsIniEditor::setValue(QStringLiteral("gtk-cursor-theme-size"), cursorSize);
    XSettingsEditor::setValue(QStringLiteral("Gtk/CursorThemeSize"), cursorSize);
}

void GtkConfig::setIconsOnButtons() const
{
    const bool iconsOnButtonsConfigValue = configValueProvider->iconsOnButtons();
    Gtk2ConfigEditor::setValue(QStringLiteral("gtk-button-images"), iconsOnButtonsConfigValue);
    // Deprecated in GTK 4
    SettingsIniEditor::setValue(QStringLiteral("gtk-button-images"), iconsOnButtonsConfigValue, 3);
    XSettingsEditor::setValue(QStringLiteral("Gtk/ButtonImages"), iconsOnButtonsConfigValue);
}

void GtkConfig::setIconsInMenus() const
{
    const bool iconsInMenusConfigValue = configValueProvider->iconsInMenus();
    Gtk2ConfigEditor::setValue(QStringLiteral("gtk-menu-images"), iconsInMenusConfigValue);
    // Deprecated in GTK 4
    SettingsIniEditor::setValue(QStringLiteral("gtk-menu-images"), iconsInMenusConfigValue, 3);
    XSettingsEditor::setValue(QStringLiteral("Gtk/MenuImages"), iconsInMenusConfigValue);
}

void GtkConfig::setToolbarStyle() const
{
    const int toolbarStyle = configValueProvider->toolbarStyle();
    Gtk2ConfigEditor::setValue(QStringLiteral("gtk-toolbar-style"), toolbarStyle);
    GSettingsEditor::setValueAsEnum(QStringLiteral("toolbar-style"), toolbarStyle);
    // Deprecated in GTK 4
    SettingsIniEditor::setValue(QStringLiteral("gtk-toolbar-style"), toolbarStyle, 3);
    XSettingsEditor::setValue(QStringLiteral("Gtk/ToolbarStyle"), toolbarStyle);
}

void GtkConfig::setScrollbarBehavior() const
{
    const bool scrollbarBehavior = configValueProvider->scrollbarBehavior();
    Gtk2ConfigEditor::setValue(QStringLiteral("gtk-primary-button-warps-slider"), scrollbarBehavior);
    SettingsIniEditor::setValue(QStringLiteral("gtk-primary-button-warps-slider"), scrollbarBehavior);
    XSettingsEditor::setValue(QStringLiteral("Gtk/PrimaryButtonWarpsSlider"), scrollbarBehavior);
}

void GtkConfig::setDarkThemePreference() const
{
    const bool preferDarkTheme = configValueProvider->preferDarkTheme();
    SettingsIniEditor::setValue(QStringLiteral("gtk-application-prefer-dark-theme"), preferDarkTheme);
    // https://gitlab.gnome.org/GNOME/gsettings-desktop-schemas/-/blob/master/headers/gdesktop-enums.h
    GSettingsEditor::setValueAsEnum(QStringLiteral("color-scheme"),
                                    preferDarkTheme ? 1 /*G_DESKTOP_COLOR_SCHEME_PREFER_DARK*/ : 0 /*G_DESKTOP_COLOR_SCHEME_DEFAULT*/,
                                    QStringLiteral("org.gnome.desktop.interface"));
}

void GtkConfig::setWindowDecorationsAppearance() const
{
    if (gtkTheme() == QStringLiteral("Breeze")) { // Only Breeze GTK supports custom decoration buttons
        const auto windowDecorationsButtonsImages = configValueProvider->windowDecorationsButtonsImages();
        CustomCssEditor::setCustomClientSideDecorations(windowDecorationsButtonsImages);
    } else {
        CustomCssEditor::disableCustomClientSideDecorations();
    }
}

void GtkConfig::setWindowDecorationsButtonsOrder() const
{
    const QString windowDecorationsButtonOrder = configValueProvider->windowDecorationsButtonsOrder();
    GSettingsEditor::setValue(QStringLiteral("button-layout"), windowDecorationsButtonOrder, QStringLiteral("org.gnome.desktop.wm.preferences"));
    SettingsIniEditor::setValue(QStringLiteral("gtk-decoration-layout"), windowDecorationsButtonOrder);
    XSettingsEditor::setValue(QStringLiteral("Gtk/DecorationLayout"), windowDecorationsButtonOrder);
}

void GtkConfig::setEnableAnimations() const
{
    const bool enableAnimations = configValueProvider->enableAnimations();
    Gtk2ConfigEditor::setValue(QStringLiteral("gtk-enable-animations"), enableAnimations);
    GSettingsEditor::setValue(QStringLiteral("enable-animations"), enableAnimations);
    SettingsIniEditor::setValue(QStringLiteral("gtk-enable-animations"), enableAnimations);
    XSettingsEditor::setValue(QStringLiteral("Gtk/EnableAnimations"), enableAnimations);
    if (m_gsdXsettingsManager) {
        m_gsdXsettingsManager->enableAnimationsChanged();
    }
}

void GtkConfig::setGlobalScale() const
{
    const double scaleFactor = configValueProvider->x11GlobalScaleFactor();
    XSettingsEditor::setValue(QStringLiteral("Gdk/WindowScalingFactor"), int(scaleFactor));
}

void GtkConfig::setTextScale() const
{
    const double x11Scale = configValueProvider->x11GlobalScaleFactor();
    const int x11ScaleIntegerPart = int(x11Scale);

    const int forceFontDpi = configValueProvider->fontDpi();

    int x11TextDpiAbsolute = 96 * 1024;
    double waylandTextScaleFactor = 1.0;

    if (forceFontDpi == 0) {
        x11TextDpiAbsolute = (96 * 1024) * x11Scale;
    } else {
        x11TextDpiAbsolute = (forceFontDpi * 1024);

        if (!KWindowSystem::isPlatformX11()) {
            x11TextDpiAbsolute *= x11Scale;
        }

        waylandTextScaleFactor = double(forceFontDpi) / 96.0;
        waylandTextScaleFactor = std::clamp(waylandTextScaleFactor, 0.5, 3.0);
    }

    XSettingsEditor::unsetValue(QStringLiteral("Xft/DPI"));
    SettingsIniEditor::setValue(QStringLiteral("gtk-xft-dpi"), x11TextDpiAbsolute);
    XSettingsEditor::setValue(QStringLiteral("Gdk/UnscaledDPI"), x11TextDpiAbsolute / x11ScaleIntegerPart);
    GSettingsEditor::setValue(QStringLiteral("text-scaling-factor"), waylandTextScaleFactor);
}

void GtkConfig::setColors() const
{
    const QMap<QString, QColor> colors = configValueProvider->colors();
    CustomCssEditor::setColors(colors);
    if (m_gsdXsettingsManager) {
        m_gsdXsettingsManager->modulesChanged();
    }
}

void GtkConfig::applyAllSettings() const
{
    setFont();
    setIconTheme();
    setCursorTheme();
    setCursorSize();
    setIconsOnButtons();
    setIconsInMenus();
    setToolbarStyle();
    setScrollbarBehavior();
    setDarkThemePreference();
    setWindowDecorationsAppearance();
    setWindowDecorationsButtonsOrder();
    setEnableAnimations();
    setGlobalScale();
    setTextScale();
    setColors();
}

void GtkConfig::onKdeglobalsSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const
{
    if (group.name() == QStringLiteral("KDE")) {
        if (names.contains(QByteArrayLiteral("AnimationDurationFactor"))) {
            setEnableAnimations();
        }
        if (names.contains(QByteArrayLiteral("ShowIconsInMenuItems"))) {
            setIconsInMenus();
        }
        if (names.contains(QByteArrayLiteral("ShowIconsOnPushButtons"))) {
            setIconsOnButtons();
        }
        if (names.contains(QByteArrayLiteral("ScrollbarLeftClickNavigatesByPage"))) {
            setScrollbarBehavior();
        }
    } else if (group.name() == QStringLiteral("Icons")) {
        if (names.contains(QByteArrayLiteral("Theme"))) {
            setIconTheme();
        }
    } else if (group.name() == QStringLiteral("General")) {
        if (names.contains(QByteArrayLiteral("font"))) {
            setFont();
        }
        if (names.contains(QByteArrayLiteral("ColorScheme")) || names.contains(QByteArrayLiteral("AccentColor"))) {
            setColors();
            setDarkThemePreference();
            setWindowDecorationsAppearance(); // Decorations' color can depend on the current color scheme
        }
    } else if (group.name() == QStringLiteral("KScreen")) {
        if (names.contains(QByteArrayLiteral("ScaleFactor"))) {
            setGlobalScale();
            // setTextScale() will be called in onKCMFontsSettingsChange
        }
    } else if (group.name() == QStringLiteral("Toolbar style")) {
        if (names.contains(QByteArrayLiteral("ToolButtonStyle"))) {
            setToolbarStyle();
        }
    }
}

void GtkConfig::onKWinSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const
{
    if (group.name() == QStringLiteral("org.kde.kdecoration2")) {
        if (names.contains(QByteArrayLiteral("ButtonsOnRight")) //
            || names.contains(QByteArrayLiteral("ButtonsOnLeft"))) {
            setWindowDecorationsButtonsOrder();
        }
        if (names.contains(QByteArrayLiteral("theme"))) {
            setWindowDecorationsAppearance();
        }
    } else if (group.name() == QStringLiteral("Xwayland")) {
        if (names.contains(QByteArrayLiteral("Scale"))) {
            setGlobalScale();
            setTextScale();
        }
    }
}

void GtkConfig::onKCMFontsSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const
{
    if (group.name() == QStringLiteral("General")) {
        if (names.contains("forceFontDPI") || names.contains("forceFontDPIWayland")) {
            setTextScale();
        }
    }
}

void GtkConfig::onKCMInputSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const
{
    if (group.name() == QStringLiteral("Mouse")) {
        if (names.contains("cursorTheme")) {
            setCursorTheme();
        }
        if (names.contains("cursorSize")) {
            setCursorSize();
        }
    }
}

void GtkConfig::onBreezeSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const
{
    if (group.name() == QStringLiteral("Common") //
        && names.contains("OutlineCloseButton")) {
        setWindowDecorationsAppearance();
    }
}

#include "gtkconfig.moc"
