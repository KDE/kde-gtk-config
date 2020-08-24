/*
 * SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QDebug>

#include <KConfigGroup>
#include <KSharedConfig>

#include <gdk/gdkx.h>

#include <functional>

#include <plasmakeydata.h>

#include "gtkimcontextwrapper.h"

extern "C" {

// These are the dangerous members of the KWrapperContext
// in regards to memory management.
// Bugs, crashes, memory leaks, and security flaws lie here.
struct KWrapperContextDangerous {
    // Both std::string and QString have APIs unique to each of them that we use
    // enough in order to warrant storing them as both formats here to prevent
    // needless conversions.
    std::string currentSString = std::string();
    std::string previousAsString = std::string();
    QString previousAsQString = QString();
    QString currentQString = QString();
    KSharedConfig::Ptr config = KSharedConfig::openConfig( QStringLiteral("kcminputrc") );
    KConfigGroup keyboard = KConfigGroup(config, "Keyboard");
};

struct KWrapperContext;

// This holds the needed data for a clicked connection on a GtkButton
struct KWrapperContextButtonData {
    KWrapperContext *parent;
    int kind;
};

struct KWrapperContext {
    GtkIMContext parent;
    GtkIMContext *wrapped;
    GdkWindow *window;
    GdkRectangle rect;
    bool isX11;

    GtkWidget *popupWindow = nullptr;
    bool popupWindowShown = false;

    void repositionWindow(GdkRectangle *area) const
    {
        if (popupWindow != nullptr) {
            auto x = area->x;
            auto y = area->y;
            if (this->isX11) {
                GdkRectangle frameExtents;
                gdk_window_get_frame_extents(this->window, &frameExtents);
                x += rect.x;
                y += rect.y;
            }
            gtk_window_move((GtkWindow *)popupWindow, x, y);
        }
    }

    void constructWindow(GdkRectangle *area)
    {
        if (popupWindow == nullptr) {
            popupWindow = gtk_window_new(GTK_WINDOW_POPUP);

            gtk_widget_realize(popupWindow);

            auto handle = gtk_widget_get_window(popupWindow);
            gdk_window_set_transient_for(handle, window);

            gtk_window_set_default_size((GtkWindow *)popupWindow, 9, 9);
            repositionWindow(area);

            gtk_widget_show_all(popupWindow);
            popupWindowShown = true;
        }
    }

    void showWindow()
    {
        if (window != nullptr && !popupWindowShown) {
            popupWindowShown = true;
            gtk_widget_show_all(popupWindow);
        }
    }

    void hideWindow()
    {
        if (window != nullptr && popupWindowShown) {
            popupWindowShown = false;
            gtk_widget_hide(popupWindow);
        }
    }

    GtkWidget *previousBox;

    GtkWidget* buildKey(const QString& key, int i, bool caps)
    {
        auto boxlet = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        auto topLabel = gtk_label_new((caps ? key.toUpper() : key).toLocal8Bit().data());
        auto bottomLabel = gtk_button_new_with_label(QString::number(i < 10 ? i : 0).toLocal8Bit().data());

        g_signal_connect_data (bottomLabel, "clicked", (GCallback)(void(*)(GtkButton*,KWrapperContextButtonData*))([](GtkButton *button, KWrapperContextButtonData *data) {
            bool ret;
            g_signal_emit_by_name(data->parent, "delete-surrounding", -1, 1, &ret);
            Q_UNUSED(ret) // GTK will give a warning if we pass in a null pointer as the return value, but we don't care about it
            g_signal_emit_by_name(data->parent, "commit", (data->parent->popupIsHigher ? KeyData::KeyMappings[data->parent->dangerous->previousAsQString.toLower()][data->kind].toUpper() : KeyData::KeyMappings[data->parent->dangerous->previousAsQString.toLower()][data->kind]).toLocal8Bit().data());
            data->parent->dangerous->previousAsQString.clear();
            data->parent->dangerous->previousAsString.clear();
            data->parent->previousType = (GdkEventType)(0);
            data->parent->previousVal = (guint)(0);
            data->parent->previousTime = 0;
            data->parent->keyHasUp = false;
            data->parent->hasRepeated = false;
            data->parent->hideWindow();
        }), new KWrapperContextButtonData { this, i-1 }, NULL, (GConnectFlags) 0);

        auto styleContext = gtk_widget_get_style_context(bottomLabel);
        gtk_style_context_add_class(styleContext, "keycap");
        gtk_style_context_remove_class(styleContext, "text-button");

        gtk_box_pack_start((GtkBox *)boxlet, topLabel, false, false, 0);
        gtk_box_pack_start((GtkBox *)boxlet, bottomLabel, false, false, 0);

        return boxlet;
    }

    void setWindowContents(const QList<QString> &keys, bool caps)
    {
        GtkWidget *currentBox = nullptr;
        currentBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
        int i = 1;
        for (const auto& key : keys) {
            gtk_box_pack_start((GtkBox *)currentBox, buildKey(key, i, caps), false, false, 0);

            i++;
        }
        if (previousBox != nullptr) {
            gtk_widget_destroy(previousBox);
        }
        gtk_widget_set_margin_end(currentBox, 6);
        gtk_widget_set_margin_start(currentBox, 6);
        gtk_widget_set_margin_top(currentBox, 6);
        gtk_widget_set_margin_bottom(currentBox, 6);
        gtk_container_add((GtkContainer *)popupWindow, currentBox);
        gtk_widget_show_all(currentBox);
        gtk_window_resize((GtkWindow *)popupWindow, 1, 1);
        previousBox = currentBox;
    }

    GdkEventType previousType = (GdkEventType)(0);
    guint previousVal = (guint)(0);
    guint previousTime = 0;
    bool first = true;
    bool hasRepeated = false;
    bool popupIsHigher = false;
    bool keyHasUp = false;
    bool supportsBack = false;
    bool hasCheckedSupportsBack = false;
    bool ignoreNextOneRelease = false;
    KWrapperContextDangerous* dangerous = nullptr;

    inline bool checkSupportsDeleteSurrounding() {
        if (!hasCheckedSupportsBack) {
            // GTK always returns true even if we don't delete anything
            // This is used to weed out bad implementations of an IM Context
            g_signal_emit_by_name(&parent, "delete-surrounding", 0, 0, &supportsBack);
            hasCheckedSupportsBack = true;
        }

        if (!supportsBack) {
            return false;
        }

        return true;
    }

    inline void clear() {
        previousType = (GdkEventType)(0);
        previousVal = 0;
        dangerous->previousAsString.clear();
        hasRepeated = false;
        keyHasUp = false;
        this->hideWindow();
    }

    inline bool passThrough(GdkEventKey *ev, bool eat) {
        previousType = ev->type;
        previousVal = ev->keyval;
        dangerous->previousAsString = std::string(1, (char)gdk_keyval_to_unicode(ev->keyval));
        this->hideWindow();
        return eat || gtk_im_context_filter_keypress(this->wrapped, ev);
    }

    gboolean filterKeypress(GdkEventKey *ev) {
        dangerous->previousAsQString = QString::fromStdString(dangerous->previousAsString);
        dangerous->currentSString = std::string(1, (char)gdk_keyval_to_unicode(ev->keyval));
        dangerous->currentQString = QString::fromStdString(dangerous->currentSString);

        bool isAccent = dangerous->keyboard.readEntry("KeyRepeat", "accent") == QLatin1String("accent");
        bool isNothing = dangerous->keyboard.readEntry("KeyRepeat", "accent") == QLatin1String("nothing");

        if (!isAccent && !isNothing) {
            return false;
        }

        bool eventIsRepeatedKey = false;
        bool eat = false;

        if (!checkSupportsDeleteSurrounding()) {
            return false;
        }

        if (ev->time <= previousTime) {
            previousTime--;
            return false;
        }
        previousTime = ev->time;

        if (ev->is_modifier) {
            return false;
        }

        // Don't intercept key events that are done while holding control
        // otherwise we'll interrupt stuff like held ctrl-z
        if ((ev->state&GDK_CONTROL_MASK) != 0) {
            return false;
        }

        if (hasRepeated && ev->keyval == GDK_KEY_Escape) {
            previousType = ev->type;
            previousVal = ev->keyval;
            dangerous->previousAsString = std::string(1, (char)gdk_keyval_to_unicode(ev->keyval));
            this->hideWindow();
            return true;
        }

        // this is NOT a keycode we want to handle, let's clean up and let the IM we're wrapping handle it
        if (ev->keyval > 0xf000) {
            clear();
            return gtk_im_context_filter_keypress(this->wrapped, ev);
        }

        if (ev->window != nullptr && this->window == nullptr) {
            this->window = ev->window;
        }

        if (first) {
            first = false;
            return passThrough(ev, eat);
        }

        eventIsRepeatedKey = ((previousType == GDK_KEY_PRESS) && (ev->type == GDK_KEY_PRESS) && (previousVal == ev->keyval));

        if (eventIsRepeatedKey && isNothing) {
            return true;
        }

        if (ev->type == GDK_KEY_RELEASE && ev->keyval == GDK_KEY_1) {
            if (!keyHasUp && !ignoreNextOneRelease) {
                keyHasUp = true;
                return true;
            }
            if (ignoreNextOneRelease) {
                ignoreNextOneRelease = false;
            }
        }

        if (hasRepeated) {
            int choice = -1;

            if (dangerous->previousAsQString.toLower() == dangerous->currentQString.toLower() && !keyHasUp) {
                this->setWindowContents(KeyData::KeyMappings[dangerous->currentQString.toLower()], popupIsHigher);
                this->showWindow();
                return true;
            }
            // keycodes for 0 through 9
            if (ev->keyval < 0x30 || 0x39 < ev->keyval) {
                return passThrough(ev, eat);
            }
            choice = ev->keyval - 0x30;
            if (choice == 0) {
                choice = 10;
            }
            if (KeyData::KeyMappings[dangerous->previousAsQString.toLower()].count() < choice) {
                return passThrough(ev, eat);
            }
            bool ret;
            g_signal_emit_by_name(&this->parent, "delete-surrounding", -1, 1, &ret);
            Q_UNUSED(ret) // GTK will give a warning if we pass in a null pointer as the return value, but we don't care about it
            g_signal_emit_by_name(&this->parent, "commit", (popupIsHigher ? KeyData::KeyMappings[dangerous->previousAsQString.toLower()][choice - 1].toUpper() : KeyData::KeyMappings[dangerous->previousAsQString.toLower()][choice - 1]).toLocal8Bit().data());
            if (ev->keyval == GDK_KEY_1) {
                ignoreNextOneRelease = true;
            }
            if (keyHasUp) {
                previousType = (GdkEventType)(0);
                previousVal = 0;
                keyHasUp = false;
            }
            hasRepeated = false;
            this->hideWindow();
            return true;
        }

        if (eventIsRepeatedKey) {
            if (!KeyData::KeyMappings.contains(dangerous->currentQString.toLower())) {
                eat = true;
                return passThrough(ev, eat);
            }
            this->constructWindow(&this->rect);
            this->setWindowContents(KeyData::KeyMappings[dangerous->currentQString], dangerous->currentQString.isUpper());
            popupIsHigher = dangerous->currentQString.isUpper();
            hasRepeated = true;
            return true;
        }

        return passThrough(ev, eat);
    }
};

#define kThis ((KWrapperContext *)self)

struct KWrapperContextClass {
    GtkIMContextClass parent;
};

static GType KWrapperContextType = 0;
GType KWrapperContextTypeInstance()
{
    if (KWrapperContextType == 0)
        RegisterType(nullptr);
    return KWrapperContextType;
}

void InitClass(KWrapperContextClass *self);
void DeinitClass(KWrapperContextClass *self);
void InitContext(GObject *obj);

void RegisterType(GTypeModule *mod)
{
    const GTypeInfo info = {
        sizeof(KWrapperContextClass),
        nullptr,
        nullptr,
        (GClassInitFunc)InitClass,
        (GClassFinalizeFunc)DeinitClass,
        nullptr,
        sizeof(KWrapperContext),
        0,
        (GInstanceInitFunc)InitContext,
    };
    if (KWrapperContextType == 0) {
        if (mod == nullptr) {
            KWrapperContextType = g_type_register_static(GTK_TYPE_IM_CONTEXT, "KWrapperContext", &info, (GTypeFlags)0);
        } else {
            KWrapperContextType = g_type_module_register_type(mod, GTK_TYPE_IM_CONTEXT, "KWrapperContext", &info, (GTypeFlags)0);
        }
    }
}

void InitClass(KWrapperContextClass *self)
{
    auto IMCClass = (GtkIMContextClass *)self;
    auto GOBClass = (GObjectClass *)self;

    IMCClass->reset = [](GtkIMContext *self) { gtk_im_context_reset(kThis->wrapped); };
    IMCClass->focus_in = [](GtkIMContext *self) { gtk_im_context_focus_in(kThis->wrapped); };
    IMCClass->focus_out = [](GtkIMContext *self) { gtk_im_context_focus_out(kThis->wrapped); };
    IMCClass->filter_keypress = [](GtkIMContext *self, GdkEventKey *ev) -> gboolean {
        return kThis->filterKeypress(ev);
    };
    IMCClass->get_preedit_string = [](GtkIMContext *self, gchar **str, PangoAttrList **attrs, gint *cursorPos) {
        gtk_im_context_get_preedit_string(kThis->wrapped, str, attrs, cursorPos);
    };
    IMCClass->set_client_window = [](GtkIMContext *self, GdkWindow *window) {
        kThis->window = window;
        gtk_im_context_set_client_window(kThis->wrapped, window);
    };
    IMCClass->set_cursor_location = [](GtkIMContext *self, GdkRectangle *area) {
        kThis->rect = *area;
        kThis->repositionWindow(area);
        gtk_im_context_set_cursor_location(kThis->wrapped, area);
    };
    IMCClass->set_use_preedit = [](GtkIMContext *self, gboolean usePreedit) {
        gtk_im_context_set_use_preedit(kThis->wrapped, usePreedit);
    };
    IMCClass->set_surrounding = [](GtkIMContext *self, const gchar *text, gint len, gint cursorIndex) {
        gtk_im_context_set_surrounding(kThis->wrapped, text, len, cursorIndex);
    };
    GOBClass->notify = [](GObject *self, GParamSpec *spec) {
        g_object_notify_by_pspec((GObject*)(kThis->wrapped), spec);
    };
    GOBClass->finalize = [](GObject *self) {
        delete kThis->dangerous;
        g_object_unref(kThis->wrapped);
    };
}

void DeinitClass(KWrapperContextClass *self)
{
}

void InitContext(GObject *obj)
{
}

}

template<typename Ret, typename... Args, typename Callable> void Connect(const char *sig, GtkIMContext *wrap, KWrapperContext *obj, Callable callable)
{
    using functor = Ret (*)(GtkIMContext *, Args..., KWrapperContext *);
    auto ptr = (functor)(callable);
    g_signal_connect(wrap, sig, (GCallback)(ptr), obj);
}

extern "C" {

GtkIMContext *NewContext(GtkIMContext *wrappee)
{
    auto obj = (KWrapperContext *)g_object_new(KWrapperContextTypeInstance(), NULL);
    obj->wrapped = wrappee;
    obj->isX11 = false;
    obj->dangerous = new KWrapperContextDangerous;
    obj->dangerous->config->reparseConfiguration();

    if (GDK_IS_X11_DISPLAY(gdk_display_get_default())) {
        obj->isX11 = true;
    }

    Connect<void, char *>("commit", wrappee, obj, [](GtkIMContext *target, char *str, KWrapperContext *self) {
        g_signal_emit_by_name(self, "commit", str, nullptr);
    });
    Connect<gboolean, int, int>("delete-surrounding", wrappee, obj, [](GtkIMContext *target, int offset, int charCount, KWrapperContext *self) -> gboolean {
        gboolean ret;
        g_signal_emit_by_name(self, "delete-surrounding", offset, charCount, &ret);
        return ret;
    });
    Connect<void>("preedit-changed", wrappee, obj, [](GtkIMContext *target, KWrapperContext *self) {
        g_signal_emit_by_name(self, "preedit-changed");
    });
    Connect<void>("preedit-end", wrappee, obj, [](GtkIMContext *target, KWrapperContext *self) {
        g_signal_emit_by_name(self, "preedit-end");
    });
    Connect<void>("preedit-start", wrappee, obj, [](GtkIMContext *target, KWrapperContext *self) {
        g_signal_emit_by_name(self, "preedit-start");
    });
    Connect<void>("retrieve-surrounding", wrappee, obj, [](GtkIMContext *target, KWrapperContext *self) {
        qDebug() << ((GObject*)(self))->g_type_instance.g_class;
        g_signal_emit_by_name(self, "retrieve-surrounding");
    });

    return (GtkIMContext *)obj;
}

}
