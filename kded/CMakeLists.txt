set(kscreen_daemon_SRCS
    gtkconfig.cpp
    configeditor.cpp
    configvalueprovider.cpp
    themepreviewer.cpp
)

add_library(gtkconfig MODULE ${kscreen_daemon_SRCS})

target_compile_definitions(gtkconfig
  PUBLIC
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
)

target_include_directories(gtkconfig
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GTK3_INCLUDE_DIRS}
)

target_link_libraries(gtkconfig
  PUBLIC
    Qt5::DBus
    KF5::CoreAddons
    KF5::ConfigCore
    KF5::ConfigWidgets
    KF5::DBusAddons
    KF5::IconThemes
    PkgConfig::GTK+3
    KF5::GuiAddons
    PkgConfig::GIO
    PkgConfig::GObject
)

install(TARGETS gtkconfig DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/kded)
