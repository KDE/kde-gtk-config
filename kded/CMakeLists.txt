add_subdirectory(kwin_bridge)
add_subdirectory(scss)

add_library(gtkconfig MODULE)

target_sources(gtkconfig 
  PRIVATE
    gtkconfig.cpp
    configvalueprovider.cpp
    themepreviewer.cpp
)

add_subdirectory(config_editor)

target_compile_definitions(gtkconfig
  PUBLIC
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
)

target_link_libraries(gtkconfig
  PRIVATE
    KWinBridge
    KF6::WindowSystem
  PUBLIC
    Qt::DBus
    Qt::Svg
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::ConfigWidgets
    KF6::DBusAddons
    KF6::GuiAddons
    PkgConfig::GTK+3
    PkgConfig::GIO
    PkgConfig::GObject
)

install(TARGETS gtkconfig DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf${QT_MAJOR_VERSION}/kded)
