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

PRJ=..

PRJNAME=registry
TARGET=regtest

USE_LDUMP2=TRUE

ENABLE_EXCEPTIONS := TRUE

# --- Settings -----------------------------------------------------
.INCLUDE :  settings.mk


# ------------------------------------------------------------------
CDEFS += -DDLL_VERSION=$(EMQ)"$(DLLPOSTFIX)$(EMQ)"

CXXFILES= \
               testregcpp.cxx	\
               testmerge.cxx		


LIB1TARGET= $(SLB)$/$(TARGET).lib

LIB1OBJFILES= \
                $(SLO)$/testregcpp.obj	\
                $(SLO)$/testmerge.obj


SHL1TARGET= rgt$(DLLPOSTFIX)
SHL1IMPLIB= rgt
SHL1STDLIBS=	\
                $(SALLIB) \
                $(SALHELPERLIB)	\
                $(REGLIB) \
                $(STDLIBCPP)

SHL1LIBS=	$(LIB1TARGET)
SHL1DEPN=	$(LIB1TARGET)
SHL1DEF=	$(MISC)$/$(SHL1TARGET).def
DEF1NAME=	$(SHL1TARGET)
             
DEF1DEPN	=$(MISC)$/rgt$(DLLPOSTFIX).flt $(SLOFILES)
DEFLIB1NAME =$(TARGET)
DEF1DES 	=Registry Runtime - TestDll

# --- Targets ------------------------------------------------------

#all: \
#	ALLTAR	

.INCLUDE :  target.mk

# --- SO2-Filter-Datei ---


$(MISC)$/rgt$(DLLPOSTFIX).flt:
    @echo ------------------------------
    @echo Making: $@
    @echo WEP>$@
    @echo LIBMAIN>>$@
    @echo LibMain>>$@


