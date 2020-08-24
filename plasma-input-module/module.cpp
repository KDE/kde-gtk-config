/*
 * SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QDebug>
#include <QDir>
#include <list>

#include <gdk/gdkwayland.h>
#include <gtk/gtk.h>
#include <gtk/gtkimmodule.h>

#include <dlfcn.h>

#include "gtkimcontextwrapper.h"
#include "conf.h"

// function signatures for symbols we expect to be available in other IM module plugins
using IMModuleInitPtr = void (*)(GTypeModule *);
using IMModuleExitPtr = void (*)();
using IMModuleCreatePtr = GtkIMContext *(*)(const gchar *);
using IMModuleListPtr = void (*)(const GtkIMContextInfo ***, gint *);

const char *wrapPrefix = "plasma-";

class IMModule
{
    void *wrappedIMModuleHandle;

public:
    IMModuleInitPtr init;
    IMModuleExitPtr exit;
    IMModuleCreatePtr create;
    IMModuleListPtr list;
    QString wrappedIMmodulePath;

    explicit IMModule(const char *path)
    {
        wrappedIMmodulePath = path;
        wrappedIMModuleHandle = dlopen(path, RTLD_LAZY);
        init = getSymbol<IMModuleInitPtr>("im_module_init");
        exit = getSymbol<IMModuleExitPtr>("im_module_exit");
        create = getSymbol<IMModuleCreatePtr>("im_module_create");
        list = getSymbol<IMModuleListPtr>("im_module_list");
    }
    ~IMModule()
    {
        dlclose(wrappedIMModuleHandle);
    }
    template<typename T> T getSymbol(const char *name)
    {
        auto symbol = dlsym(wrappedIMModuleHandle, name);
        return (T)symbol;
    }
};

std::list<IMModule *> findOtherIMModules()
{
    static std::list<IMModule *> handles;
    static bool initted = false;
    if (!initted) {
        initted = true;
        QDir directory(GTK_IMMODULE_PATH);
        QStringList files = directory.entryList(QStringList() << "*.so");
        for (const auto& item : files) {
            if (item.contains("im-kwrapper")) {
                continue;
            }
            if (!GDK_IS_WAYLAND_DISPLAY(gdk_display_get_default())) {
                if (item.contains("wayland")) {
                    continue;
                }
            }
            handles.push_back(new IMModule((GTK_IMMODULE_PATH + QStringLiteral("/") + item).toStdString().c_str()));
        }
    }
    return handles;
}

extern "C" {

G_MODULE_EXPORT const gchar *g_module_check_init(GModule *module)
{
    return NULL;
}

G_MODULE_EXPORT void im_module_init(GTypeModule *module)
{
    for (auto handle : findOtherIMModules()) {
        handle->init(module);
    }
    RegisterType(module);
}

G_MODULE_EXPORT void im_module_exit()
{
    for (auto handle : findOtherIMModules()) {
        handle->exit();
    }
}

G_MODULE_EXPORT GtkIMContext *im_module_create(const gchar *context_id)
{
    auto contextIDStdString = std::string(context_id);
    if (contextIDStdString == "plasma-nothing") {
        return NewContext(gtk_im_context_simple_new());
    }
    auto contextIDQString = QString::fromStdString(contextIDStdString);
    auto prefixQString = QString::fromStdString(std::string(wrapPrefix));
    if (contextIDQString.startsWith(prefixQString)) {
        contextIDQString = contextIDQString.remove(prefixQString);
    }
    for (auto handle : findOtherIMModules()) {
        auto item = handle->create(contextIDQString.toStdString().c_str());
        if (item != nullptr) {
            return NewContext(item);
        }
    }
    return nullptr;
}

struct Info {
    std::string contextID;
    std::string contextName;
    std::string domain;
    std::string domainDirname;
    std::string defaultLocales;
};

// this function has intentional memory leaks, due to this:
// Normally for GTK modules, this function changes the passed in pointer to a pointer
// to a pointer to point to a pointer to a pointer of data that's compiled into the binary,
// but since we generate data at runtime, we have to intentionally leak it for the same effect
// of "this will never ever EVER ever change"
G_MODULE_EXPORT void im_module_list(const GtkIMContextInfo ***contexts, gint *n_contexts)
{
    static std::vector<Info> readVector;
    static bool initted = false;
    static auto returnVector = new std::vector<const GtkIMContextInfo *>();

    if (!initted) {
        auto handles = findOtherIMModules();
        for (auto handle : handles) {
            handle->list(contexts, n_contexts);
            for (int i = 0; i < *n_contexts; i++) {
                auto ctx = *(*contexts[i]);
                auto info = Info {
                    .contextID = ctx.context_id,
                    .contextName = ctx.context_name,
                    .domain = ctx.domain,
                    .domainDirname = ctx.domain_dirname,
                    .defaultLocales = ctx.default_locales
                };

                info.contextID = "plasma-" + info.contextID;
                readVector.push_back(info);
            }
        }
        readVector.push_back(Info {
            .contextID = "plasma-nothing",
            .contextName = "plasma-nothing",
            .domain = "gtk30",
            .domainDirname = "/usr/share/locale",
            .defaultLocales = "*",
        });

        for (const auto &item : readVector) {
            auto info = new GtkIMContextInfo;
            info->context_id = item.contextID.c_str();
            info->context_name = item.contextName.c_str();
            info->domain = item.domain.c_str();
            info->domain_dirname = item.domainDirname.c_str();
            info->default_locales = item.defaultLocales.c_str();
            returnVector->push_back(info);
        }

        initted = true;
    }

    *contexts = returnVector->data();
    *n_contexts = returnVector->size();
}

}
