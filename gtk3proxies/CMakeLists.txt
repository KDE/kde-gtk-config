# gtk3 pulls in X11 headers, we need to add the include path if they are not in /usr/include
find_package(X11)

add_executable(gtk3_preview preview3.c)

if(X11_FOUND)
  target_include_directories(gtk3_preview PRIVATE ${X11_X11_INCLUDE_PATH})
endif()

target_compile_definitions(gtk3_preview PRIVATE "-DDATA_DIR=\"${KDE_INSTALL_FULL_DATADIR}/kcm-gtk-module/\"")
target_link_libraries(gtk3_preview PkgConfig::GTK+3 PkgConfig::GLib2 PkgConfig::GObject)
install(TARGETS gtk3_preview  RUNTIME DESTINATION "${KDE_INSTALL_LIBEXECDIR}")
