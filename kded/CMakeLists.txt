set(kscreen_daemon_SRCS
    gtkconfig.cpp
    configeditor.cpp
    configvalueprovider.cpp
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
    Qt5::Gui
    Qt5::DBus
    KF5::CoreAddons
    KF5::ConfigCore
    KF5::DBusAddons
    KF5::IconThemes
    ${GIO2_LIBRARY}
    ${GOBJECT2_LIBRARY}
)

install(TARGETS gtkconfig DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/kded)