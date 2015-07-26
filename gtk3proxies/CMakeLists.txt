find_package(GTK3 REQUIRED)

include_directories(${GTK3_INCLUDE_DIRS})

add_definitions(-DDATA_DIR="\\\"${DATA_INSTALL_DIR}/kcm-gtk-module/\\\"")

add_executable(gtk3_preview preview3.c)
target_link_libraries(gtk3_preview ${GTK3_LIBRARY} ${GOBJECT2_LIBRARY} ${GLIB2_LIBRARY})
install(TARGETS gtk3_preview  RUNTIME DESTINATION "${KDE4_LIBEXEC_INSTALL_DIR}")
