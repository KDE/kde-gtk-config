/*
 * SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

/*
 * GTK3 only sets the prelight state of a menu bar item while its menu is open,
 * there is no state for the pointer being over an item otherwise. Qt highlights
 * the item as soon as the pointer is over it, so mark it with a style class
 * themes can opt in to.
 */

#include <gtk/gtk.h>

namespace
{
constexpr const char *hoverClass = "kde-menubar-hover";
const GdkRGBA ProbeColor = {1, 0, 1, 1};
GtkStyleContext *probeContext = nullptr;

GtkWidget *menuBarItem(const GValue *value)
{
    GObject *object = G_OBJECT(g_value_get_object(value));
    if (!GTK_IS_MENU_ITEM(object)) {
        return nullptr;
    }
    GtkWidget *widget = GTK_WIDGET(object);
    return GTK_IS_MENU_BAR(gtk_widget_get_parent(widget)) ? widget : nullptr;
}

bool hasAppStyle(GtkWidget *menuBar)
{
    // set_path copies the path, so the cached context is always restyled
    gtk_style_context_set_path(probeContext, gtk_widget_get_path(menuBar));

    GdkRGBA *color = nullptr;
    gtk_style_context_get(probeContext, GTK_STATE_FLAG_NORMAL, GTK_STYLE_PROPERTY_BACKGROUND_COLOR, &color, nullptr);
    const bool result = !color || !gdk_rgba_equal(color, &ProbeColor);
    gdk_rgba_free(color);
    return result;
}

void clearHover(GtkWidget *item)
{
    gtk_style_context_remove_class(gtk_widget_get_style_context(item), hoverClass);
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

    GtkCssProvider *probe = gtk_css_provider_new();
    gtk_css_provider_load_from_data(probe, "menubar { background-color: rgb(255, 0, 255); }", -1, nullptr);
    probeContext = gtk_style_context_new();
    gtk_style_context_add_provider(probeContext, GTK_STYLE_PROVIDER(probe), GTK_STYLE_PROVIDER_PRIORITY_SETTINGS + 1);
    g_object_unref(probe);

    GSignalEmissionHook pointerOver = [](GSignalInvocationHint *, guint, const GValue *values, gpointer) -> gboolean {
        GtkWidget *item = menuBarItem(&values[0]);
        if (item && gtk_widget_is_sensitive(item) && !gtk_menu_shell_get_selected_item(GTK_MENU_SHELL(gtk_widget_get_parent(item)))
            && !hasAppStyle(gtk_widget_get_parent(item))) {
            gtk_style_context_add_class(gtk_widget_get_style_context(item), hoverClass);
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
