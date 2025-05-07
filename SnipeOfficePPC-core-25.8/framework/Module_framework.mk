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

$(eval $(call gb_Module_Module,framework))

$(eval $(call gb_Module_add_targets,framework,\
    AllLangResTarget_fwe \
    Library_fwe \
    Library_fwi \
    Library_fwk \
    Library_fwl \
    Library_fwm \
    Package_dtd \
    Package_inc \
    Package_uiconfig \
    Package_unotypes \
))

ifeq ($(ENABLE_LOMENUBAR),TRUE)
$(eval $(call gb_Module_add_targets,framework,\
    Library_lomenubar \
))
endif

$(eval $(call gb_Module_add_subsequentcheck_targets,framework,\
    JunitTest_framework_complex \
    JunitTest_framework_unoapi \
))
# vim: set noet sw=4 ts=4:
