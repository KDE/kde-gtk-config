/*
 * SPDX-FileCopyrightText: 2026 David Edmundson <kde@davidedmundson.co.uk>
 * SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

/*
 * XSettings-focused kded plugin. Contains only XSettings and xsettingsd-related actions.
 */

#include "gtkconfig-xsettings.h"

#include <KPluginFactory>

#include <QDBusConnection>

#include "config_editor/settings_ini.h"
#include "config_editor/utils.h"
#include "config_editor/xsettings.h"

K_PLUGIN_CLASS_WITH_JSON(GtkConfigXSettings, "gtkconfig-xsettings.json")

GtkConfigXSettings::GtkConfigXSettings(QObject *parent, const QVariantList &)
    : KDEDModule(parent)
    , configValueProvider(new ConfigValueProvider())
    , kdeglobalsConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig()))
    , kwinConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("kwinrc"))))
    , kcminputConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("kcminputrc"))))
{
    QDBusConnection::sessionBus().connect(QStringLiteral("org.kde.GtkConfig"),
                                          QStringLiteral("/GtkConfig"),
                                          QStringLiteral("org.kde.GtkConfig"),
                                          QStringLiteral("gtk2ThemeChanged"),
                                          this,
                                          SLOT(onGtk2ThemeChanged(QString)));

    connect(kdeglobalsConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfigXSettings::onKdeglobalsSettingsChange);
    connect(kwinConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfigXSettings::onKWinSettingsChange);
    connect(kcminputConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfigXSettings::onKCMInputSettingsChange);
    applyAllSettings();
}

void GtkConfigXSettings::setGtk2Theme(const QString &themeName, const bool preferDarkTheme) const
{
    XSettingsEditor::setValue(QStringLiteral("Net/ThemeName"), Utils::gtk2ThemeName(themeName, preferDarkTheme));
}

void GtkConfigXSettings::setFont() const
{
    const QString configFontName = configValueProvider->fontName(false);
    XSettingsEditor::setValue(QStringLiteral("Gtk/FontName"), configFontName);
}

void GtkConfigXSettings::setIconTheme() const
{
    const QString iconThemeName = configValueProvider->iconThemeName();
    XSettingsEditor::setValue(QStringLiteral("Net/IconThemeName"), iconThemeName);
}

void GtkConfigXSettings::setSoundTheme() const
{
    const QString soundThemeName = configValueProvider->soundThemeName();
    XSettingsEditor::setValue(QStringLiteral("Net/SoundThemeName"), soundThemeName);
}

void GtkConfigXSettings::setEventSoundsEnabled() const
{
    const bool soundsEnabled = configValueProvider->eventSoundsEnabled();
    XSettingsEditor::setValue(QStringLiteral("Net/EnableEventSounds"), soundsEnabled);
}

void GtkConfigXSettings::setCursorTheme() const
{
    const QString cursorThemeName = configValueProvider->cursorThemeName();
    XSettingsEditor::setValue(QStringLiteral("Gtk/CursorThemeName"), cursorThemeName);
}

void GtkConfigXSettings::setCursorSize() const
{
    // Not KWindowSystem: this module can run on xcb inside a Wayland session
    qreal xwaylandScale = 1.0;
    if (qgetenv("XDG_SESSION_TYPE") != "x11") {
        xwaylandScale = configValueProvider->x11GlobalScaleFactor();
    }

    const int cursorSize = configValueProvider->cursorSize();
    XSettingsEditor::setValue(QStringLiteral("Gtk/CursorThemeSize"), int(cursorSize * xwaylandScale));
}

void GtkConfigXSettings::setIconsOnButtons() const
{
    const bool iconsOnButtonsConfigValue = configValueProvider->iconsOnButtons();
    XSettingsEditor::setValue(QStringLiteral("Gtk/ButtonImages"), iconsOnButtonsConfigValue);
}

void GtkConfigXSettings::setIconsInMenus() const
{
    const bool iconsInMenusConfigValue = configValueProvider->iconsInMenus();
    XSettingsEditor::setValue(QStringLiteral("Gtk/MenuImages"), iconsInMenusConfigValue);
}

void GtkConfigXSettings::setToolbarStyle() const
{
    const int toolbarStyle = configValueProvider->toolbarStyle();
    XSettingsEditor::setValue(QStringLiteral("Gtk/ToolbarStyle"), toolbarStyle);
}

void GtkConfigXSettings::setScrollbarBehavior() const
{
    const bool scrollbarBehavior = configValueProvider->scrollbarBehavior();
    XSettingsEditor::setValue(QStringLiteral("Gtk/PrimaryButtonWarpsSlider"), scrollbarBehavior);
}

void GtkConfigXSettings::setDoubleClickInterval() const
{
    const int doubleClickInterval = configValueProvider->doubleClickInterval();
    XSettingsEditor::setValue(QStringLiteral("Net/DoubleClickTime"), doubleClickInterval);
}

void GtkConfigXSettings::setCursorBlinkRate() const
{
    const bool cursorBlinkEnabled = configValueProvider->cursorBlinkRate() > 0;
    int cursorBlinkRate = qBound(100, configValueProvider->cursorBlinkRate(), 2500);
    if (!cursorBlinkEnabled) {
        cursorBlinkRate = 1000;
    }

    XSettingsEditor::setValue(QStringLiteral("Net/CursorBlink"), cursorBlinkEnabled);
    XSettingsEditor::setValue(QStringLiteral("Net/CursorBlinkTime"), cursorBlinkRate);
}

void GtkConfigXSettings::setWindowDecorationsButtonsOrder() const
{
    const QString windowDecorationsButtonOrder = configValueProvider->windowDecorationsButtonsOrder();
    XSettingsEditor::setValue(QStringLiteral("Gtk/DecorationLayout"), windowDecorationsButtonOrder);
}

void GtkConfigXSettings::setEnableAnimations() const
{
    const bool enableAnimations = configValueProvider->enableAnimations();
    XSettingsEditor::setValue(QStringLiteral("Gtk/EnableAnimations"), enableAnimations);
}

void GtkConfigXSettings::setGlobalScale() const
{
    const unsigned scaleFactor = configValueProvider->x11GlobalScaleFactor();
    XSettingsEditor::setValue(QStringLiteral("Gdk/WindowScalingFactor"), scaleFactor);
}

void GtkConfigXSettings::setTextScale() const
{
    const double x11Scale = configValueProvider->x11GlobalScaleFactor();
    const int x11ScaleIntegerPart = int(x11Scale);

    int x11TextDpiAbsolute = 96 * 1024 * x11Scale;

    XSettingsEditor::unsetValue(QStringLiteral("Xft/DPI"));
    XSettingsEditor::setValue(QStringLiteral("Gdk/UnscaledDPI"), x11TextDpiAbsolute / x11ScaleIntegerPart);
}

void GtkConfigXSettings::applyAllSettings() const
{
    setGtk2Theme(SettingsIniEditor::value(QStringLiteral("gtk-theme-name")), configValueProvider->preferDarkTheme());
    setFont();
    setIconTheme();
    setSoundTheme();
    setEventSoundsEnabled();
    setCursorTheme();
    setCursorSize();
    setIconsOnButtons();
    setIconsInMenus();
    setToolbarStyle();
    setScrollbarBehavior();
    setDoubleClickInterval();
    setCursorBlinkRate();
    setWindowDecorationsButtonsOrder();
    setEnableAnimations();
    setGlobalScale();
    setTextScale();
}

void GtkConfigXSettings::onGtk2ThemeChanged(const QString &themeName) const
{
    XSettingsEditor::setValue(QStringLiteral("Net/ThemeName"), themeName);
}

void GtkConfigXSettings::onKdeglobalsSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const
{
    if (group.name() == QStringLiteral("KDE")) {
        if (names.contains(QByteArrayLiteral("AnimationDurationFactor"))) {
            setEnableAnimations();
        }
        if (names.contains(QByteArrayLiteral("LookAndFeelPackage"))) {
            applyAllSettings();
        }
        if (names.contains(QByteArrayLiteral("ShowIconsInMenuItems"))) {
            setIconsInMenus();
        }
        if (names.contains(QByteArrayLiteral("ShowIconsOnPushButtons"))) {
            setIconsOnButtons();
        }
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
        if (names.contains(QByteArrayLiteral("font"))) {
            setFont();
        }
    } else if (group.name() == QStringLiteral("Toolbar style")) {
        if (names.contains(QByteArrayLiteral("ToolButtonStyle"))) {
            setToolbarStyle();
        }
    }
}

void GtkConfigXSettings::onKWinSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const
{
    if (group.name() == QStringLiteral("org.kde.kdecoration2")) {
        if (names.contains(QByteArrayLiteral("ButtonsOnRight")) || names.contains(QByteArrayLiteral("ButtonsOnLeft"))) {
            setWindowDecorationsButtonsOrder();
        }
    } else if (group.name() == QStringLiteral("Xwayland")) {
        if (names.contains(QByteArrayLiteral("Scale"))) {
            setGlobalScale();
            setTextScale();
            setCursorSize();
        }
    }
}

void GtkConfigXSettings::onKCMInputSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const
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

#include "gtkconfig-xsettings.moc"
