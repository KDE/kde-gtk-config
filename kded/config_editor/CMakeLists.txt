# SPDX-FileCopyrightText: 2022 Mikhail Zolotukhin <zomial@protonmail.com>
# SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

target_sources(gtkconfig 
  PRIVATE 
    custom_css.cpp custom_css.h 
    gsettings.cpp gsettings.h 
    gtk2.cpp gtk2.h
    settings_ini.cpp settings_ini.h 
    utils.cpp utils.h
    xsettings.cpp xsettings.h
)
