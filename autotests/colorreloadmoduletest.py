#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2023 Fushan Wen <qydwhotmail@gmail.com>
# SPDX-License-Identifier: MIT

import os
import shutil
import subprocess
import time
import unittest
from typing import Final

from gi.repository import Gio, GLib
from resources.gtk3_window import ColorWindow, Gtk3MainThread

KDE_VERSION: Final = 6


def name_has_owner(session_bus: Gio.DBusConnection, name: str) -> bool:
    """
    Whether the given name is available on session bus
    """
    SERVICE_NAME: Final = "org.freedesktop.DBus"
    ROOT_OBJECT_PATH: Final = "/"
    INTERFACE_NAME: Final = SERVICE_NAME
    message: Gio.DBusMessage = Gio.DBusMessage.new_method_call(SERVICE_NAME, ROOT_OBJECT_PATH, INTERFACE_NAME, "NameHasOwner")
    message.set_body(GLib.Variant("(s)", [name]))
    reply, _ = session_bus.send_message_with_reply_sync(message, Gio.DBusSendMessageFlags.NONE, 1000)
    return reply and reply.get_signature() == 'b' and reply.get_body().get_child_value(0).get_boolean()


def notify_color_change() -> None:
    time.sleep(5)
    # The signature is equivalent to QHash<QString, QByteArrayList>
    changed_properties = GLib.Variant('a{saay}', {
        "General": [b"ColorScheme", b"AccentColor"],
    })
    session_bus: Gio.DBusConnection = Gio.bus_get_sync(Gio.BusType.SESSION)
    Gio.DBusConnection.emit_signal(session_bus, None, "/kdeglobals", "org.kde.kconfig.notify", "ConfigChanged", GLib.Variant.new_tuple(changed_properties))
    time.sleep(5)


class ColorReloadModuleTest(unittest.TestCase):
    """
    The ColorReloadModule is used to monitor changes in gtk-3.0/colors.css and reload the current theme in GTK3 apps
    """

    kded: subprocess.Popen | None = None
    main_loop: Gtk3MainThread

    @classmethod
    def setUpClass(cls) -> None:
        session_bus: Gio.DBusConnection = Gio.bus_get_sync(Gio.BusType.SESSION)
        if not name_has_owner(session_bus, f"org.kde.kded{KDE_VERSION}"):
            cls.kded = subprocess.Popen([f"kded{KDE_VERSION}"])
            cls.addClassCleanup(cls.kded.kill)
            kded_started: bool = False
            for _ in range(10):
                if name_has_owner(session_bus, f"org.kde.kded{KDE_VERSION}"):
                    kded_started = True
                    break
                print(f"waiting for kded{KDE_VERSION} to appear on the DBus session")
                time.sleep(1)
            assert kded_started

        kded_reply: GLib.Variant = session_bus.call_sync(f"org.kde.kded{KDE_VERSION}", "/kded", f"org.kde.kded{KDE_VERSION}", "loadModule", GLib.Variant("(s)", ["gtkconfig"]), GLib.VariantType("(b)"), Gio.DBusSendMessageFlags.NONE, 1000)
        assert kded_reply.get_child_value(0).get_boolean(), "gtkconfig module is not available"

        cls.main_loop = Gtk3MainThread()
        cls.main_loop.start()

    @classmethod
    def tearDownClass(cls) -> None:
        cls.main_loop.quit()

    def test_active_color(self) -> None:
        win = ColorWindow()
        win.show_all()

        shutil.copy(os.path.join(os.path.dirname(os.path.abspath(__file__)), "resources", "kdeglobals.pink"), os.path.join(GLib.get_user_config_dir(), "kdeglobals"))
        notify_color_change()
        # From [Colors:Button][ForegroundActive] in kdeglobals
        self.assertAlmostEqual(win.button.color.red * 255, 233)
        self.assertAlmostEqual(win.button.color.green * 255, 58)
        self.assertAlmostEqual(win.button.color.blue * 255, 154)

        shutil.copy(os.path.join(os.path.dirname(os.path.abspath(__file__)), "resources", "kdeglobals.green"), os.path.join(GLib.get_user_config_dir(), "kdeglobals"))
        notify_color_change()

        self.assertAlmostEqual(win.button.color.red * 255, 61)
        self.assertAlmostEqual(win.button.color.green * 255, 212)
        self.assertAlmostEqual(win.button.color.blue * 255, 37)

        win.close()


if __name__ == "__main__":
    if "KDECI_BUILD" in os.environ:
        os.environ["KDE_COLOR_SCHEME_PATH"] = os.path.join(GLib.get_user_config_dir(), "kdeglobals")
        os.environ["GTK_EXE_PREFIX"] = os.environ["CMAKE_PREFIX_PATH"].split(":")[0]
        print("CMAKE_PREFIX_PATH", os.environ["CMAKE_PREFIX_PATH"], os.environ["GTK_EXE_PREFIX"])
        os.makedirs(os.path.join(os.environ["GTK_EXE_PREFIX"], "lib", "gtk-3.0", "modules"), exist_ok=True)
        shutil.copy(os.path.join(os.environ["GTK_EXE_PREFIX"], "lib64", "gtk-3.0", "modules", "libcolorreload-gtk-module.so"), os.path.join(os.environ["GTK_EXE_PREFIX"], "lib", "gtk-3.0", "modules", "libcolorreload-gtk-module.so"))
    unittest.main()
