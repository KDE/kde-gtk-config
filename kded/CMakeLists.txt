add_subdirectory(kwin_bridge)
add_subdirectory(scss)

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

target_link_libraries(gtkconfig
  PRIVATE
    KWinBridge
  PUBLIC
    Qt::DBus
    Qt::Svg
    KF5::CoreAddons
    KF5::ConfigCore
    KF5::ConfigWidgets
    KF5::DBusAddons
    KF5::GuiAddons
    PkgConfig::GTK+3
    PkgConfig::GIO
    PkgConfig::GObject
)

install(TARGETS gtkconfig DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/kded)
