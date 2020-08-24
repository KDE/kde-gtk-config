/*
 * SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <gtk/gtk.h>
#include <gtk/gtkimmodule.h>

extern "C" {

void RegisterType(GTypeModule *mod);
GtkIMContext *NewContext(GtkIMContext *wrappee);

}