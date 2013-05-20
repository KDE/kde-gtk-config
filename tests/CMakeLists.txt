macro(add_kgc_test name)
    kde4_add_unit_test(${name} ${name}.cpp ${ARGV})
    target_link_libraries(${name} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} ${KDE4_KDECORE_LIBS})
endmacro(add_kgc_test)

add_kgc_test(fontstest ../src/fontshelpers.cpp)
add_kgc_test(configsavetest ../src/abstractappearance.cpp ../src/appearancegtk2.cpp ../src/appearancegtk3.cpp)