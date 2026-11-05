# SPDX-FileCopyrightText: 2026 David Edmundson <kde@davidedmundson.co.uk>
# SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

add_library(gtkconfig_xsettings MODULE)

target_sources(gtkconfig_xsettings
  PRIVATE
    gtkconfig-xsettings.cpp
    ../kded/configvalueprovider.cpp
    ../kded/config_editor/xsettings.cpp
    ../kded/config_editor/settings_ini.cpp
    ../kded/config_editor/utils.cpp
)

target_compile_definitions(gtkconfig_xsettings
  PUBLIC
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
)

target_link_libraries(gtkconfig_xsettings
  PRIVATE
    KWinBridge
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
