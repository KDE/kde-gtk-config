/* KDE GTK Configuration Module
 * 
 * Copyright 2011 Aleix Pol Gonzalez <aleixpol@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
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
