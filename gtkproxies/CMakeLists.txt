find_package(GTK2 REQUIRED)

include_directories(${GTK2_INCLUDE_DIRS})

add_executable(reload_gtk_apps reload.c)
target_link_libraries(reload_gtk_apps ${GTK2_LIBRARIES})
install(TARGETS reload_gtk_apps RUNTIME DESTINATION "${KDE4_LIBEXEC_INSTALL_DIR}")

add_definitions(-DDATA_DIR="\\\"${DATA_INSTALL_DIR}/kcm-gtk-module/\\\"")

add_executable(gtk_preview preview.c)
target_link_libraries(gtk_preview ${GTK2_LIBRARIES} ${GTK2_GIOMM_LIBRARY})
install(TARGETS gtk_preview RUNTIME DESTINATION "${KDE4_LIBEXEC_INSTALL_DIR}")

# preview file used for the gtk3 version too
install(FILES preview.ui
    DESTINATION ${DATA_INSTALL_DIR}/kcm-gtk-module/)
