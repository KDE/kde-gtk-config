add_executable(gtk_theme gtktheme.cpp)

target_sources(gtk_theme
  PRIVATE
    ../kded/configeditor.cpp
)

target_link_libraries(gtk_theme
  PRIVATE
    Qt5::Core
    KF5::ConfigCore
    ${GIO2_LIBRARY}
    ${GOBJECT2_LIBRARY}
)

target_include_directories(gtk_theme
  PRIVATE
    ../kded/
    ${GTK3_INCLUDE_DIRS}
)

target_compile_definitions(gtk_theme
  PRIVATE
    QT_NO_SIGNALS_SLOTS_KEYWORDS
)

install(
  TARGETS
    gtk_theme
  DESTINATION
    ${LIB_INSTALL_DIR}/kconf_update_bin/
)
install(
  FILES
    gtkconfig.upd
  DESTINATION
    ${KCONF_UPDATE_INSTALL_DIR}
)
