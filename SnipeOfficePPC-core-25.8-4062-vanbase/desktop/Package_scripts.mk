# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
# Version: MPL 1.1 / GPLv3+ / LGPLv3+
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License or as specified alternatively below. You may obtain a copy of
# the License at http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# Major Contributor(s):
# Copyright (C) 2011 Red Hat, Inc., David Tardon <dtardon@redhat.com>
#  (initial developer)
#
# All Rights Reserved.
#
# For minor contributions see the git repository.
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 3 or later (the "GPLv3+"), or
# the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
# in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
# instead of those above.

$(eval $(call gb_Package_Package,desktop_scripts,$(SRCDIR)/desktop/scripts))

ifeq ($(OS),WNT)

$(eval $(call gb_Package_add_file,desktop_scripts,bin/ure-link,ure-link))

else

$(eval $(call gb_Package_add_file,desktop_scripts,bin/gdbtrace,gdbtrace))
$(eval $(call gb_Package_add_file,desktop_scripts,bin/mozwrapper,mozwrapper.sh))
$(eval $(call gb_Package_add_file,desktop_scripts,bin/sbase,sbase.sh))
$(eval $(call gb_Package_add_file,desktop_scripts,bin/scalc,scalc.sh))
$(eval $(call gb_Package_add_file,desktop_scripts,bin/sdraw,sdraw.sh))
$(eval $(call gb_Package_add_file,desktop_scripts,bin/simpress,simpress.sh))
$(eval $(call gb_Package_add_file,desktop_scripts,bin/smaster,smaster.sh))
$(eval $(call gb_Package_add_file,desktop_scripts,bin/smath,smath.sh))
$(eval $(call gb_Package_add_file,desktop_scripts,bin/sweb,sweb.sh))
$(eval $(call gb_Package_add_file,desktop_scripts,bin/swriter,swriter.sh))
$(eval $(call gb_Package_add_file,desktop_scripts,bin/unoinfo,unoinfo.sh))
$(eval $(call gb_Package_add_file,desktop_scripts,bin/unopkg,unopkg.sh))

endif

# vim: set ts=4 sw=4 et:
