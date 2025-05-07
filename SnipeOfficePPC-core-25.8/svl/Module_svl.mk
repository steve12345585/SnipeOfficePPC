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

$(eval $(call gb_Module_Module,svl))

$(eval $(call gb_Module_add_targets,svl,\
    AllLangResTarget_svl \
    Library_fsstorage \
    Library_passwordcontainer \
    Library_svl \
    Package_inc \
))

$(eval $(call gb_Module_add_check_targets,svl,\
	CppunitTest_svl_lngmisc \
))
#TODO: CppunitTest_svl_urihelper depends on ucb, can only be added once svl is
# in tail build
#FIXME: fails on MSVC

ifneq ($(OS),WNT)
$(eval $(call gb_Module_add_subsequentcheck_targets,svl,\
	CppunitTest_svl_urihelper \
    JunitTest_svl_complex \
))
endif

#todo: dde platform dependent
#todo: package_inc
#todo: map file

# vim: set noet sw=4 ts=4:
