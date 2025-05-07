# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2011 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************


$(eval $(call gb_Library_Library,qstart_gtk))

$(eval $(call gb_Library_set_include,qstart_gtk,\
    $$(INCLUDE) \
    -I$(SRCDIR)/sfx2/inc \
    -I$(SRCDIR)/sfx2/inc/sfx2 \
))

$(eval $(call gb_Library_use_api,qstart_gtk,\
	udkapi \
	offapi \
))

$(eval $(call gb_Library_add_defs,qstart_gtk,\
    -DENABLE_QUICKSTART_APPLET \
))

ifeq ($(ENABLE_SYSTRAY_GTK),TRUE)
$(eval $(call gb_Library_add_defs,qstart_gtk,\
    -DENABLE_SYSTRAY_GTK \
))
endif

$(eval $(call gb_Library_use_libraries,qstart_gtk,\
    comphelper \
    cppu \
    cppuhelper \
    fwe \
    i18nisolang1 \
    sal \
    sax \
    sb \
    sot \
    svl \
    svt \
    tk \
    tl \
    ucbhelper \
    utl \
    vcl \
    sfx \
    $(gb_STDLIBS) \
))

$(eval $(call gb_Library_use_externals,qstart_gtk,\
	gtk \
	libxml2 \
))

$(eval $(call gb_Library_add_exception_objects,qstart_gtk,\
    sfx2/source/appl/shutdowniconunx \
))

# vim: set noet sw=4 ts=4:
