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
#***********************************************************************/

PRJ = ../..
PRJNAME = cppuhelper
TARGET = cppuhelper_cppunittester_all

ENABLE_EXCEPTIONS = TRUE
LIBTARGET = NO

.INCLUDE: settings.mk

CFLAGSCXX += $(CPPUNIT_CFLAGS)

.IF "$(OS)" != "IOS"

ALL :
    @echo This is only for iOS

.ENDIF

CFLAGSCXX += $(OBJCXXFLAGS)

.IF "$(OOO_SUBSEQUENT_TESTS)" != ""

APP1OBJS = $(OBJ)/cppuhelper_cppunittester_subsequent.obj
APP1RPATH = NONE
APP1LIBS += \
    $(SLB)/qa_propertysetmixin.lib \
    $(SLB)/cppuhelper.lib

APP1STDLIBS = $(CPPUNITLIB) $(CPPULIB) $(SALLIB) $(SALHELPERLIB)
APP1TARGET = $(TARGET)

.ELSE

APP1OBJS = $(OBJ)/cppuhelper_cppunittester_all.obj
APP1RPATH = NONE
APP1LIBS += \
    $(SLB)/cppu_ifcontainer.lib \
    $(SLB)/cppu_unourl.lib \
    $(SLB)/cppuhelper.lib

APP1STDLIBS = $(CPPUNITLIB) $(CPPULIB) $(SALLIB) $(SALHELPERLIB)
APP1TARGET = $(TARGET)

.ENDIF

.INCLUDE: target.mk
