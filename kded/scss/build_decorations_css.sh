#!/bin/sh

# Copyright (C) 2019 Mikhail Zolotukhin <zomial@protonmail.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License or (at your option) version 3 or any later version
# accepted by the membership of KDE e.V. (or its successor approved
# by the membership of KDE e.V.), which shall act as a proxy
# defined in Section 14 of version 3 of the license.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Usage: build_sass <source-directory> <target-directory> <include-directory>
build_sass() {
  if command -v sassc >/dev/null 2>&1; then
      sassc -I "$3" "$1" "$2"
  else
      sass -I "$3" --cache-location /tmp/sass-cache "$1" "$2"
  fi
}

# Usage: build_and_install_decorations_css <target-directory>
build_and_install_decorations_css() {
  BUILD_DIR="$(mktemp -d)"
  INSTALL_DIR="$1"

  # Build
  build_sass "window_decorations.scss" "${BUILD_DIR}/window_decorations.css"

  # Install
  mkdir -p "${INSTALL_DIR}"
  mv -f "${BUILD_DIR}/window_decorations.css" "${INSTALL_DIR}"
}

build_and_install_decorations_css "$1"
