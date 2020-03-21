find_package(GTK3 REQUIRED)
# gtk3 pulls in X11 headers, we need to add the include path if they are not in /usr/include
find_package(X11)


include_directories(${GTK3_INCLUDE_DIRS})

add_executable(gtk3_preview preview3.c)
if(X11_FOUND)
  target_include_directories(gtk3_preview PRIVATE ${X11_X11_INCLUDE_PATH})
endif()
target_compile_definitions(gtk3_preview PRIVATE "-DDATA_DIR=\"${KDE_INSTALL_FULL_DATADIR}/kcm-gtk-module/\"")
target_link_libraries(gtk3_preview ${GTK3_LIBRARY} ${GOBJECT2_LIBRARY} ${GLIB2_LIBRARY})
install(TARGETS gtk3_preview  RUNTIME DESTINATION "${LIBEXEC_INSTALL_DIR}")
