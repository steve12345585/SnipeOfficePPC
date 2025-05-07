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
# Copyright (C) 2011 Red Hat, Inc., Caolán McNamara <caolanm@redhat.com>
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

$(eval $(call gb_Library_Library,hwp))

$(eval $(call gb_Library_set_include,hwp,\
    $$(INCLUDE) \
))

$(eval $(call gb_Library_use_api,hwp,\
    offapi \
    udkapi \
))

$(eval $(call gb_Library_use_libraries,hwp,\
    comphelper \
    cppu \
    cppuhelper \
    sal \
    $(gb_STDLIBS) \
))

$(eval $(call gb_Library_use_external,hwp,zlib))

ifeq ($(OS),WNT)

$(eval $(call gb_Library_use_libraries,hwp,\
    ole32 \
    uuid \
))

endif

$(eval $(call gb_Library_set_componentfile,hwp,hwpfilter/source/hwp))

$(eval $(call gb_Library_add_exception_objects,hwp,\
    hwpfilter/source/attributes \
    hwpfilter/source/cspline \
    hwpfilter/source/fontmap \
    hwpfilter/source/formula \
    hwpfilter/source/grammar \
    hwpfilter/source/hbox \
    hwpfilter/source/hcode \
    hwpfilter/source/hfont \
    hwpfilter/source/hgzip \
    hwpfilter/source/hinfo \
    hwpfilter/source/hiodev \
    hwpfilter/source/hpara \
    hwpfilter/source/hstream \
    hwpfilter/source/hstyle \
    hwpfilter/source/htags \
    hwpfilter/source/hutil \
    hwpfilter/source/hwpeq \
    hwpfilter/source/hwpfile \
    hwpfilter/source/hwpread \
    hwpfilter/source/hwpreader \
    hwpfilter/source/lexer \
    hwpfilter/source/mzstring \
    hwpfilter/source/solver \
))

# vim: set noet sw=4 ts=4:
