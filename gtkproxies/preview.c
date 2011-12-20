/*      Copyright 2011 Aleix Pol Gonzalez <aleixpol@kde.org>
 * 
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation version 3 of the License.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include <sys/inotify.h>

int inotifyDescriptor;

static void on_dlg_response(GtkDialog* dlg, int res, gpointer user_data)
{
    switch(res)
    {
    default:
        gtk_main_quit();
    }
}

int initializeInotify(gchar* target)
{
    inotifyDescriptor = inotify_init();
    if(inotifyDescriptor<0) {
        perror("gtk-preview");
        exit(123);
    }
    
    int r = inotify_add_watch (inotifyDescriptor, target, IN_CLOSE_WRITE);
    if(r<0) {
        perror("gtk-preview");
        exit(124);
    }
    fprintf(stderr, "watching %s\n", target);
}

void reloadstyle(GIOChannel *source,
                    GIOCondition condition,
                    gpointer data)
{
    fprintf(stderr, "changing settings...\n");
    char buf[200];
    ssize_t r = read(inotifyDescriptor, buf, 200);
    
    gtk_rc_reparse_all();
    fprintf(stderr, "settings changed!! %d\n", r);
}

int main(int argc, char **argv)
{
    GError     *error = NULL;
    unsigned long wid=0;
    if(argc==2)
        sscanf(argv[1], "%ld", &wid);
/*     fprintf(stderr, "holaaa %ld %s\n", argc, argv[1]);*/
    gtk_init( &argc, &argv );
    const char* ui_file = DATA_DIR "/preview.ui";
 
    GtkBuilder *builder = gtk_builder_new();
    if( ! gtk_builder_add_from_file( builder, ui_file, &error ) ) {
        g_warning( "%s", error->message );
        g_free( error );
        return 1;
    }
    
    GtkWidget *previewUI = GTK_WIDGET( gtk_builder_get_object( builder, "frame1" ) );
    gtk_builder_connect_signals( builder, NULL );
    
    /* a plug when embedded, a window when a window */
    GtkWidget* window;
    
    if(wid==0) {
        window = gtk_dialog_new();
        gtk_container_add (GTK_CONTAINER (GTK_DIALOG (window)->vbox), previewUI);
        g_signal_connect(window, "response", G_CALLBACK(on_dlg_response), NULL);
    } else {
        window = gtk_plug_new(wid);
        gtk_container_add (GTK_CONTAINER (window), previewUI);
    }
    
    GdkColor black = {0, 0, 0, 0};
    gtk_widget_modify_bg(previewUI, GTK_STATE_NORMAL, &black);
    
    g_signal_connect (window, "destroy",
                    G_CALLBACK (gtk_widget_destroyed),
                    &window);
    
    gtk_widget_show_all ( window );
    
    if(wid)
        fprintf(stderr, "--- is embedded: %d\n", gtk_plug_get_embedded(GTK_PLUG(window)));
    
    gchar** files = gtk_rc_get_default_files();
    initializeInotify(files[0]);
    GIOChannel* channel = g_io_channel_unix_new(inotifyDescriptor);
    guint ret = g_io_add_watch(channel, G_IO_IN, reloadstyle, NULL);
    g_object_unref( G_OBJECT( builder ) );
    
    gtk_main();
    
    g_io_channel_unref(channel);
    
    return 0;
}








