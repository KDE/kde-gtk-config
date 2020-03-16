/*
 * Copyright (C) 2020  Carson Black <uhhadd@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>
#include <glib.h>
#include <gio/gio.h>
#include <gdk/gdk.h>

gchar *path;
GFile *file;
GFileMonitor *mon;
GtkCssProvider *provider;

void reload_colours() {
    if (provider == NULL) {
        provider = gtk_css_provider_new();
    } else {
        gtk_style_context_remove_provider_for_screen(
            gdk_screen_get_default(),
            GTK_STYLE_PROVIDER(provider)
        );
        g_clear_object(&provider);
        provider = gtk_css_provider_new();
    }
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER (provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER+1
    );
    gtk_css_provider_load_from_file(
        provider,
        file,
        NULL
    );
}

void gtk_module_init(gint *argc, gchar ***argv[]) {
    path = g_strconcat(g_get_home_dir(), "/.config/gtk-3.0/colors.css", NULL);
    file = g_file_new_for_path(path);

    mon = g_file_monitor_file(
        file,
        G_FILE_MONITOR_NONE,
        NULL,
        NULL
    );

    g_signal_connect(mon, "changed", G_CALLBACK(reload_colours), NULL);
}
