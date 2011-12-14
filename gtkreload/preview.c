#include <gtk/gtk.h>

int main(int argc, char **argv)
{
    GError     *error = NULL;
 
    gtk_init( &argc, &argv );
    const char* ui_file = DATA_DIR "/preview.ui";
 
    GtkBuilder *builder = gtk_builder_new();
    if( ! gtk_builder_add_from_file( builder, ui_file, &error ) ) {
        g_warning( "%s", error->message );
        g_free( error );
        return 1;
    }
 
    /* Get main window pointer from UI */
    GtkWidget *window = GTK_WIDGET( gtk_builder_get_object( builder, "window1" ) );
 
    gtk_builder_connect_signals( builder, NULL );
 
    /* Destroy builder, since we don't need it anymore */
    g_object_unref( G_OBJECT( builder ) );
 
    gtk_widget_show_all( window );
 
    gtk_main();
 
    return 0;
}
