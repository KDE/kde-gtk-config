add_library(gtkconfig_xsettings MODULE)

set(gtkconfig_xsettings_SRCS
    gtkconfig.cpp
    gsd-xsettings-manager/gsd-xsettings-manager.cpp
    ../kded/configvalueprovider.cpp
    ../kded/config_editor/xsettings.cpp
    ../kded/config_editor/settings_ini.cpp
    ../kded/config_editor/utils.cpp
)

qt_add_dbus_adaptor(gtkconfig_xsettings_SRCS
    gsd-xsettings-manager/org.gtk.Settings.xml
    gsd-xsettings-manager/gsd-xsettings-manager.h
    GSDXSettingsManager
    gsd_xsettings_manager_adaptor
    GSDXSettingsManagerAdaptor
)

target_sources(gtkconfig_xsettings
  PRIVATE
    ${gtkconfig_xsettings_SRCS}
)

target_compile_definitions(gtkconfig_xsettings
  PUBLIC
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
)

target_link_libraries(gtkconfig_xsettings
  PRIVATE
    KWinBridge
    KF6::WindowSystem
  PUBLIC
    Qt::DBus
    PkgConfig::GIO
    PkgConfig::GObject
)

target_link_libraries(gtkconfig_xsettings
  PUBLIC
    Qt::Svg
    KF6::ColorScheme
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::DBusAddons
    KF6::GuiAddons
    PkgConfig::GTK+3
)

target_include_directories(gtkconfig_xsettings
  PRIVATE
    ../kded
)

install(TARGETS gtkconfig_xsettings DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/kded)
