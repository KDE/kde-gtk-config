#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2023 Fushan Wen <qydwhotmail@gmail.com>
# SPDX-License-Identifier: MIT

import threading

import gi

gi.require_version('Gtk', '3.0')
from gi.repository import GLib, Gtk


class Gtk3MainThread(threading.Thread):

    def __init__(self) -> None:
        self.loop = GLib.MainLoop()
        self.quit_timer = threading.Timer(60, Gtk.main_quit)
        super().__init__()

    def run(self) -> None:
        self.quit_timer.start()
        Gtk.main()

    def quit(self) -> None:
        self.quit_timer.cancel()
        Gtk.main_quit()


class ColorButton(Gtk.Button):
    """
    The button is used to monitor theme changes
    """

    def __init__(self) -> None:
        super().__init__(label="Colorful")

        style_context = self.get_style_context()
        self.color = style_context.get_background_color(Gtk.StateFlags.ACTIVE)
        self.connect("style_updated", self.on_style_updated)

    def on_style_updated(self, widget) -> None:
        style_context = self.get_style_context()
        self.color = style_context.get_background_color(Gtk.StateFlags.ACTIVE)


class ColorWindow(Gtk.Window):
    """
    A window is required to make style_updated signal work
    """

    def __init__(self):
        super().__init__(title="ColorWindow")

        # Create a button
        self.button = ColorButton()
        self.add(self.button)
