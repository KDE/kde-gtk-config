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

#include <algorithm>
#include <cmath>

#include "config_editor/custom_css.h"
#include "config_editor/gsettings.h"
#include "config_editor/gtk2.h"
#include "config_editor/settings_ini.h"
#include "config_editor/xsettings.h"
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

    connect(kdeglobalsConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfig::onKdeglobalsSettingsChange);
    connect(kwinConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfig::onKWinSettingsChange);
    connect(kcminputConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfig::onKCMInputSettingsChange);
    connect(breezeConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfig::onBreezeSettingsChange);

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

}

void GtkConfig::setGtk2Theme(const QString &themeName, const bool preferDarkTheme)
{
    // GTK2 does not support using dark variant automatically, so we have to find a dark theme folder variant if available.
    QString possiblyDarkThemeName = themeName;
    if (preferDarkTheme) {
        const QString darkVariant = findGtk2DarkThemeVariant(themeName);
        if (!darkVariant.isEmpty()) {
            possiblyDarkThemeName = darkVariant;
        }
    }

    m_gtk2.set(QStringLiteral("gtk-theme-name"), possiblyDarkThemeName);
    m_xsettings.set(QStringLiteral("Net/ThemeName"), possiblyDarkThemeName);
}

void GtkConfig::setGtkTheme(const QString &themeName)
{
    setGtk2Theme(themeName, configValueProvider->preferDarkTheme());
    m_gsettings.set(u"gtk-theme"_s, themeName);
    m_settingsIni.set(QStringLiteral("gtk-theme-name"), themeName);

    // Window decorations are part of the theme, in case of Breeze we inject custom ones from KWin
    setWindowDecorationsAppearance();

    syncBackends();
}

void GtkConfig::addGtkModule(const QString &moduleName)
{
    const QString currentModules = SettingsIniBackend::value(u"gtk-modules"_s, 3);
    if (currentModules.contains(moduleName)) {
        return;
    }
    m_settingsIni3.set(u"gtk-modules"_s, currentModules.isEmpty() ? moduleName : currentModules + u':' + moduleName);
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

QString GtkConfig::gtkTheme() const
{
    return SettingsIniBackend::value(QStringLiteral("gtk-theme-name"), 3);
}

void GtkConfig::showGtkThemePreview(const QString &themeName) const
{
    const bool darkTheme = configValueProvider->preferDarkTheme();
    const auto themeVariant = darkTheme ? QStringLiteral(":dark") : QStringLiteral(":light");
    themePreviewer->showGtk3App(themeName + themeVariant);
}

void GtkConfig::setFixed()
{
    const QString configfixedName = configValueProvider->fixedName(true);
    m_gsettings.set(u"monospace-font-name"_s, configfixedName);
}

void GtkConfig::setFont()
{
    const QString configFontName = configValueProvider->fontName(false);
    const QString configFontNameGsettings = configValueProvider->fontName(true);
    m_gtk2.set(QStringLiteral("gtk-font-name"), configFontName);
    m_gsettings.set(u"document-font-name"_s, configFontNameGsettings);
    m_gsettings.set(u"font-name"_s, configFontNameGsettings);
    m_settingsIni.set(QStringLiteral("gtk-font-name"), configFontName);
    m_xsettings.set(QStringLiteral("Gtk/FontName"), configFontName);
}

void GtkConfig::setIconTheme()
{
    const QString iconThemeName = configValueProvider->iconThemeName();
    m_gtk2.set(QStringLiteral("gtk-icon-theme-name"), iconThemeName);
    m_gsettings.set(u"icon-theme"_s, iconThemeName);
    m_settingsIni.set(QStringLiteral("gtk-icon-theme-name"), iconThemeName);
    m_xsettings.set(QStringLiteral("Net/IconThemeName"), iconThemeName);
}

void GtkConfig::setSoundTheme()
{
    const QString soundThemeName = configValueProvider->soundThemeName();
    m_gtk2.set(QStringLiteral("gtk-sound-theme-name"), soundThemeName);
    m_gsettingsSound.set(u"theme-name"_s, soundThemeName);
    m_settingsIni.set(QStringLiteral("gtk-sound-theme-name"), soundThemeName);
    m_xsettings.set(QStringLiteral("Net/SoundThemeName"), soundThemeName);
}

void GtkConfig::setEventSoundsEnabled()
{
    const bool soundsEnabled = configValueProvider->eventSoundsEnabled();
    m_gtk2.set(QStringLiteral("gtk-enable-event-sounds"), soundsEnabled);
    m_gsettingsSound.set(u"event-sounds"_s, soundsEnabled);
    m_settingsIni.set(QStringLiteral("gtk-enable-event-sounds"), soundsEnabled);
    m_xsettings.set(QStringLiteral("Net/EnableEventSounds"), soundsEnabled);
}

void GtkConfig::setCursorTheme()
{
    const QString cursorThemeName = configValueProvider->cursorThemeName();
    m_gtk2.set(QStringLiteral("gtk-cursor-theme-name"), cursorThemeName);
    m_gsettings.set(u"cursor-theme"_s, cursorThemeName);
    m_settingsIni.set(QStringLiteral("gtk-cursor-theme-name"), cursorThemeName);
    m_xsettings.set(QStringLiteral("Gtk/CursorThemeName"), cursorThemeName);
}

void GtkConfig::setCursorSize()
{
    qreal xwaylandScale = 1.0;
    if (KWindowSystem::isPlatformWayland()) {
        xwaylandScale = configValueProvider->x11GlobalScaleFactor();
    }

    const int cursorSize = configValueProvider->cursorSize();
    m_gtk2.set(QStringLiteral("gtk-cursor-theme-size"), cursorSize);
    m_gsettings.set(u"cursor-size"_s, cursorSize);
    m_settingsIni.set(QStringLiteral("gtk-cursor-theme-size"), cursorSize);
    m_xsettings.set(QStringLiteral("Gtk/CursorThemeSize"), int(cursorSize * xwaylandScale));
}

void GtkConfig::setIconsOnButtons()
{
    const bool iconsOnButtonsConfigValue = configValueProvider->iconsOnButtons();
    m_gtk2.set(QStringLiteral("gtk-button-images"), iconsOnButtonsConfigValue);
    // Deprecated in GTK 4
    m_settingsIni3.set(QStringLiteral("gtk-button-images"), iconsOnButtonsConfigValue);
    m_xsettings.set(QStringLiteral("Gtk/ButtonImages"), iconsOnButtonsConfigValue);
}

void GtkConfig::setIconsInMenus()
{
    const bool iconsInMenusConfigValue = configValueProvider->iconsInMenus();
    m_gtk2.set(QStringLiteral("gtk-menu-images"), iconsInMenusConfigValue);
    // Deprecated in GTK 4
    m_settingsIni3.set(QStringLiteral("gtk-menu-images"), iconsInMenusConfigValue);
    m_xsettings.set(QStringLiteral("Gtk/MenuImages"), iconsInMenusConfigValue);
}

void GtkConfig::setToolbarStyle()
{
    const int toolbarStyle = configValueProvider->toolbarStyle();
    m_gtk2.set(QStringLiteral("gtk-toolbar-style"), toolbarStyle);
    m_gsettings.set(u"toolbar-style"_s, QVariant::fromValue(GSettingsEnum{toolbarStyle}));
    // Deprecated in GTK 4
    m_settingsIni3.set(QStringLiteral("gtk-toolbar-style"), toolbarStyle);
    m_xsettings.set(QStringLiteral("Gtk/ToolbarStyle"), toolbarStyle);
}

void GtkConfig::setScrollbarBehavior()
{
    const bool scrollbarBehavior = configValueProvider->scrollbarBehavior();
    m_gtk2.set(QStringLiteral("gtk-primary-button-warps-slider"), scrollbarBehavior);
    m_settingsIni.set(QStringLiteral("gtk-primary-button-warps-slider"), scrollbarBehavior);
    m_xsettings.set(QStringLiteral("Gtk/PrimaryButtonWarpsSlider"), scrollbarBehavior);
}

void GtkConfig::setDoubleClickInterval()
{
    const int doubleClickInterval = configValueProvider->doubleClickInterval();
    m_gtk2.set(QStringLiteral("gtk-double-click-time"), doubleClickInterval);
    m_gsettingsMouse.set(u"double-click"_s, doubleClickInterval);
    m_settingsIni.set(QStringLiteral("gtk-double-click-time"), doubleClickInterval);
    m_xsettings.set(QStringLiteral("Net/DoubleClickTime"), doubleClickInterval);
}

void GtkConfig::setCursorBlinkRate()
{
    const bool cursorBlinkEnabled = configValueProvider->cursorBlinkRate() > 0;
    // Range for cusor-blink-time in GSettings.
    int cursorBlinkRate = qBound(100, configValueProvider->cursorBlinkRate(), 2500);
    // Qt's GTK 3 platform theme didn't honor blink enabled properly before,
    // set a sane default value when it's off to keep it from blinking like crazy.
    if (!cursorBlinkEnabled) {
        cursorBlinkRate = 1000;
    }

    m_gtk2.set(QStringLiteral("gtk-cursor-blink"), cursorBlinkEnabled);
    m_gtk2.set(QStringLiteral("gtk-cursor-blink-time"), cursorBlinkRate);
    m_gsettings.set(u"cursor-blink"_s, cursorBlinkEnabled);
    m_gsettings.set(u"cursor-blink-time"_s, cursorBlinkRate);
    m_settingsIni.set(QStringLiteral("gtk-cursor-blink"), cursorBlinkEnabled);
    m_settingsIni.set(QStringLiteral("gtk-cursor-blink-time"), cursorBlinkRate);
    m_xsettings.set(QStringLiteral("Net/CursorBlink"), cursorBlinkEnabled);
    m_xsettings.set(QStringLiteral("Net/CursorBlinkTime"), cursorBlinkRate);
}

void GtkConfig::setDarkThemePreference()
{
    const bool preferDarkTheme = configValueProvider->preferDarkTheme();
    m_settingsIni.set(QStringLiteral("gtk-application-prefer-dark-theme"), preferDarkTheme);
    // https://gitlab.gnome.org/GNOME/gsettings-desktop-schemas/-/blob/master/headers/gdesktop-enums.h
    m_gsettings.set(u"color-scheme"_s,
                    QVariant::fromValue(GSettingsEnum{preferDarkTheme ? 1 /*G_DESKTOP_COLOR_SCHEME_PREFER_DARK*/ : 2 /*G_DESKTOP_COLOR_SCHEME_PREFER_LIGHT*/}));
    setGtk2Theme(gtkTheme(), preferDarkTheme);
}

void GtkConfig::setWindowDecorationsAppearance()
{
    if (gtkTheme() == QStringLiteral("Breeze")) { // Only Breeze GTK supports custom decoration buttons
        addGtkModule(u"window-decorations-gtk-module"_s);
        CustomCssEditor::setCustomClientSideDecorations(configValueProvider->windowDecorationsButtonsImages());
    } else {
        CustomCssEditor::disableCustomClientSideDecorations();
    }
}

void GtkConfig::setWindowDecorationsButtonsOrder()
{
    const QString windowDecorationsButtonOrder = configValueProvider->windowDecorationsButtonsOrder();
    m_gsettingsWm.set(u"button-layout"_s, windowDecorationsButtonOrder);
    m_settingsIni.set(QStringLiteral("gtk-decoration-layout"), windowDecorationsButtonOrder);
    m_xsettings.set(QStringLiteral("Gtk/DecorationLayout"), windowDecorationsButtonOrder);
}

void GtkConfig::setEnableAnimations()
{
    const bool enableAnimations = configValueProvider->enableAnimations();
    m_gtk2.set(QStringLiteral("gtk-enable-animations"), enableAnimations);
    m_gsettings.set(u"enable-animations"_s, enableAnimations);
    m_settingsIni.set(QStringLiteral("gtk-enable-animations"), enableAnimations);
    m_xsettings.set(QStringLiteral("Gtk/EnableAnimations"), enableAnimations);
    if (m_gsdXsettingsManager) {
        m_gsdXsettingsManager->enableAnimationsChanged();
    }
}

void GtkConfig::setGlobalScale()
{
    const unsigned scaleFactor = configValueProvider->x11GlobalScaleFactor();
    m_xsettings.set(QStringLiteral("Gdk/WindowScalingFactor"), scaleFactor);
    m_gsettings.set(u"scaling-factor"_s, scaleFactor); // For IntelliJ IDEA
}

void GtkConfig::setTextScale()
{
    const double x11Scale = configValueProvider->x11GlobalScaleFactor();
    const int x11ScaleIntegerPart = int(x11Scale);

    int x11TextDpiAbsolute = 96 * 1024 * x11Scale;
    double waylandTextScaleFactor = 1.0;

    m_xsettings.unset(QStringLiteral("Xft/DPI"));
    m_settingsIni.set(QStringLiteral("gtk-xft-dpi"), x11TextDpiAbsolute);
    m_xsettings.set(QStringLiteral("Gdk/UnscaledDPI"), x11TextDpiAbsolute / x11ScaleIntegerPart);
    m_gsettings.set(u"text-scaling-factor"_s, waylandTextScaleFactor);
}

void GtkConfig::setColors()
{
    addGtkModule(QStringLiteral("colorreload-gtk-module"));
    if (m_gsdXsettingsManager) {
        m_gsdXsettingsManager->modulesChanged();
    }
    // modulesChanged signal will take some time to reach a GTK app, so explicitly wait a moment
    QTimer::singleShot(200, this, [this] {
        CustomCssEditor::setColors(configValueProvider->colors());
    });
}

void GtkConfig::applyAllSettings()
{
    setFixed();
    setFont();
    setIconTheme();
    setSoundTheme();
    setCursorTheme();
    setCursorSize();
    setCursorBlinkRate();
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

    syncBackends();
}

void GtkConfig::onKdeglobalsSettingsChange(const KConfigGroup &group, const QByteArrayList &names)
{
    if (group.name() == QStringLiteral("KDE")) {
        if (names.contains(QByteArrayLiteral("AnimationDurationFactor"))) {
            setEnableAnimations();
        }
        if (names.contains(QByteArrayLiteral("LookAndFeelPackage"))) {
            // update all settings, since global theme can include most of them
            applyAllSettings();
        }
        if (names.contains(QByteArrayLiteral("ShowIconsInMenuItems"))) {
            setIconsInMenus();
        }
        if (names.contains(QByteArrayLiteral("ShowIconsOnPushButtons"))) {
            setIconsOnButtons();
        }
        // ScrollbarLeftClickNavigatesByPage is now the default setting, so when it's
        // true, it won't be present, so we need to check for its absence
        if (!names.contains(QByteArrayLiteral("ScrollbarLeftClickNavigatesByPage"))) {
            setScrollbarBehavior();
        }
        if (names.contains(QByteArrayLiteral("DoubleClickInterval"))) {
            setDoubleClickInterval();
        }
        if (names.contains(QByteArrayLiteral("CursorBlinkRate"))) {
            setCursorBlinkRate();
        }
    } else if (group.name() == QStringLiteral("Icons")) {
        if (names.contains(QByteArrayLiteral("Theme"))) {
            setIconTheme();
        }
    } else if (group.name() == QLatin1String("Sounds")) {
        if (names.contains(QByteArrayLiteral("Theme"))) {
            setSoundTheme();
        }
        if (names.contains(QByteArrayLiteral("Enable"))) {
            setEventSoundsEnabled();
        }
    } else if (group.name() == QStringLiteral("General")) {
        if (names.contains(QByteArrayLiteral("fixed"))) {
            setFixed();
        }
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
    } else if (group.name() == QStringLiteral("KDE")) {
        if (names.contains(QByteArrayLiteral("frameContrast"))) {
            setColors();
        }
    }

    syncBackends();
}

void GtkConfig::onKWinSettingsChange(const KConfigGroup &group, const QByteArrayList &names)
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
            setCursorSize();
        }
    }

    syncBackends();
}

void GtkConfig::onKCMInputSettingsChange(const KConfigGroup &group, const QByteArrayList &names)
{
    if (group.name() == QStringLiteral("Mouse")) {
        if (names.contains("cursorTheme")) {
            setCursorTheme();
        }
        if (names.contains("cursorSize")) {
            setCursorSize();
        }
    }

    syncBackends();
}

void GtkConfig::onBreezeSettingsChange(const KConfigGroup &group, const QByteArrayList &names)
{
    if (group.name() == QStringLiteral("Common") //
        && names.contains("OutlineCloseButton")) {
        setWindowDecorationsAppearance();
    }

    syncBackends();
}

#include "gtkconfig.moc"

#include "moc_gtkconfig.cpp"
