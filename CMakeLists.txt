project(chakragtkconfig)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

set(CMAKE_PREFIX_PATH /usr)

message("PREFIX : ${CMAKE_PREFIX_PATH}")
find_package(Qt4 REQUIRED)
find_package(KDE4 REQUIRED)
find_package(MSGFMT REQUIRED)
include(KDE4Defaults)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
    ${CMAKE_SOURCE_DIR} 
    ${CMAKE_SOURCE_DIR}/src 
    ${CMAKE_SOURCE_DIR}/src/ui 
    ${KDE4_INCLUDES} ${QT_INCLUDES})

add_subdirectory(translations)

# Codigos Fuente
set(kcm_SRCS 
     src/aparienciagtk.cpp 
     src/thread.cpp
     src/installer.cpp 
     src/modulo.cpp 
     src/dialog_installer.cpp 
     src/dialog_uninstaller.cpp 

)

# ficheros para GHNS
set(GHNS_FILES 
     cgcicon.knsrc
     cgctheme.knsrc
     cgcgtk3.knsrc
)


# Uis 
message("CURRENT FOLDER : ${chakragtkconfig_SOURCE_DIR}")
kde4_add_ui_files(kcm_SRCS 
     src/ui/gui.ui 
     src/ui/dialog_installer.ui 
     src/ui/dialog_uninstaller.ui
)
kde4_add_plugin(kcm_cgc ${kcm_SRCS})

message("GHNS DIR : ${CONFIG_INSTALL_DIR}")

target_link_libraries(kcm_cgc ${KDE4_KIO_LIBS} ${KDE4_KNEWSTUFF3_LIBS})
message("THE PLUGIN WILL BE INSTALLED HERE = ${KDE4_LIB_DIR}/kde4")

install(TARGETS kcm_cgc DESTINATION ${KDE4_LIB_DIR}/kde4)
install(FILES ${GHNS_FILES} DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES chakra-gtk-config.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})