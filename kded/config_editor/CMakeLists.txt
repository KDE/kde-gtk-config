# SPDX-FileCopyrightText: 2022 Mikhail Zolotukhin <zomial@protonmail.com>
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

# Key/value writers for the GTK settings channels, shared by the kded module, kconf_update and tests
add_library(gtkconfig_backends STATIC
    gsettings.cpp
    gtk2.cpp
    settings_ini.cpp
    utils.cpp
    xsettings.cpp
)
set_target_properties(gtkconfig_backends PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(gtkconfig_backends PUBLIC QT_NO_SIGNALS_SLOTS_KEYWORDS)
target_include_directories(gtkconfig_backends PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(gtkconfig_backends
  PUBLIC
    Qt::Core
    KF6::ConfigCore
    PkgConfig::GIO
)

target_sources(gtkconfig
  PRIVATE
    custom_css.cpp
)

qt_add_resources(gtkconfig "window_decorations_css"
    PREFIX "/"
    FILES window_decorations.css
)
