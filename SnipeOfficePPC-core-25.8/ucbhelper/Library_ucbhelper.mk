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

$(eval $(call gb_Library_Library,ucbhelper))

$(eval $(call gb_Library_use_package,ucbhelper,ucbhelper_inc))

$(eval $(call gb_Library_set_include,ucbhelper,\
	$$(INCLUDE) \
	-I$(SRCDIR)/ucbhelper/inc \
))

$(eval $(call gb_Library_add_defs,ucbhelper,\
	-DUCBHELPER_DLLIMPLEMENTATION \
))

$(eval $(call gb_Library_use_api,ucbhelper,\
    offapi \
    udkapi \
))

$(eval $(call gb_Library_use_libraries,ucbhelper,\
	cppu \
	cppuhelper \
	sal \
	salhelper \
    $(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,ucbhelper,\
	ucbhelper/source/provider/simpleauthenticationrequest \
	ucbhelper/source/provider/simpleioerrorrequest \
	ucbhelper/source/provider/cancelcommandexecution \
	ucbhelper/source/provider/contenthelper \
	ucbhelper/source/provider/propertyvalueset \
	ucbhelper/source/provider/resultset \
	ucbhelper/source/provider/resultsetmetadata \
	ucbhelper/source/provider/simplecertificatevalidationrequest \
	ucbhelper/source/provider/contentidentifier \
	ucbhelper/source/provider/resultsethelper \
	ucbhelper/source/provider/simplenameclashresolverequest \
	ucbhelper/source/provider/simpleinteractionrequest \
	ucbhelper/source/provider/registerucb \
	ucbhelper/source/provider/contentinfo \
	ucbhelper/source/provider/interactionrequest \
	ucbhelper/source/provider/providerhelper \
	ucbhelper/source/provider/fd_inputstream \
	ucbhelper/source/provider/std_inputstream \
	ucbhelper/source/provider/std_outputstream \
	ucbhelper/source/client/proxydecider \
	ucbhelper/source/client/content \
	ucbhelper/source/client/contentbroker \
	ucbhelper/source/client/commandenvironment \
	ucbhelper/source/client/activedatastreamer \
	ucbhelper/source/client/fileidentifierconverter \
	ucbhelper/source/client/interceptedinteraction \
	ucbhelper/source/client/activedatasink \
))

# vim: set noet sw=4 ts=4:
