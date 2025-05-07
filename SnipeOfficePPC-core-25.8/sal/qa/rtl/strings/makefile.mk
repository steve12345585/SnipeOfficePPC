#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
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

PRJ := ..$/..$/..
PRJNAME := sal

TARGET := qa_rtl_strings

ENABLE_EXCEPTIONS := TRUE
VISIBILITY_HIDDEN := TRUE

.INCLUDE: settings.mk

.IF "$(OS)" == "IOS"
CFLAGSCXX += -DCPPUNIT_PLUGIN_EXPORTED_NAME=cppunitTest_$(TARGET)
.ENDIF

CFLAGS+= $(LFS_CFLAGS)
CXXFLAGS+= $(LFS_CFLAGS)

CFLAGSCXX += $(CPPUNIT_CFLAGS)

SHL1TARGET := $(TARGET)
SHL1OBJS := \
    $(SLO)$/test_oustring_compare.obj \
    $(SLO)$/test_oustring_convert.obj \
    $(SLO)$/test_oustring_endswith.obj \
    $(SLO)$/test_oustring_noadditional.obj
SHL1IMPLIB := i$(SHL1TARGET)
SHL1STDLIBS := $(SALLIB) $(CPPUNITLIB)
SHL1USE_EXPORTS := name
DEF1NAME := $(SHL1TARGET)

.INCLUDE: target.mk
.INCLUDE: $(PRJ)$/qa$/cppunit_local.mk
