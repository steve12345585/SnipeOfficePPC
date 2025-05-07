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

$(eval $(call gb_JunitTest_JunitTest,dbaccess_complex))

$(eval $(call gb_JunitTest_set_defs,dbaccess_complex,\
	$$(DEFS) \
	-Dorg.openoffice.test.arg.tdoc=$(SRCDIR)/dbaccess/qa/dbaccess \
))

$(eval $(call gb_JunitTest_add_sourcefiles,dbaccess_complex,\
	dbaccess/qa/complex/dbaccess/ApplicationController \
	dbaccess/qa/complex/dbaccess/Beamer \
	dbaccess/qa/complex/dbaccess/DataSource \
	dbaccess/qa/complex/dbaccess/DatabaseDocument \
	dbaccess/qa/complex/dbaccess/Parser \
	dbaccess/qa/complex/dbaccess/PropertyBag \
	dbaccess/qa/complex/dbaccess/Query \
	dbaccess/qa/complex/dbaccess/QueryInQuery \
	dbaccess/qa/complex/dbaccess/RowSet \
	dbaccess/qa/complex/dbaccess/SingleSelectQueryComposer \
	dbaccess/qa/complex/dbaccess/UISettings \
	dbaccess/qa/complex/dbaccess/CopyTableWizard \
	dbaccess/qa/complex/dbaccess/CRMBasedTestCase \
	dbaccess/qa/complex/dbaccess/CopyTableInterActionHandler \
	dbaccess/qa/complex/dbaccess/DatabaseApplication \
	dbaccess/qa/complex/dbaccess/FileHelper \
	dbaccess/qa/complex/dbaccess/RowSetEventListener \
	dbaccess/qa/complex/dbaccess/TestCase \
))

$(eval $(call gb_JunitTest_use_jars,dbaccess_complex,\
	$(OUTDIR)/bin/OOoRunner.jar \
	$(OUTDIR)/bin/ridl.jar \
	$(OUTDIR)/bin/test.jar \
	$(OUTDIR)/bin/unoil.jar \
	$(OUTDIR)/bin/jurt.jar \
	$(OUTDIR)/bin/ConnectivityTools.jar \
))

$(eval $(call gb_JunitTest_add_classes,dbaccess_complex,\
	complex.dbaccess.Beamer \
	complex.dbaccess.PropertyBag \
))
#	complex.dbaccess.DatabaseDocument \
# deactivated since sb123;
# apparently most of these fail because OOo does not find JVM?
#	complex.dbaccess.ApplicationController \
	complex.dbaccess.DataSource \
	complex.dbaccess.Parser \
	complex.dbaccess.Query \
	complex.dbaccess.QueryInQuery \
	complex.dbaccess.RowSet \
	complex.dbaccess.SingleSelectQueryComposer \
	complex.dbaccess.UISettings \
	complex.dbaccess.CopyTableWizard \

# vim: set noet sw=4 ts=4:
