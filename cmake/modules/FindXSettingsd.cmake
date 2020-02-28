# - Find XSettingsd
# This module defines the following variables:
#
#  XSettingsd_FOUND - true if found
#  XSettingsd_PATH - path to the bin (only when found)
#
# Copyright (c) 2020 Mikhail Zolotukhin <zomial@protonmail.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

find_program(XSettingsd_PATH "xsettingsd")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XSettingsd
    FOUND_VAR XSettingsd_FOUND
    REQUIRED_VARS XSettingsd_PATH
)
mark_as_advanced(XSettingsd_PATH)

