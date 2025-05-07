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

PRJ=.

PRJNAME=xsltml
TARGET=xsltml

.IF "$(ENABLE_MEDIAWIKI)" == "YES"

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

# --- Files --------------------------------------------------------

TARFILE_NAME=xsltml_2.1.2
TARFILE_MD5=a7983f859eafb2677d7ff386a023bc40
PATCH_FILES=$(TARFILE_NAME).patch
TARFILE_IS_FLAT:=TRUE

CONVERTFILES=\
    README \
    entities.xsl \
    glayout.xsl \
    mmltex.xsl \
    cmarkup.xsl \
    scripts.xsl \
    tables.xsl \
    tokens.xsl \

# --- Targets ------------------------------------------------------

.INCLUDE :	set_ext.mk
.INCLUDE :	target.mk
.INCLUDE :	tg_ext.mk

.ELSE
@all:
    @echo "MediaWiki Publisher extension disabled."
.ENDIF

