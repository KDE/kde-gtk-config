add_executable(reload_gtk_apps reload.c)
target_link_libraries(reload_gtk_apps PkgConfig::GTK+2)
install(TARGETS reload_gtk_apps RUNTIME DESTINATION "${LIBEXEC_INSTALL_DIR}")

add_executable(gtk_preview preview.c)
target_compile_definitions(gtk_preview PRIVATE "-DDATA_DIR=\"${KDE_INSTALL_FULL_DATADIR}/kcm-gtk-module/\"")
target_link_libraries(gtk_preview PkgConfig::GTK+2)
install(TARGETS gtk_preview RUNTIME DESTINATION "${LIBEXEC_INSTALL_DIR}")

# preview file used for the gtk3 version too
install(FILES preview.ui
    DESTINATION ${DATA_INSTALL_DIR}/kcm-gtk-module/)
