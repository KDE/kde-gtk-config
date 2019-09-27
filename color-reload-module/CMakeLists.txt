include(GNUInstallDirs)

add_library(colorreload-gtk-module SHARED
    reloader.c
)

target_link_libraries(colorreload-gtk-module
    PRIVATE
      ${GLIB_LIBRARIES}
      ${GTK_LIBRARIES}
)

target_include_directories(
    colorreload-gtk-module PRIVATE
    ${GLIB_INCLUDE_DIRS} ${GTK_INCLUDE_DIRS}
)

install(
    TARGETS colorreload-gtk-module 
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/gtk-3.0/modules
)
