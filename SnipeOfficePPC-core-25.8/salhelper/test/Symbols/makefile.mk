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

PRJ=..$/..$/

PRJNAME=salhelper
TARGET=dynloader
TARGET1=samplelib
TARGETTYPE=CUI
LIBTARGET=NO

ENABLE_EXCEPTIONS=TRUE

# --- Settings ---

.INCLUDE : settings.mk

# --- Files ---

#RTTI on
.IF "$(OS)" == "WNT"
CFLAGS+= -GR
.ENDIF


#---------------------------------------------------------------------------
# Build the test library which is loaded by the 
# RealDynamicLoader

SLOFILES=	$(SLO)$/samplelib.obj

LIB1TARGET=	$(SLB)$/$(TARGET1).lib
LIB1OBJFILES=	$(SLOFILES)


SHL1TARGET=	$(TARGET1)

SHL1STDLIBS= \
    $(SALLIB)

SHL1DEPN=
SHL1IMPLIB=	i$(TARGET1)
SHL1LIBS=	$(SLB)$/$(TARGET1).lib
SHL1DEF=	$(MISC)$/$(SHL1TARGET).def

DEF1NAME=	$(SHL1TARGET)
DEFLIB1NAME=	$(TARGET1)
DEF1DEPN=	$(MISC)$/$(SHL1TARGET).flt

# ------------------------------------------------------------------------------

OBJFILES=	$(OBJ)$/loader.obj

APP1TARGET=	$(TARGET)
APP1OBJS=	$(OBJFILES)

APP1STDLIBS= \
    $(SALHELPERLIB)	\
    $(SALLIB)

APP1DEF=	$(MISC)\$(APP1TARGET).def

# --- Targets ---

.INCLUDE : target.mk

$(MISC)$/$(SHL1TARGET).flt: makefile.mk
    @echo ------------------------------
    @echo Making: $@
    @echo __CT>>$@
