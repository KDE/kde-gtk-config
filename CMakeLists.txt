project(kde-gtk-config)
set(PROJECT_VERSION "5.18.80")
cmake_minimum_required(VERSION 3.10)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED NO_MODULE COMPONENTS DBus)
find_package(KF5CoreAddons REQUIRED)
find_package(KF5Config REQUIRED)
find_package(KF5IconThemes REQUIRED)
find_package(KF5DBusAddons REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(GSettingSchemas REQUIRED)
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

set_package_properties(XSettingsd PROPERTIES
    DESCRIPTION "XSettingsd daemon"
    TYPE RUNTIME
    PURPOSE "Allows GTK Config kded module to apply settings to GTK applications on the fly"
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake kded/config.h)

add_subdirectory(gtkproxies)
add_subdirectory(gtk3proxies)
add_subdirectory(kded)
add_subdirectory(kconf_update)
add_subdirectory(color-reload-module)

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
