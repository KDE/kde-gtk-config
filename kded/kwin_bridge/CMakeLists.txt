add_library(KWinBridge STATIC
    dummydecoratedclient.cpp dummydecoratedclient.h
    dummydecorationbridge.cpp dummydecorationbridge.h
    dummydecorationsettings.cpp dummydecorationsettings.h
    decorationpalette.cpp decorationpalette.h
    decorationpainter.cpp decorationpainter.h
    auroraedecorationpainter.cpp auroraedecorationpainter.h
    standarddecorationpainter.cpp standarddecorationpainter.h
)
set_property(TARGET KWinBridge PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(KWinBridge
  PUBLIC
    KDecoration2::KDecoration
  PRIVATE
    KDecoration2::KDecoration2Private
    KF5::ConfigWidgets
    Qt::Svg
)
