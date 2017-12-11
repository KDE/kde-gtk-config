macro(add_kgc_test name)
    add_executable(${name} ${name}.cpp ${ARGV})
    add_test(${name} ${name})
    ecm_mark_as_test(${name})
    target_link_libraries(${name} ${GIO2_LIBRARY} ${GLIB2_LIBRARY} ${GTK3_LIBRARY} ${GOBJECT2_LIBRARY} Qt5::Core Qt5::Gui Qt5::Test KF5::ConfigCore)
    target_include_directories(${name} PRIVATE ${CMAKE_BINARY_DIR})
endmacro(add_kgc_test)

add_kgc_test(configsavetest ../src/abstractappearance.cpp ../src/appearancegtk2.cpp ../src/appearancegtk3.cpp)
