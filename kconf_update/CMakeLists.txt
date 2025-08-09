add_executable(gtk_theme)

target_sources (gtk_theme
  PRIVATE
    gtktheme.cpp
    ../kded/config_editor/gsettings.cpp
    ../kded/config_editor/gtk2.cpp
    ../kded/config_editor/settings_ini.cpp
    ../kded/config_editor/utils.cpp
    ../kded/config_editor/xsettings.cpp
)

target_link_libraries(gtk_theme
  PRIVATE
    Qt::Core
    KF6::ConfigCore
    KF6::GuiAddons
    PkgConfig::GIO
    PkgConfig::GObject
)

target_include_directories(gtk_theme
  PRIVATE
    ../kded/
)

target_compile_definitions(gtk_theme
  PRIVATE
    QT_NO_SIGNALS_SLOTS_KEYWORDS
)

install(
  TARGETS
    gtk_theme
  DESTINATION
    ${KDE_INSTALL_LIBDIR}/kconf_update_bin/
)

add_executable(remove_deprecated_gtk4_option_v2
  remove_deprecated_gtk4_option_v2.cpp
   ../kded/config_editor/settings_ini.cpp
   ../kded/config_editor/utils.cpp
)

target_link_libraries(remove_deprecated_gtk4_option_v2
  PRIVATE
    Qt::Core
    KF6::ConfigCore
    KF6::GuiAddons
    PkgConfig::GIO
    PkgConfig::GObject
)

target_include_directories(remove_deprecated_gtk4_option_v2
  PRIVATE
    ../kded/
)

target_compile_definitions(remove_deprecated_gtk4_option_v2
  PRIVATE
    QT_NO_SIGNALS_SLOTS_KEYWORDS
)

install(
  TARGETS
    remove_deprecated_gtk4_option_v2
  DESTINATION
    ${KDE_INSTALL_LIBDIR}/kconf_update_bin/
)

install(
  FILES
    gtkconfig.upd
  DESTINATION
    ${KDE_INSTALL_KCONFUPDATEDIR}
)
install(
  PROGRAMS
    remove_window_decorations_from_gtk_css.sh
  DESTINATION
    ${KDE_INSTALL_KCONFUPDATEDIR}
)
