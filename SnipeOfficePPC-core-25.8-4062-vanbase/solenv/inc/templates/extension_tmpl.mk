#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file incorporates work covered by the following license notice:
#
#   Licensed to the Apache Software Foundation (ASF) under one or more
#   contributor license agreements. See the NOTICE file distributed
#   with this work for additional information regarding copyright
#   ownership. The ASF licenses this file to you under the Apache
#   License, Version 2.0 (the "License"); you may not use this file
#   except in compliance with the License. You may obtain a copy of
#   the License at http://www.apache.org/licenses/LICENSE-2.0 .
#

############################################
#
# sample makefile for extension packing
#
# NOTE: not all of this is required or useful
# for every specific case
#
############################################

PRJ=..$/..

PRJNAME=my_project
TARGET=some_unique_target

# --- Settings -----------------------------------------------------

.INCLUDE: settings.mk
# it might be useful to have an extension wide include to set things
# like the EXTNAME variable (used for configuration processing)
# .INCLUDE :  $(PRJ)$/source$/<extension name>$/<extension_name>.pmk

# --- Files --------------------------------------------------------

# name for uniq directory
EXTENSIONNAME:=MyExtension
EXTENSION_ZIPNAME:=MyExtension

.IF "$(ENABLE_MYEXTENSION)" != "YES"
@all:
    @echo "$(EXTENSIONNAME) - extension disabled."
.ENDIF
.IF "$(SOLAR_JAVA)"==""
@all:
    @echo "$(EXTENSIONNAME) - extension requires java."
.ENDIF


# some other targets to be done



# --- Extension packaging ------------------------------------------

# these variables are optional and defaulted to the current directory

DESCRIPTION_SRC:=config$/description.xml
MANIFEST_SRC:=config$/manifest.xml
COMPONENT_CONFIGDIR:=config

# ------------------------------------------------------------------

# optional: generated list of .xcu files contained in the extension
COMPONENT_MANIFEST_GENERIC:=TRUE
COMPONENT_MANIFEST_SEARCHDIR:=registry


# variables to trigger predifined targets
# just copy:
COMPONENT_FILES= \
    $(EXTENSIONDIR)$/xMyExtension$(EXECPOST) \
    $(EXTENSIONDIR)$/some_local.html

# localized configuration files
COMPONENT_MERGED_XCU= \
    $(EXTENSIONDIR)$/registry$/data$/org$/openoffice$/Office$/Addons.xcu \
    $(EXTENSIONDIR)$/registry$/data$/org$/openoffice$/Office$/extension$/MyExtension.xcu

# other configuration files
COMPONENT_XCU= \
    $(EXTENSIONDIR)$/registry$/data$/org$/openoffice$/Office$/Data.xcu

# location of configurationfiles inside extension,
# "." for flat .xcu files
#COMPONENT_CONFIGDEST=.

# native libraries
COMPONENT_LIBRARIES= \
    $(EXTENSIONDIR)$/$(SHL1TARGET)$(DLLPOST)

# jar files
COMPONENT_JARFILES = \
    $(EXTENSIONDIR)$/MyExtension.jar

# disable fetching default OOo license text
#CUSTOM_LICENSE=my_license.txt
# override default license destination
#PACKLICS= $(EXTENSIONDIR)$/registration$/$(CUSTOM_LICENSE)

# -------------------------------
#  variables for own targets specific to this extension; no common
#  target available...
#
CONVERTER_FILE= \
    $(EXTENSIONDIR)$/xMyExtension$(EXECPOST) \

COMPONENT_DIALOGS= \
    $(EXTENSIONDIR)$/basic$/Module1.xba \
    $(EXTENSIONDIR)$/basic$/TargetChooser.xdl \
    $(EXTENSIONDIR)$/basic$/dialog.xlb \
    $(EXTENSIONDIR)$/basic$/impress.png \
    $(EXTENSIONDIR)$/basic$/script.xlb \
    $(EXTENSIONDIR)$/basic$/writer.png

# add own targets to packing dependencies (need to be done before
# packing the xtension
EXTENSION_PACKDEPS=$(CONVERTER_FILE)  $(COMPONENT_DIALOGS) makefile.mk $(CUSTOM_LICENSE)

# global settings for extension packing
.INCLUDE : extension_pre.mk
.INCLUDE : target.mk
# global targets for extension packing
.INCLUDE : extension_post.mk

# own targets 
$(CONVERTER_FILE) : $(SOLARBINDIR)$/$$(@:f)
    @@-$(MKDIRHIER) $(@:d)
    $(COPY) $< $@

$(COMPONENT_DIALOGS) : dialogs$/$$(@:f)
    @@-$(MKDIRHIER) $(@:d)
    $(COPY) $< $@

$(CUSTOM_LICENSE) : my_license.txt
    @@-$(MKDIRHIER) $(@:d)
    $(COPY) $< $@

