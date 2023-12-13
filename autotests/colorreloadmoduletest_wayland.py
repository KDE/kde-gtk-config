#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2023 Fushan Wen <qydwhotmail@gmail.com>
# SPDX-License-Identifier: MIT

import os
import subprocess
import sys

if __name__ == '__main__':
    environ = os.environ.copy()
    environ["KWIN_WAYLAND_NO_PERMISSION_CHECKS"] = "1"
    environ["GDK_BACKEND"] = "wayland"
    kwin_process = subprocess.Popen(["kwin_wayland", "--virtual", "--no-lockscreen", "--no-global-shortcuts", "--no-kactivities", "--exit-with-session", "python3", os.path.join(os.path.dirname(os.path.abspath(__file__)), "colorreloadmoduletest.py")], env=environ, stdout=sys.stderr, stderr=sys.stderr)

    result: int = 1
    try:
        result = kwin_process.wait(timeout=60)
    except subprocess.TimeoutExpired as e:
        print("Timeout", e)
    finally:
        kwin_process.terminate()

    sys.exit(result)
