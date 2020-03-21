find_package(GTK2 REQUIRED)

include_directories(BEFORE ${GTK2_INCLUDE_DIRS})

add_executable(reload_gtk_apps reload.c)
target_link_libraries(reload_gtk_apps ${GTK2_LIBRARIES})
install(TARGETS reload_gtk_apps RUNTIME DESTINATION "${LIBEXEC_INSTALL_DIR}")


add_executable(gtk_preview preview.c)
target_compile_definitions(gtk_preview PRIVATE "-DDATA_DIR=\"${KDE_INSTALL_FULL_DATADIR}/kcm-gtk-module/\"")
target_link_libraries(gtk_preview ${GTK2_LIBRARIES} ${GTK2_GIOMM_LIBRARY})
install(TARGETS gtk_preview RUNTIME DESTINATION "${LIBEXEC_INSTALL_DIR}")

# preview file used for the gtk3 version too
install(FILES preview.ui
    DESTINATION ${DATA_INSTALL_DIR}/kcm-gtk-module/)
