project(kde-gtk-config)
set(PROJECT_VERSION "5.3.1")
cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED NO_MODULE COMPONENTS Widgets Test)
find_package(KF5 REQUIRED COMPONENTS I18n KIO ConfigWidgets NewStuff Archive KCMUtils IconThemes)

include_directories(
    ${CMAKE_SOURCE_DIR} 
    ${CMAKE_SOURCE_DIR}/src 
    ${CMAKE_SOURCE_DIR}/src/ui 
)

include(ECMSetupVersion)
include(ECMInstallIcons)
include(ECMMarkAsTest)
include(GenerateExportHeader)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

# Set KI18n translation domain
add_definitions(-DTRANSLATION_DOMAIN=\"kde-gtk-config\")

set(kcm_SRCS
     src/iconthemesmodel.cpp
     src/appearancegtk3.cpp
     src/appearancegtk2.cpp
     src/appearencegtk.cpp
     src/abstractappearance.cpp
     src/thread.cpp
     src/installer.cpp 
     src/gtkconfigkcmodule.cpp
     src/dialog_installer.cpp 
     src/dialog_uninstaller.cpp 
     src/fontshelpers.cpp
)

set(GHNS_FILES 
     cgcicon.knsrc
     cgctheme.knsrc
     cgcgtk3.knsrc
)

ki18n_wrap_ui(kcm_SRCS 
     src/ui/gui.ui 
     src/ui/dialog_installer.ui 
     src/ui/dialog_uninstaller.ui
)
add_library(kcm_kdegtkconfig MODULE ${kcm_SRCS})
target_compile_definitions(kcm_kdegtkconfig PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")
target_link_libraries(kcm_kdegtkconfig KF5::I18n KF5::KIOWidgets KF5::NewStuff KF5::Archive KF5::NewStuff KF5::ConfigWidgets KF5::IconThemes)

kcoreaddons_desktop_to_json(kcm_kdegtkconfig kde-gtk-config.desktop)

install(TARGETS kcm_kdegtkconfig DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES ${GHNS_FILES} DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES kde-gtk-config.desktop DESTINATION ${SERVICES_INSTALL_DIR})

add_subdirectory(gtkproxies)
add_subdirectory(gtk3proxies)
add_subdirectory(icons)
add_subdirectory(tests)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
