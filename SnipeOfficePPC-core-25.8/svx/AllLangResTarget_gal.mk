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

$(eval $(call gb_AllLangResTarget_AllLangResTarget,gal))

$(eval $(call gb_AllLangResTarget_set_reslocation,gal,svx))

$(eval $(call gb_AllLangResTarget_add_srs,gal,svx/gal))

$(eval $(call gb_SrsTarget_SrsTarget,svx/gal))

$(eval $(call gb_SrsTarget_use_packages,svx/gal,\
	editeng_inc \
	sfx2_inc \
	svl_inc \
	svx_inc \
))

$(eval $(call gb_SrsTarget_set_include,svx/gal,\
    $$(INCLUDE) \
    -I$(SRCDIR)/svx/source/gallery2 \
    -I$(SRCDIR)/svx/inc/ \
))

$(eval $(call gb_SrsTarget_add_files,svx/gal,\
    svx/source/gallery2/gallery.src \
    svx/source/gallery2/galtheme.src \
))

# vim: set noet sw=4 ts=4:
