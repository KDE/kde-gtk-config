/*
 * SPDX-FileCopyrightText: 2011 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

static void printHelp(void)
{
    printf(
        "Small utility for previewing Gtk4 theme.\n"
        "\n"
        "Using:\n"
        "gtk4_preview <args>\n"
        "\n"
        "Arguments:\n"
        "  -h, --help\tShows this help\n"
        "  -V, --version\tPrints the program version\n");
}

int main(int argc, char **argv)
{
    for (int i = 0; i < argc; i++) {
        if (strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0) {
            printHelp();
            return 0;
        } else if (strcmp("-V", argv[i]) == 0 || strcmp("--version", argv[i]) == 0) {
            printf("gtk4_preview version 1.0\n");
            return 0;
        }
    }

    gtk_init();

    GError *error = NULL;
    GtkBuilder *builder = gtk_builder_new();
    if (!gtk_builder_add_from_file(builder, DATA_DIR "/preview.ui", &error)) {
        g_warning("%s", error->message);
        g_error_free(error);
        return 1;
    }

    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "GTK Theme Preview");
    gtk_window_set_child(GTK_WINDOW(window), GTK_WIDGET(gtk_builder_get_object(builder, "preview")));
    g_object_unref(builder);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(g_main_loop_quit), loop);
    gtk_window_present(GTK_WINDOW(window));
    g_main_loop_run(loop);

    return 0;
}
