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

$(eval $(call gb_JunitTest_JunitTest,reportdesign_complex))

$(eval $(call gb_JunitTest_set_defs,reportdesign_complex,\
	$$(DEFS) \
	-Dorg.openoffice.test.arg.tdoc=$(SRCDIR)/reportdesign/qa/complex/reportdesign/test_documents \
))

$(eval $(call gb_JunitTest_add_sourcefiles,reportdesign_complex,\
	reportdesign/qa/complex/reportdesign/ReportDesignerTest \
	reportdesign/qa/complex/reportdesign/TestDocument \
	reportdesign/qa/complex/reportdesign/FileURL \
))

$(eval $(call gb_JunitTest_use_jars,reportdesign_complex,\
	OOoRunner \
	ridl \
	test \
	unoil \
	jurt \
))

$(eval $(call gb_JunitTest_add_classes,reportdesign_complex,\
	complex.reportdesign.ReportDesignerTest \
))

# vim: set noet sw=4 ts=4:
