/*
 * SPDX-FileCopyrightText: 2020 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <gtk/gtk.h>
#include <glib.h>
#include <gio/gio.h>
#include <gdk/gdk.h>

gchar *window_decorations_css_path;
GFile *window_decorations_css_file;

gchar *gtk_css_path;
GFile *gtk_css_file;

gchar *assets_directory_path;
GFile *assets_directory;

GFileMonitor *directory_monitor;
GFileMonitor *gtk_css_monitor;
GtkCssProvider *css_provider;

void reload_decorations();
void remove_css_provider_if_needed();
void remove_css_provider();

__attribute__((visibility("default"))) void gtk_module_init(gint *argc, gchar ***argv[])
{
    window_decorations_css_path = g_strconcat(g_get_user_config_dir(), "/gtk-3.0/window_decorations.css", NULL);
    window_decorations_css_file = g_file_new_for_path(window_decorations_css_path);

    gtk_css_path = g_strconcat(g_get_user_config_dir(), "/gtk-3.0/gtk.css", NULL);
    gtk_css_file = g_file_new_for_path(gtk_css_path);

    assets_directory_path = g_strconcat(g_get_user_config_dir(), "/gtk-3.0/assets/", NULL);
    assets_directory = g_file_new_for_path(assets_directory_path);


    gtk_css_monitor = g_file_monitor_file(gtk_css_file, G_FILE_MONITOR_NONE, NULL, NULL);
    directory_monitor = g_file_monitor_directory(assets_directory, G_FILE_MONITOR_NONE, NULL, NULL);

    g_signal_connect(directory_monitor, "changed", G_CALLBACK(reload_decorations), NULL);
    g_signal_connect(gtk_css_monitor, "changed", G_CALLBACK(remove_css_provider_if_needed), NULL);
}

void reload_decorations()
{
    if (css_provider == NULL) {
        css_provider = gtk_css_provider_new();
    } else {
        remove_css_provider();
        css_provider = gtk_css_provider_new();
    }

    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER (css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER + 1
    );

    gtk_css_provider_load_from_file(css_provider, window_decorations_css_file, NULL);
}

void remove_css_provider_if_needed()
{
    gchar *gtk_css_contents;
    gboolean success = g_file_load_contents(gtk_css_file, NULL, &gtk_css_contents, NULL, NULL, NULL);
    if (success == FALSE) {
        return;
    }

    // If custom decorations are disabled we must unload our custom CSS
    if (g_strrstr(gtk_css_contents, "@import 'window_decorations.css';") == NULL) {
        if (css_provider != NULL) {
            remove_css_provider();
        }
    }

    g_free(gtk_css_contents);
}

void remove_css_provider()
{
    gtk_style_context_remove_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css_provider));
    g_clear_object(&css_provider);
}
