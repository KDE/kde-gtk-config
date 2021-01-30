add_library(KWinBridge STATIC
    dummydecoratedclient.cpp
    dummydecorationbridge.cpp
    dummydecorationsettings.cpp
    decorationpalette.cpp
    decorationpainter.cpp
    auroraedecorationpainter.cpp
    standarddecorationpainter.cpp
)

target_link_libraries(KWinBridge
  PUBLIC
    KDecoration2::KDecoration
  PRIVATE
    KDecoration2::KDecoration2Private
    KF5::ConfigWidgets
    Qt::Svg
)
