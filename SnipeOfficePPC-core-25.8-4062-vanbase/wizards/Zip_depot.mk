# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2011 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of SnipeOffice.
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

$(eval $(call gb_Zip_Zip,basicsrvdepot,$(SRCDIR)/wizards/source/depot))

$(eval $(call gb_Zip_add_files,basicsrvdepot,\
	CommonLang.xba \
	Currency.xba \
	Depot.xba \
	dialog.xlb \
	Dialog2.xdl \
	Dialog3.xdl \
	Dialog4.xdl \
	Internet.xba \
	Lang_de.xba \
	Lang_en.xba \
	Lang_es.xba \
	Lang_fr.xba \
	Lang_it.xba \
	Lang_ja.xba \
	Lang_ko.xba \
	Lang_sv.xba \
	Lang_tw.xba \
	Lang_zh.xba \
	script.xlb \
	tools.xba \
))

# vim: set noet sw=4 ts=4:
