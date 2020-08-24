add_library(im-wrapper
  MODULE
    module.cpp
    gtkimcontextwrapper.cpp
)

set(module_path ${KDE_INSTALL_FULL_LIBDIR}/gtk-3.0/3.0.0/immodules)
configure_file(conf.h.in conf.h)

target_compile_definitions(im-wrapper
  PUBLIC
    QT_NO_SIGNALS_SLOTS_KEYWORDS
)

target_link_libraries(im-wrapper
  PRIVATE
    PkgConfig::GLib2 
    PkgConfig::GTK+3 
    PkgConfig::PlasmaKeyData
    Qt5::Core
    KF5::ConfigCore
    ${CMAKE_DL_LIBS}
)

set_target_properties(im-wrapper
  PROPERTIES
    PREFIX im-
)

install(
  TARGETS
    im-wrapper
  DESTINATION
    ${module_path}
)
