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

PRJ=..$/..$/..

PRJNAME=sal
TARGET=qa_rtl_uritest

ENABLE_EXCEPTIONS=TRUE
VISIBILITY_HIDDEN=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

CFLAGS+= $(LFS_CFLAGS)
CXXFLAGS+= $(LFS_CFLAGS)

CFLAGSCXX += $(CPPUNIT_CFLAGS)

# --- BEGIN --------------------------------------------------------
SHL1OBJS=  \
    $(SLO)$/rtl_Uri.obj
SHL1TARGET= rtl_uri_simple
SHL1STDLIBS= $(SALLIB) $(CPPUNITLIB)

SHL1IMPLIB= i$(SHL1TARGET)
DEF1NAME    =$(SHL1TARGET)
SHL1USE_EXPORTS = name

# END --------------------------------------------------------------

# --- BEGIN --------------------------------------------------------
SHL2OBJS=  \
    $(SLO)$/rtl_testuri.obj
SHL2TARGET= rtl_Uri
SHL2STDLIBS= $(SALLIB) $(CPPUNITLIB)

SHL2IMPLIB= i$(SHL2TARGET)
DEF2NAME    =$(SHL2TARGET)
SHL2USE_EXPORTS = name

# END --------------------------------------------------------------

#------------------------------- All object files -------------------------------
# do this here, so we get right dependencies
# SLOFILES=$(SHL1OBJS)

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : $(PRJ)$/qa$/cppunit_local.mk
