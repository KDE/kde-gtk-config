add_executable(gtk_theme gtktheme.cpp ../kded/configeditor.cpp)

target_link_libraries(gtk_theme
  PRIVATE
    Qt::Core
    KF5::ConfigCore
    KF5::ConfigWidgets
    KF5::GuiAddons
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
