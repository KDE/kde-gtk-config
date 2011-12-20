#! /bin/sh
$EXTRACTRC `find . -name \*.ui` >> rc.cpp
$XGETTEXT rc.cpp src/*.cpp -o $podir/kde-gtk-config.pot

