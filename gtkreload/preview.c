#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <stdio.h>

static void on_dlg_response(GtkDialog* dlg, int res, gpointer user_data)
{
    switch(res)
    {
    default:
        gtk_main_quit();
    }
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
    
    g_object_unref( G_OBJECT( builder ) );
    
    gtk_main();
    return 0;
}
