#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    GError     *error = NULL;
    unsigned long wid=0;
    if(argc==2)
        sscanf(argv[1], "%ld", &wid);
    fprintf(stderr, "holaaa %ld %s\n", argc, argv[1]);
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
#if 0
 
    GtkWidget* w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_add(GTK_CONTAINER(w), frame);
    gtk_widget_show_all( w );
#endif
    if(wid==0) {
        GtkWidget* window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        gtk_container_add (GTK_CONTAINER (window), previewUI);
        gtk_widget_show_all (window);
    } else {
        GtkWidget* plug = gtk_plug_new(wid);
        gtk_container_add (GTK_CONTAINER (plug), previewUI);
        gtk_widget_show_all ( plug );
        
        fprintf(stderr, "--- %x\n", gtk_plug_get_embedded(plug));
    }
    
    gtk_main();
    g_object_unref( G_OBJECT( builder ) );
    
    return 0;
}
