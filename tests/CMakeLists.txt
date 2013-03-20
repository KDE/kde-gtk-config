kde4_add_unit_test(fontstest fontstest.cpp ../src/fontshelpers.cpp)
target_link_libraries(fontstest ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} ${KDE4_KDECORE_LIBS})
