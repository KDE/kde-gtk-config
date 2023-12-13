# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2023 Fushan Wen <qydwhotmail@gmail.com>

add_test(
    NAME colorreloadmoduletest_x11
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/colorreloadmoduletest.py
)
execute_process(COMMAND gcc -print-file-name=libasan.so OUTPUT_VARIABLE LIBASAN_PATH ECHO_OUTPUT_VARIABLE OUTPUT_STRIP_TRAILING_WHITESPACE)
set_tests_properties(colorreloadmoduletest_x11 PROPERTIES ENVIRONMENT "LD_PRELOAD=${LIBASAN_PATH};GTK_THEME=Breeze")

add_test(
    NAME colorreloadmoduletest_wayland
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/colorreloadmoduletest_wayland.py
)
set_tests_properties(colorreloadmoduletest_wayland PROPERTIES ENVIRONMENT "GDK_BACKEND=wayland;GTK_THEME=Breeze;LD_PRELOAD=${LIBASAN_PATH}")