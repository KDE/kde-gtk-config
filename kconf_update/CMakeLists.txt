add_executable(gtk_theme)

target_sources (gtk_theme
  PRIVATE
    gtktheme.cpp
)

target_link_libraries(gtk_theme
  PRIVATE
    gtkconfig_backends
)

install(
  TARGETS
    gtk_theme
  DESTINATION
    ${KDE_INSTALL_LIBDIR}/kconf_update_bin/
)

add_executable(remove_deprecated_gtk4_option_v2
  remove_deprecated_gtk4_option_v2.cpp
)

target_link_libraries(remove_deprecated_gtk4_option_v2
  PRIVATE
    gtkconfig_backends
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
