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

PRJ=..$/..

PRJNAME=apache-commons
TARGET=commons-logging

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

# override buildfile
ANT_BUILDFILE=build.xml

.INCLUDE : antsettings.mk

.IF "$(SOLAR_JAVA)" != "" && ( "$(ENABLE_MEDIAWIKI)" == "YES" || "$(ENABLE_REPORTBUILDER)" == "YES" )
# --- Files --------------------------------------------------------

TARFILE_NAME=commons-logging-1.1.1-src
TARFILE_MD5=3c219630e4302863a9a83d0efde889db

TARFILE_ROOTDIR=commons-logging-1.1.1-src

PATCH_FILES=$(PRJ)$/patches$/logging.patch

CONVERTFILES=build.xml
                
OUT2CLASS=target$/commons-logging-1.1.1-SNAPSHOT.jar

ANT_OPTS+="-Dfile.encoding=ISO-8859-1"
.EXPORT : ANT_OPTS

.IF "$(SYSTEM_TOMCAT)" != "YES"
SERVLETAPI_JAR := $(SOLARVER)$/$(INPATH)$/bin$(UPDMINOREXT)$/servlet-api.jar
.ENDIF

.IF "$(JAVACISGCJ)"=="yes"
JAVA_HOME=
.EXPORT : JAVA_HOME
BUILD_ACTION="$(ANT)" -Dbuild.label="build-$(RSCREVISION)" -Dbuild.compiler=gcj -Dservletapi.jar=$(SERVLETAPI_JAR) -f $(ANT_BUILDFILE) compile build-jar
.ELSE
BUILD_ACTION="$(ANT)" -Dbuild.label="build-$(RSCREVISION)" -f $(ANT_BUILDFILE) -Dservletapi.jar=$(SERVLETAPI_JAR) compile build-jar
.ENDIF

.ENDIF # $(SOLAR_JAVA)!= ""

# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk

.IF "$(SOLAR_JAVA)" != "" && ( "$(ENABLE_MEDIAWIKI)" == "YES" || "$(ENABLE_REPORTBUILDER)" == "YES" )
.INCLUDE : tg_ext.mk
.ENDIF

