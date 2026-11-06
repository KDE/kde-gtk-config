add_library(menubar-hover-gtk3-module MODULE
    menubarhover.cpp
)

target_link_libraries(menubar-hover-gtk3-module
  PRIVATE
    PkgConfig::GTK+3
)

# Loaded into every GTK3 application, keep it free of the C++ runtime
target_compile_options(menubar-hover-gtk3-module
  PRIVATE
    -fno-exceptions
    -fno-rtti
)

target_link_options(menubar-hover-gtk3-module
  PRIVATE
    -Wl,--as-needed
)

set_target_properties(menubar-hover-gtk3-module
  PROPERTIES
    OUTPUT_NAME libmenubar-hover-gtk-module
    AUTOMOC OFF
)

install(
    TARGETS menubar-hover-gtk3-module
    DESTINATION ${KDE_INSTALL_LIBDIR}/gtk-3.0/modules
)
