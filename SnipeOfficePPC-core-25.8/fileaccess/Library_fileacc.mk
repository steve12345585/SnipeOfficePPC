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
# Copyright (C) 2010 Red Hat, Inc., David Tardon <dtardon@redhat.com>
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

$(eval $(call gb_Library_Library,fileacc))

$(eval $(call gb_Library_set_include,fileacc,\
    $$(INCLUDE) \
))

$(eval $(call gb_Library_use_api,fileacc,\
    offapi \
    udkapi \
))

$(eval $(call gb_Library_use_libraries,fileacc,\
    cppu \
    cppuhelper \
    sal \
    tl \
    ucbhelper \
    utl \
    $(gb_STDLIBS) \
))

$(eval $(call gb_Library_set_componentfile,fileacc,fileaccess/source/fileacc))

$(eval $(call gb_Library_add_exception_objects,fileacc,fileaccess/source/FileAccess))

# vim: set noet sw=4 ts=4:
