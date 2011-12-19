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

#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <stdio.h>

GdkEventClient createEvent()
{
    GdkEventClient event;
    event.type = GDK_CLIENT_EVENT;
    event.send_event = TRUE;
    event.window = NULL;
    event.message_type = gdk_atom_intern("_GTK_READ_RCFILES", FALSE);
    event.data_format = 8;
    
    return event;
}

int main(int argc, char** argv)
{
    gtk_init(&argc, &argv);
    int winid=0;
    if(argc==2)
        sscanf(argv[1], "%d", &winid);
    
    GdkEventClient event = createEvent();
    printf("Reload all apps!!!! %d\n", winid);
/* NOTE: not working for some reason...
    if(winid)
        gdk_event_send_client_message((GdkEvent *)&event, winid);*/
    
    gdk_event_send_clientmessage_toall((GdkEvent *)&event);
    return 0;
}
