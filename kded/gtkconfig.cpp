/*
 * Copyright (C) 2019 Mikhail Zolotukhin <zomial@protonmail.com>
 * Copyright (C) 2019 Nicolas Fella <nicolas.fella@gmx.de>
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

#include <QFont>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QGuiApplication>

#include <KPluginFactory>
#include <KConfigWatcher>
#include <KColorScheme>

#include "gtkconfig.h"
#include "configvalueprovider.h"
#include "themepreviewer.h"

K_PLUGIN_CLASS_WITH_JSON(GtkConfig, "gtkconfig.json")

GtkConfig::GtkConfig(QObject *parent, const QVariantList&) :
    KDEDModule(parent),
    configValueProvider(new ConfigValueProvider()),
    themePreviewer(new ThemePreviewer(this)),
    kdeglobalsConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("kdeglobals")))),
    kwinConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("kwinrc")))),
    kcminputConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("kcminputrc"))))
{
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerService(QStringLiteral("org.kde.GtkConfig"));
    dbus.registerObject(QStringLiteral("/GtkConfig"), this, QDBusConnection::ExportScriptableSlots);

    connect(kdeglobalsConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfig::onKdeglobalsSettingsChange);
    connect(kwinConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfig::onKWinSettingsChange);
    connect(kcminputConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfig::onKCMInputSettingsChange);

    ConfigEditor::removeLegacyGtk2Strings();
    applyAllSettings();
}

GtkConfig::~GtkConfig()
{
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.unregisterService(QStringLiteral("org.kde.GtkConfig"));
    dbus.unregisterObject(QStringLiteral("/GtkConfig"));
}

void GtkConfig::setGtk2Theme(const QString &themeName) const
{
    ConfigEditor::setGtk2ConfigValue(QStringLiteral("gtk-theme-name"), themeName);
}

void GtkConfig::setGtk3Theme(const QString &themeName) const
{
    ConfigEditor::setGtk3ConfigValueGSettings(QStringLiteral("gtk-theme"), themeName);
    ConfigEditor::setGtk3ConfigValueSettingsIni(QStringLiteral("gtk-theme-name"), themeName);
    ConfigEditor::setGtk3ConfigValueXSettingsd(QStringLiteral("Net/ThemeName"),  themeName);
}

QString GtkConfig::gtk2Theme() const
{
    return ConfigEditor::gtk2ConfigValue(QStringLiteral("gtk-theme-name"));
}

QString GtkConfig::gtk3Theme() const
{
    return ConfigEditor::gtk3ConfigValueSettingsIni(QStringLiteral("gtk-theme-name"));
}

void GtkConfig::showGtk2ThemePreview(const QString& themeName) const
{
    themePreviewer->showGtk2App(themeName);
}

void GtkConfig::showGtk3ThemePreview(const QString& themeName) const
{
    themePreviewer->showGtk3App(themeName);
}

void GtkConfig::setFont() const
{
    const QString configFontName = configValueProvider->fontName();
    ConfigEditor::setGtk2ConfigValue(QStringLiteral("gtk-font-name"), configFontName);
    ConfigEditor::setGtk3ConfigValueGSettings(QStringLiteral("font-name"), configFontName);
    ConfigEditor::setGtk3ConfigValueSettingsIni(QStringLiteral("gtk-font-name"), configFontName);
    ConfigEditor::setGtk3ConfigValueXSettingsd(QStringLiteral("Gtk/FontName"),  configFontName);
}

void GtkConfig::setIconTheme() const
{
    const QString iconThemeName = configValueProvider->iconThemeName();
    ConfigEditor::setGtk2ConfigValue(QStringLiteral("gtk-icon-theme-name"), iconThemeName);
    ConfigEditor::setGtk3ConfigValueGSettings(QStringLiteral("icon-theme"), iconThemeName);
    ConfigEditor::setGtk3ConfigValueSettingsIni(QStringLiteral("gtk-icon-theme-name"), iconThemeName);
    ConfigEditor::setGtk3ConfigValueXSettingsd(QStringLiteral("Net/IconThemeName"),  iconThemeName);
}

void GtkConfig::setCursorTheme() const
{
    const QString cursorThemeName = configValueProvider->cursorThemeName();
    ConfigEditor::setGtk2ConfigValue(QStringLiteral("gtk-cursor-theme-name"), cursorThemeName);
    ConfigEditor::setGtk3ConfigValueGSettings(QStringLiteral("cursor-theme"), cursorThemeName);
    ConfigEditor::setGtk3ConfigValueSettingsIni(QStringLiteral("gtk-cursor-theme-name"), cursorThemeName);
    ConfigEditor::setGtk3ConfigValueXSettingsd(QStringLiteral("Gtk/CursorThemeName"),  cursorThemeName);
}

void GtkConfig::setIconsOnButtons() const
{
    const bool iconsOnButtonsConfigValue = configValueProvider->iconsOnButtons();
    ConfigEditor::setGtk2ConfigValue(QStringLiteral("gtk-button-images"), iconsOnButtonsConfigValue);
    ConfigEditor::setGtk3ConfigValueSettingsIni(QStringLiteral("gtk-button-images"), iconsOnButtonsConfigValue);
    ConfigEditor::setGtk3ConfigValueXSettingsd(QStringLiteral("Gtk/ButtonImages"), iconsOnButtonsConfigValue);
}

void GtkConfig::setIconsInMenus() const
{
    const bool iconsInMenusConfigValue = configValueProvider->iconsInMenus();
    ConfigEditor::setGtk2ConfigValue(QStringLiteral("gtk-menu-images"), iconsInMenusConfigValue);
    ConfigEditor::setGtk3ConfigValueSettingsIni(QStringLiteral("gtk-menu-images"), iconsInMenusConfigValue);
    ConfigEditor::setGtk3ConfigValueXSettingsd(QStringLiteral("Gtk/MenuImages"), iconsInMenusConfigValue);
}

void GtkConfig::setToolbarStyle() const
{
    const int toolbarStyle = configValueProvider->toolbarStyle();
    ConfigEditor::setGtk2ConfigValue(QStringLiteral("gtk-toolbar-style"), toolbarStyle);
    ConfigEditor::setGtk3ConfigValueGSettingsAsEnum(QStringLiteral("toolbar-style"), toolbarStyle);
    ConfigEditor::setGtk3ConfigValueSettingsIni(QStringLiteral("gtk-toolbar-style"), toolbarStyle);
    ConfigEditor::setGtk3ConfigValueXSettingsd(QStringLiteral("Gtk/ToolbarStyle"), toolbarStyle);
}

void GtkConfig::setScrollbarBehavior() const
{
    const bool scrollbarBehavior = configValueProvider->scrollbarBehavior();
    ConfigEditor::setGtk2ConfigValue(QStringLiteral("gtk-primary-button-warps-slider"), scrollbarBehavior);
    ConfigEditor::setGtk3ConfigValueSettingsIni(QStringLiteral("gtk-primary-button-warps-slider"), scrollbarBehavior);
    ConfigEditor::setGtk3ConfigValueXSettingsd(QStringLiteral("Gtk/PrimaryButtonWarpsSlider"), scrollbarBehavior);
}

void GtkConfig::setDarkThemePreference() const
{
    const bool preferDarkTheme = configValueProvider->preferDarkTheme();
    ConfigEditor::setGtk3ConfigValueSettingsIni(QStringLiteral("gtk-application-prefer-dark-theme"), preferDarkTheme);
}

void GtkConfig::setWindowDecorationsButtonsOrder() const
{
    const QString windowDecorationsButtonOrder = configValueProvider->windowDecorationsButtonsOrder();
    ConfigEditor::setGtk3ConfigValueGSettings(QStringLiteral("button-layout"), windowDecorationsButtonOrder, QStringLiteral("org.gnome.desktop.wm.preferences"));
    ConfigEditor::setGtk3ConfigValueSettingsIni(QStringLiteral("gtk-decoration-layout"), windowDecorationsButtonOrder);
    ConfigEditor::setGtk3ConfigValueXSettingsd(QStringLiteral("Gtk/DecorationLayout"), windowDecorationsButtonOrder);
}

void GtkConfig::setEnableAnimations() const
{
    const bool enableAnimations = configValueProvider->enableAnimations();
    ConfigEditor::setGtk2ConfigValue(QStringLiteral("gtk-enable-animations"), enableAnimations);
    ConfigEditor::setGtk3ConfigValueGSettings(QStringLiteral("enable-animations"), enableAnimations);
    ConfigEditor::setGtk3ConfigValueSettingsIni(QStringLiteral("gtk-enable-animations"), enableAnimations);
    ConfigEditor::setGtk3ConfigValueXSettingsd(QStringLiteral("Gtk/EnableAnimations"), enableAnimations);
}

void GtkConfig::setColors() const
{
    const QMap<QString, QColor> colors = configValueProvider->colors();
    ConfigEditor::setGtk3Colors(colors);
}

void GtkConfig::applyAllSettings() const
{
    setFont();
    setIconTheme();
    setCursorTheme();
    setIconsOnButtons();
    setIconsInMenus();
    setToolbarStyle();
    setScrollbarBehavior();
    setDarkThemePreference();
    setWindowDecorationsButtonsOrder();
    setEnableAnimations();
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
        if (names.contains(QByteArrayLiteral("ColorScheme"))) {
            setColors();
            setDarkThemePreference();
        }
    } else if (group.name() == QStringLiteral("Toolbar style")) {
        if (names.contains(QByteArrayLiteral("ToolButtonStyle"))) {
            setToolbarStyle();
        }
    }
}

void GtkConfig::onKWinSettingsChange(const KConfigGroup &group, const QByteArrayList &names) const
{
    if (group.name() == QStringLiteral("org.kde.kdecoration2")
            && (names.contains("ButtonsOnRight") || names.contains("ButtonsOnLeft"))) {
        setWindowDecorationsButtonsOrder();
    }
}

void GtkConfig::onKCMInputSettingsChange(const KConfigGroup& group, const QByteArrayList& names) const
{
    if (group.name() == QStringLiteral("Mouse")
            && names.contains("cursorTheme")) {
        setCursorTheme();
    }
}


#include "gtkconfig.moc"
