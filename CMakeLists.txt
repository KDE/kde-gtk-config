project(chakragtkconfig)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
    ${CMAKE_SOURCE_DIR} 
    ${CMAKE_SOURCE_DIR}/src 
    ${CMAKE_SOURCE_DIR}/src/ui 
    ${KDE4_INCLUDES} ${QT_INCLUDES})

add_subdirectory(translations)

set(kcm_SRCS 
     src/appearencegtk.cpp
     src/thread.cpp
     src/installer.cpp 
     src/gtkconfigkcmodule.cpp
     src/dialog_installer.cpp 
     src/dialog_uninstaller.cpp 
)

set(GHNS_FILES 
     cgcicon.knsrc
     cgctheme.knsrc
     cgcgtk3.knsrc
)

kde4_add_ui_files(kcm_SRCS 
     src/ui/gui.ui 
     src/ui/dialog_installer.ui 
     src/ui/dialog_uninstaller.ui
)
kde4_add_plugin(kcm_cgc ${kcm_SRCS})

target_link_libraries(kcm_cgc ${KDE4_KIO_LIBS} ${KDE4_KNEWSTUFF3_LIBS})

install(TARGETS kcm_cgc DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES ${GHNS_FILES} DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES kde-gtk-config.desktop DESTINATION ${SERVICES_INSTALL_DIR})

add_subdirectory(gtkproxies)
add_subdirectory(gtk3proxies)
add_subdirectory(icons)
