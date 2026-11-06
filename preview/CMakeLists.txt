# SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

add_executable(gtk4_preview preview.c)

target_compile_definitions(gtk4_preview PRIVATE "-DDATA_DIR=\"${KDE_INSTALL_FULL_DATADIR}/kcm-gtk-module/\"")
target_link_libraries(gtk4_preview PkgConfig::GTK4)
# moc's C++ file would pull in libstdc++
set_target_properties(gtk4_preview PROPERTIES C_STANDARD 11 AUTOMOC OFF)

install(TARGETS gtk4_preview RUNTIME DESTINATION "${KDE_INSTALL_LIBEXECDIR}")

install(FILES preview.ui DESTINATION ${KDE_INSTALL_DATADIR}/kcm-gtk-module/)
