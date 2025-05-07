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

PRJNAME=hsqldb
TARGET=so_hsqldb

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

# override buildfile
ANT_BUILDFILE=build$/build.xml

.INCLUDE : antsettings.mk

.INCLUDE :  tg_javav.mk

.INCLUDE :  version.mk

.IF "$(SOLAR_JAVA)" != ""
# --- Files --------------------------------------------------------

TARFILE_NAME=hsqldb_$(HSQLDB_VERSION)
TARFILE_MD5=17410483b5b5f267aa18b7e00b65e6e0

TARFILE_ROOTDIR=hsqldb

CONVERTFILES=build$/build.xml \
            src$/org$/hsqldb$/persist$/HsqlDatabaseProperties.java \
            src$/org$/hsqldb$/Library.java

PATCH_FILES=patches$/i96823.patch \
            patches$/i97032.patch \
            patches$/i103528.patch \
            patches$/i104901.patch \
            patches$/fdo36824.patch

.IF "$(JAVANUMVER:s/.//)" >= "000100060000"
PATCH_FILES+= \
            patches$/jdbc-4.1.patch
.ENDIF

.IF "$(JAVACISGCJ)"=="yes" || "$(JAVA_SOURCE_VER)"=="1.5"
JAVA_HOME=
.EXPORT : JAVA_HOME
.ENDIF

.IF "$(JAVACISGCJ)"=="yes"
BUILD_ACTION="$(ANT)" -Dbuild.label="build-$(RSCREVISION)" -Dbuild.compiler=gcj -f $(ANT_BUILDFILE) jar
.ELSE
BUILD_ACTION="$(ANT)" -Dbuild.label="build-$(RSCREVISION)" -Dant.build.javac.source=$(JAVA_SOURCE_VER) -Dant.build.javac.target=$(JAVA_TARGET_VER) -f $(ANT_BUILDFILE) jar
.ENDIF

.IF "$(debug)"!=""
BUILD_ACTION+=-Dbuild.debug="on"
.ENDIF

.ENDIF # $(SOLAR_JAVA)!= ""

# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk

.IF "$(SOLAR_JAVA)" != ""
.INCLUDE : tg_ext.mk
.ENDIF

