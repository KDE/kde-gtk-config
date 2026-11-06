/*
 * SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

/*
 * GTK3 only sets the prelight state of a menu bar item while its menu is open,
 * there is no state for the pointer being over an item otherwise. Qt highlights
 * the item as soon as the pointer is over it, so set the prelight state ourselves.
 */

#include <gtk/gtk.h>

namespace
{
GtkWidget *menuBarItem(const GValue *value)
{
    GObject *object = G_OBJECT(g_value_get_object(value));
    if (!GTK_IS_MENU_ITEM(object)) {
        return nullptr;
    }
    GtkWidget *widget = GTK_WIDGET(object);
    return GTK_IS_MENU_BAR(gtk_widget_get_parent(widget)) ? widget : nullptr;
}

void clearHover(GtkWidget *item)
{
    // Prelight of the selected item is GTK's own: it marks the open menu
    if (item != gtk_menu_shell_get_selected_item(GTK_MENU_SHELL(gtk_widget_get_parent(item)))) {
        gtk_widget_unset_state_flags(item, GTK_STATE_FLAG_PRELIGHT);
    }
}

void addHook(const char *signal, GType type, GSignalEmissionHook hook)
{
    g_signal_add_emission_hook(g_signal_lookup(signal, type), 0, hook, nullptr, nullptr);
}
}

extern "C" __attribute__((visibility("default"))) void gtk_module_init(gint *, gchar ***)
{
    // Signals can only be looked up once their classes exist
    g_type_class_ref(GTK_TYPE_WIDGET);
    g_type_class_ref(GTK_TYPE_MENU_ITEM);

    GSignalEmissionHook pointerOver = [](GSignalInvocationHint *, guint, const GValue *values, gpointer) -> gboolean {
        // While a menu is open GTK selects items itself and crossing events are not reliable because of the grab
        GtkWidget *item = menuBarItem(&values[0]);
        if (item && gtk_widget_is_sensitive(item) && !gtk_menu_shell_get_selected_item(GTK_MENU_SHELL(gtk_widget_get_parent(item)))) {
            gtk_widget_set_state_flags(item, GTK_STATE_FLAG_PRELIGHT, FALSE);
        }
        return TRUE;
    };
    addHook("enter-notify-event", GTK_TYPE_WIDGET, pointerOver);
    // There is no new enter event when a menu is closed with the pointer still over its item
    addHook("motion-notify-event", GTK_TYPE_WIDGET, pointerOver);

    addHook("leave-notify-event", GTK_TYPE_WIDGET, [](GSignalInvocationHint *, guint, const GValue *values, gpointer) -> gboolean {
        GtkWidget *item = menuBarItem(&values[0]);
        if (item) {
            clearHover(item);
        }
        return TRUE;
    });

    addHook("select", GTK_TYPE_MENU_ITEM, [](GSignalInvocationHint *, guint, const GValue *values, gpointer) -> gboolean {
        // The item that was under the pointer may not get a leave event once the menu grabs input
        GtkWidget *item = menuBarItem(&values[0]);
        if (item) {
            const GtkCallback clear = [](GtkWidget *widget, gpointer) {
                clearHover(widget);
            };
            gtk_container_foreach(GTK_CONTAINER(gtk_widget_get_parent(item)), clear, nullptr);
        }
        return TRUE;
    });
}
