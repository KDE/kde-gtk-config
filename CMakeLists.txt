project(kde-gtk-config)
set(PROJECT_VERSION "5.22.5")
cmake_minimum_required(VERSION 3.16)

set(QT_MIN_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.82")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${ECM_MODULE_PATH})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS DBus Svg)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS CoreAddons Config ConfigWidgets GuiAddons DBusAddons)
find_package(PkgConfig REQUIRED)
find_package(KDecoration2 REQUIRED)
find_package(GSettingSchemas REQUIRED)
find_package(Sass REQUIRED)
find_package(XSettingsd)

pkg_check_modules(GTK+3 REQUIRED IMPORTED_TARGET gtk+-3.0)
pkg_check_modules(GTK+2 REQUIRED IMPORTED_TARGET gtk+-2.0)
pkg_check_modules(GLib2 REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(GObject REQUIRED IMPORTED_TARGET gobject-2.0)
pkg_check_modules(GIO REQUIRED IMPORTED_TARGET gio-2.0)

include(ECMSetupVersion)
include(ECMInstallIcons)
include(ECMMarkAsTest)
include(GenerateExportHeader)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)

set_package_properties(XSettingsd PROPERTIES
    DESCRIPTION "XSettingsd daemon"
    TYPE RUNTIME
    PURPOSE "Required to apply settings to GTK applications on the fly"
)

set_package_properties(Sass PROPERTIES
    DESCRIPTION "SASS compiler"
    URL "https://sass-lang.com/"
    PURPOSE "Required for building a modification, that allows Breeze GTK to change it's window decorations' appearance"
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake kded/config.h)

add_subdirectory(gtk3proxies)
add_subdirectory(kded)
add_subdirectory(kconf_update)
add_subdirectory(color-reload-module)
add_subdirectory(window-decorations-reload-module)

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
