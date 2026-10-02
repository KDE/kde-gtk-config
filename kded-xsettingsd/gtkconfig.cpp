/*
 * XSettings-focused kded plugin. Contains only XSettings and xsettingsd-related actions.
 */

#include "gtkconfig.h"

#include <KPluginFactory>
#include <KWindowSystem>

#include <QDBusConnection>
#include <QDir>
#include <QStandardPaths>
#include <QTimer>

#include "config_editor/settings_ini.h"
#include "config_editor/xsettings.h"
#include "gsd-xsettings-manager/gsd-xsettings-manager.h"

K_PLUGIN_CLASS_WITH_JSON(GtkConfigXSettings, "gtkconfig-xsettings.json")

namespace
{
QStringList gtkThemeSearchPaths()
{
    QStringList paths{
        QDir::homePath() + QStringLiteral("/.themes/"),
        QDir::homePath() + QStringLiteral("/.local/share/themes/"),
    };
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
    static const QStringList suffixes{QStringLiteral("-dark"),
                                      QStringLiteral("-Dark"),
                                      QStringLiteral(" dark"),
                                      QStringLiteral(" Dark"),
                                      QStringLiteral("dark"),
                                      QStringLiteral("Dark")};
    for (const QString &suffix : suffixes) {
        const QString candidate = themeName + suffix;
        if (gtkThemeExists(candidate)) {
            return candidate;
        }
    }
    return {};
}
}

GtkConfigXSettings::GtkConfigXSettings(QObject *parent, const QVariantList &)
    : KDEDModule(parent)
    , configValueProvider(new ConfigValueProvider())
    , kdeglobalsConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig()))
    , kwinConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("kwinrc"))))
    , kcminputConfigWatcher(KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("kcminputrc"))))
{
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerService(QStringLiteral("org.kde.GtkConfig.XSettings"));
    dbus.registerObject(QStringLiteral("/GtkConfigXSettings"), this, QDBusConnection::ExportScriptableSlots);

    if (qgetenv("GTK_USE_PORTAL") != "1" && KWindowSystem::isPlatformWayland()) {
        m_gsdXsettingsManager = new GSDXSettingsManager(this);
    }

    connect(kdeglobalsConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfigXSettings::onKdeglobalsSettingsChange);
    connect(kwinConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfigXSettings::onKWinSettingsChange);
    connect(kcminputConfigWatcher.data(), &KConfigWatcher::configChanged, this, &GtkConfigXSettings::onKCMInputSettingsChange);
    applyAllSettings();
}

GtkConfigXSettings::~GtkConfigXSettings()
{
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.unregisterService(QStringLiteral("org.kde.GtkConfig.XSettings"));
    dbus.unregisterObject(QStringLiteral("/GtkConfigXSettings"));
}

void GtkConfigXSettings::setGtk2Theme(const QString &themeName, const bool preferDarkTheme) const
{
    QString possiblyDarkThemeName = themeName;
    if (preferDarkTheme) {
        const QString darkVariant = findGtk2DarkThemeVariant(themeName);
        if (!darkVariant.isEmpty()) {
            possiblyDarkThemeName = darkVariant;
        }
    }

    XSettingsEditor::setValue(QStringLiteral("Net/ThemeName"), possiblyDarkThemeName);
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
    qreal xwaylandScale = 1.0;
    if (KWindowSystem::isPlatformWayland()) {
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
    if (m_gsdXsettingsManager) {
        m_gsdXsettingsManager->enableAnimationsChanged();
    }
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

void GtkConfigXSettings::setColors() const
{
    if (m_gsdXsettingsManager) {
        m_gsdXsettingsManager->modulesChanged();
    }
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
    setColors();
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
        if (names.contains(QByteArrayLiteral("fixed"))) {
            /* font handling moved to main plugin */
        }
        if (names.contains(QByteArrayLiteral("font"))) {
            setFont();
        }
        if (names.contains(QByteArrayLiteral("ColorScheme")) || names.contains(QByteArrayLiteral("AccentColor"))) {
            setColors();
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

#include "gtkconfig.moc"
